/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:       Y2PM.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Core class providing access to all components of the
  Package Management creating them on demand.

/-*/

#include <iostream>
#include <stdlib.h>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>

#include <y2pm/InstTarget.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectionManager.h>
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/InstYou.h>

using namespace std;

///////////////////////////////////////////////////////////////////
#undef Y2LOG
#define Y2LOG "Y2PM"
///////////////////////////////////////////////////////////////////

/*
 * get LangCode from LANG
 */
static LangCode getLangEnvironment()
{
    char *lang = getenv ("LANG");
    if (lang == 0)
	return LangCode ("en");
    string langstr (lang);
// segfaults -- ln
//    MIL << "LANG=" << langstr << endl;
    string::size_type sizepos = langstr.find ("@");	// cut off "@"
    if (sizepos != string::npos)
	langstr = langstr.substr (0, sizepos);
    sizepos = langstr.find (".");		// cut off "."
    if (sizepos != string::npos)
	return LangCode (langstr.substr (0, sizepos));
    return LangCode (langstr);
}

/******************************************************************
**
**
**	FUNCTION NAME : initGettext
**	FUNCTION TYPE : static void
**
**	DESCRIPTION :
*/
static void initGettext()
{
  static bool done = false;
  if ( !done ) {
    bindtextdomain( "rpm-groups", LOCALEDIR );
    bind_textdomain_codeset( "rpm-groups", "utf8" );
    done = true;
  }
}

///////////////////////////////////////////////////////////////////
// global settings
///////////////////////////////////////////////////////////////////
#warning MUST INIT GLOBAL SETTINGS
#warning Provide a serial number for localesttings. DP could cache localedependent data.

Pathname Y2PM::_instTarget_rootdir( "/" );
Pathname Y2PM::_system_rootdir    ( "/" );
bool Y2PM::_cache_to_ramdisk( true );
LangCode Y2PM::_preferred_locale (getLangEnvironment());
std::list<LangCode> Y2PM::_requested_locales;
PkgArch Y2PM::_base_arch;
std::list<PkgArch> Y2PM::_allowed_archs;

///////////////////////////////////////////////////////////////////
// components provided
///////////////////////////////////////////////////////////////////

InstTarget * Y2PM::_instTarget = 0;

InstSrcManager * Y2PM::_instSrcManager = 0;

PMPackageManager *   Y2PM::_packageManager = 0;

PMSelectionManager * Y2PM::_selectionManager = 0;

PMYouPatchManager *  Y2PM::_youPatchManager = 0;

void Y2PM::cleanupAtExit()
{
  if ( _youPatchManager ) {
    delete _youPatchManager;
    _youPatchManager = 0;
  }
}

///////////////////////////////////////////////////////////////////
// packageSelection [package/selectionManager]
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::packageSelectionSaveState
//	METHOD TYPE : void
//
void Y2PM::packageSelectionSaveState() {
  selectionManager().SaveState();
  packageManager().SaveState();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::packageSelectionRestoreState
//	METHOD TYPE : bool
//
bool Y2PM::packageSelectionRestoreState() {
  bool ret = selectionManager().RestoreState();
  return packageManager().RestoreState() && ret; // try to restore both but report if either failed
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::packageSelectionDiffState
//	METHOD TYPE : bool
//
bool Y2PM::packageSelectionDiffState() {
  return selectionManager().DiffState() || packageManager().DiffState();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::packageSelectionClearSaveState
//	METHOD TYPE : void
//
void Y2PM::packageSelectionClearSaveState() {
  selectionManager().ClearSaveState();
  packageManager().ClearSaveState();
}

///////////////////////////////////////////////////////////////////
// CallBacks
///////////////////////////////////////////////////////////////////

Y2PM::CallBacks Y2PM::_callbacks = Y2PM::CallBacks::CallBacks();

Y2PM::CallBacks::CallBacks()
{
    _provide_start_func = NULL;
    _provide_start_data = NULL;
    _provide_progress_func = NULL;
    _provide_progress_data = NULL;
    _provide_done_func = NULL;
    _provide_done_data = NULL;
    _package_start_func = NULL;
    _package_start_data = NULL;
    _package_progress_func = NULL;
    _package_progress_data = NULL;
    _package_done_func = NULL;
    _package_done_data = NULL;
    _rebuilddb_progress_func = NULL;
    _rebuilddb_progress_data = NULL;
    _source_change_func = NULL;
    _source_change_data = NULL;
};


/**
 * called right before package 'name' is provided
 * */
void
Y2PM::setProvideStartCallback(void (*func)(const std::string& name, const FSize&, bool, void*), void* data)
{
    _callbacks._provide_start_func = func;
    _callbacks._provide_start_data = data;
}

/**
 * called while package providal is in progress
 * */
void
Y2PM::setProvideProgressCallback(void (*func)(int percent, void*), void* data)
{
#warning Pass 'provide progress' callback to Media
    _callbacks._provide_progress_func = func;
    _callbacks._provide_progress_data = data;
}

/**
 * called right after a package was provided
 * */
void
Y2PM::setProvideDoneCallback(std::string (*func)(PMError err, const std::string&, const std::string&, void*), void* data)
{
    _callbacks._provide_done_func = func;
    _callbacks._provide_done_data = data;
}

/**
 * called right before package 'name' is installed or deleted
 * */
void
Y2PM::setPackageStartCallback(bool (*func)(const std::string& name, const std::string& summary, const FSize& size, bool is_delete, void*), void* data)
{
    _callbacks._package_start_func = func;
    _callbacks._package_start_data = data;
}

/**
 * called multiple times during package installation, 'progress' is the
 * already installed percentage
 * */
void
Y2PM::setPackageProgressCallback(void (*func)(int percent, void*), void* data)
{
    _callbacks._package_progress_func = func;
    _callbacks._package_progress_data = data;
    instTarget().setPackageInstallProgressCallback (func, data);
}

/**
 * called after a package got installed or deleted
 * */
void
Y2PM::setPackageDoneCallback(std::string (*func)(PMError, const std::string&, void*), void* data)
{
    _callbacks._package_done_func = func;
    _callbacks._package_done_data = data;
}


/**
 * called multiple times during rpm rebuilddb, 'progress' is the
 * rebuild progress
 * */
void
Y2PM::setRebuildDBProgressCallback(void (*func)(int percent, void*), void* data)
{
    _callbacks._rebuilddb_progress_func = func;
    _callbacks._rebuilddb_progress_data = data;
    instTarget().setRebuildDBProgressCallback(func, data);
}


/**
 * called when switching sources during package commit (install loop)
 * informal callback for user interface, no user interaction necessary
 * */
void
Y2PM::setSourceChangeCallback(void (*func)(InstSrcManager::ISrcId srcid, int medianr, void*), void* data)
{
    _callbacks._source_change_func = func;
    _callbacks._source_change_data = data;
}

///////////////////////////////////////////////////////////////////


PkgArch
Y2PM::baseArch(void)
{
#warning TBD init _base_arch from product
    if (((const std::string &)_base_arch).empty())
    {
	_base_arch = instTarget().baseArch();
    }

    return _base_arch;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::instTarget
//	METHOD TYPE : InstTarget &
//
InstTarget & Y2PM::instTarget(bool do_start, Pathname root)
{
    if ( !_instTarget )
    {
        initGettext();
	MIL << "Launch InstTarget..." << endl;
	_instTarget = new InstTarget ();
	MIL << "Created InstTarget" << endl;
    }

    if (do_start)
    {
	MIL << "Init InstTarget at '" << root << "'..." << endl;
	_instTarget_rootdir = root;
	PMError dbstat = Y2PM::instTarget().init(_instTarget_rootdir, false);
	if( dbstat != InstTargetError::E_ok )
	{
	    ERR << "error initializing target: " << dbstat << endl;
#warning error value dropped
	}
	else
	{
	    // this will start the package manager
	    Y2PM::packageManager().poolSetInstalled( Y2PM::instTarget().getPackages () );
	    // this will start the Selection manager
	    Y2PM::selectionManager().poolSetInstalled( Y2PM::instTarget().getSelections () );
	}
    }

    return *_instTarget;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::noAutoInstSrcManager
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool Y2PM::noAutoInstSrcManager()
{
  if ( !_instSrcManager ) {
    initGettext();
    MIL << "Launch InstSrcManager (no auto enable)..." << endl;
    _instSrcManager = new InstSrcManager ( /*autoEnable*/false );
    MIL << "Created InstSrcManager (no auto enable) @" << _instSrcManager << endl;
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::instSrcManager
//	METHOD TYPE : InstSrcManager &
//
//	DESCRIPTION :
//
InstSrcManager & Y2PM::instSrcManager()
{
  if ( !_instSrcManager ) {
    initGettext();
    MIL << "Launch InstSrcManager..." << endl;
    _instSrcManager = new InstSrcManager ();
    MIL << "Created InstSrcManager @" << _instSrcManager << endl;
  }
  return *_instSrcManager;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::packageManager
//	METHOD TYPE : PMPackageManager &
//
//	DESCRIPTION :
//
PMPackageManager & Y2PM::packageManager()
{
  if ( !_packageManager )
  {
    initGettext();
    MIL << "Launch PackageManager..." << endl;
    _packageManager = new PMPackageManager;
    MIL << "Created PackageManager @" << _packageManager << endl;

  }
  return *_packageManager;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::selectionManager
//	METHOD TYPE : PMSelectionManager &
//
//	DESCRIPTION :
//
PMSelectionManager & Y2PM::selectionManager()
{
  if ( !_selectionManager ) {
    initGettext();
    MIL << "Launch SelectionManager..." << endl;
    _selectionManager = new PMSelectionManager;
    MIL << "Created SelectionManager @" << _selectionManager << endl;
  }
  return *_selectionManager;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::youPatchManager
//	METHOD TYPE : PMYouPatchManager &
//
//	DESCRIPTION :
//
PMYouPatchManager & Y2PM::youPatchManager()
{
  if ( !_youPatchManager ) {
    initGettext();
    MIL << "Launch YouPatchManager..." << endl;

    _youPatchManager = new PMYouPatchManager;

    _youPatchManager->instYou().initProduct();

    _youPatchManager->instYou().initDu();

    list<PMYouPatchPtr> patches = Y2PM::instTarget().getPatches();

    _youPatchManager->poolSetInstalled( patches );

    list<PMYouPatchPtr>::const_iterator itPatch;
    for( itPatch = patches.begin(); itPatch != patches.end(); ++itPatch ) {
      Y2PM::packageManager().poolAddCandidates( (*itPatch)->packages() );
    }

    atexit( &cleanupAtExit );

    MIL << "Created YouPatchManager @" << _youPatchManager << endl;
  }
  return *_youPatchManager;
}


/******************************************************************
**
**
**	FUNCTION NAME : commitSucceeded
**	FUNCTION TYPE : inline void
*/
inline void commitSucceeded( const PMPackagePtr & pkg_r )
{
  // adjust selectables state
  if ( pkg_r && pkg_r->hasSelectable() ) {
    pkg_r->getSelectable()->user_unset();
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : commitSrcSucceeded
**	FUNCTION TYPE : inline void
*/
inline void commitSrcSucceeded( const PMPackagePtr & pkg_r )
{
  // adjust selectables state
  if ( pkg_r && pkg_r->hasSelectable() ) {
    pkg_r->getSelectable()->set_source_install( false );
  }
}


/******************************************************************
**
**
**	FUNCTION NAME : installSpmFromMedia
**	FUNCTION TYPE : bool
**
**	install spms from package source 'current_src_ptr',
**	  media number 'current_src_media'
**	loop through srclist and pick all spms matching the wanted
**	  media
*/
bool
Y2PM::installSpmFromMedia (unsigned int current_src_media,
			   constInstSrcPtr current_src_ptr,
			   std::list<PMPackagePtr>& srclist)
{
    // no-op if we don't have a medium yet

    if (current_src_media == 0)
	return true;
    if (current_src_ptr == 0)
	return true;

    bool go_on = true;
    bool retry;		// flag for inner 'retry' loops
    PMError err;

    //---------------------------------------------
    // install spm packages while the current media is still available

    //-------------------------------------------------------
    // loop over all source rpms (.srpm) selected for installation

    for (std::list<PMPackagePtr>::iterator it = srclist.begin();
	 it != srclist.end();)				// NO ++it here, see erase() at bottom !
    {
	string srcloc = (*it)->sourceloc();
	if (srcloc.empty())
	{
	    ERR << "No source location for " << (*it)->name() << endl;
	    ++it;
	    continue;
	}

	if ((*it)->source() != current_src_ptr)			// wrong package source
	{
	    ++it;
	    continue;
	}

	unsigned int spmmedia = atoi (srcloc.c_str());

	if (spmmedia != current_src_media)			// wrong media
	{
	    ++it;
	    continue;
	}

	bool is_remote = (*it)->isRemote();		// if current package source is remote

	//---------------------------------------------------
	// fetch (provide) source package for installation

	do		// retry loop for package providing (fetching rpm from possibly remote source)
	{
	    retry = false;	// default: don't retry

	    // if source is remote, show progress bar while fetching package

	    if (is_remote
		&& (_callbacks._provide_start_func != 0))
	    {
		(*_callbacks._provide_start_func)(srcloc, (*it)->sourcesize(), true, _callbacks._provide_start_data);
	    }

	    Pathname path;
	    err = (*it)->provideSrcPkgToInstall(path);

	    if ((err || is_remote)
		&& (_callbacks._provide_done_func != 0))
	    {
		std::string done_result = (*_callbacks._provide_done_func)(err, err.errstr(), (*it)->name(), _callbacks._provide_done_data);

		// check for "" (ok), "R" retry, "I" ignore err, "C" cancel all, "S" skip remaining
		if (done_result == "C")
		{
		    err = InstSrcError::E_cancel_media;		// cancel it all
		}
		else if (done_result == "S")
		{
		    err = InstSrcError::E_skip_media;		// skip current media
		}
		else if (done_result == "R")
		{
		    retry = true;				// retry !
		}
	    }
	}
	while (retry);

	if (err != PMError::E_ok)				// pack source provide
	{
	    ++it;
	    continue;
	}

	//---------------------------------------------------
	// install provided source package

	Pathname path;

	do
	{
	    retry = false;	// default: don't retry

	    if (_callbacks._package_start_func)
	    {
		go_on = (*_callbacks._package_start_func) (srcloc, (*it)->summary(), (*it)->sourcesize(), false, _callbacks._package_start_data);
		if (!go_on)
		{
		    break;
		}
	    }

	    err = instTarget().installPackage (path);
	    if ( ! err ) {
	      commitSrcSucceeded( *it );
	    }

	    if (_callbacks._package_done_func)
	    {
		std::string done_result = (*_callbacks._package_done_func) (err, err.errstr(), _callbacks._package_done_data);

		// check for "" (ok), "R" retry, "I" ignore err, "C" cancel all, "S" skip remaining
		if (done_result == "C")
		{
		    err = InstSrcError::E_cancel_media;		// cancel it all
		    go_on = false;
		}
		else if (done_result == "S")
		{
		    err = InstSrcError::E_skip_media;		// skip current media
		}
		else if (done_result == "R")
		{
		    retry = true;				// retry !
		}
	    }
	}
	while (retry);

	if (!go_on)
	{
	    break;
	}

	if (err == PMError::E_ok)
	{
	    it = srclist.erase(it);			// ok, take out of list
	}
	else
	{
	    ++it;					// bad, keep in list
	}

    } // loop over source packages

    return go_on;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::commitPackages
//	METHOD TYPE : int
//
//	DESCRIPTION : commit all changes in the package manager
//		==> actually delete/install packages
//		relies on callbacks for media change
//		!! DOES NOT SOLVE !!
//
//	if media_nr is != 0, only packages from this media are
//	installed. media_nr==0 means install all packages from all media.
//
//	returns number of sucessfully installed packages
//	  negated if the installation was aborted
//
//	returns failed packages in 'errors_r'
//	returns uninstalled packages (because media not available) in 'remaining_r'
//	returns uninstalled source packages in 'srcremaining_r'
//
int Y2PM::commitPackages( unsigned int media_nr,
			  std::list<std::string>& errors_r,
			  std::list<std::string>& remaining_r,
			  std::list<std::string>& srcremaining_r,
			  InstSrcManager::ISrcIdList installrank )
{
    int count = 0;
    bool go_on = true;

    errors_r.clear();
    remaining_r.clear();
    srcremaining_r.clear();

    std::list<PMPackagePtr> dellist;
    std::list<PMPackagePtr> inslist;
    std::list<PMPackagePtr> srclist;

    if (installrank.empty())
    {
	packageManager().getPackagesToInsDel (dellist, inslist, srclist);	// compute order
    }
    else
    {
	MIL << "ranked install !" << endl;
	packageManager().getPackagesToInsDel (dellist, inslist, srclist, installrank);	// compute order
    }

    bool retry;		// flag for inner 'retry' loops

    //-----------------------------------------------------
    // first, remove all packages marked for deletion

    for (std::list<PMPackagePtr>::iterator it = dellist.begin();
	 it != dellist.end(); ++it)
    {
	string fullname = (*it)->nameEd();

	do		// retry loop for package deletion
	{
	    retry = false;	// default: don't retry

	    if (_callbacks._package_start_func)
	    {
		go_on = (*_callbacks._package_start_func) (fullname, (*it)->summary(), (*it)->size(), true, _callbacks._package_start_data);
		if (!go_on)
		{
		    return 0;		// user cancelled it all
		}
	    }
	    PMError err = instTarget().removePackage (*it);
	    if ( ! err ) {
	      commitSucceeded( *it );
	    }

	    if (_callbacks._package_done_func)
	    {
		// show deletion result to user, if err is set, pops up a window with buttons
		std::string done_result = (*_callbacks._package_done_func) (err, err.errstr(), _callbacks._package_done_data);

		// check for "" (ignore), "R" retry, "C" cancel/abort all, "S" skip remaining
		if (done_result == "C")
		{
		    return 0;				// cancel all
		}
		else if (done_result == "S")
		{
		    go_on = false;			// skip remaining
		}
		else if (done_result == "R")
		{
		    retry = true;			// retry !
		}
		// default: ok/ignore
	    }
	}
	while (retry);

	if (!go_on)
	    break;
    }


    ///////////////////////////////////////////////////////////////////
    // One may argue whether selection data should be installed before
    // or after any packages. Doing it before has the benefit, that the
    // selection DB reflects what the user wanted. In case of trouble it
    // should be easier to check and manualy repair.
    ///////////////////////////////////////////////////////////////////
    PMError err = selectionManager().installOnTarget();
    if ( err ) {
      ERR << "Error installing selection data." << err << endl;
    }

    ///////////////////////////////////////////////////////////////////
    // install loop
    ///////////////////////////////////////////////////////////////////

    unsigned int current_src_media = 0;			// number of currently attached media
    constInstSrcPtr current_src_ptr = 0;		// pointer to media handler
    unsigned int pkgmedianr = 0;			// media number of current package

    for (std::list<PMPackagePtr>::iterator it = inslist.begin();
	 it != inslist.end(); ++it)
    {
	pkgmedianr = (*it)->medianr();
	if ((media_nr > 0)				// if a specific media number is requested
	    && (pkgmedianr != media_nr))		// and the current package is not on this media
	{
#warning loosing version information
	    remaining_r.push_back ((*it)->name());	// push it to the remaining list for later
	    continue;
	}

	bool is_remote = (*it)->isRemote();		// if current package source is remote
	string fullname = (*it)->nameEd();

	//-----------------------------------------------------------
	// check if we need a new media

	if (((*it)->source() != current_src_ptr)	// source or media change
	    || (pkgmedianr != current_src_media))
	{
	    go_on = installSpmFromMedia (current_src_media, current_src_ptr, srclist);	// install sources from it while we have it attached

	    if (((*it)->source() != current_src_ptr)	// source change -> release old source media
		&& (current_src_ptr != 0))		// if we have an old media attached
	    {
		InstSrcPtr ptr = InstSrcPtr::cast_away_const (current_src_ptr);
		ptr->releaseMedia (true);	// release if removable (CD/DVD)
	    }

	    current_src_ptr = (*it)->source();
	    current_src_media = pkgmedianr;

	    if (_callbacks._source_change_func != 0)
	    {
		(*_callbacks._source_change_func)(current_src_ptr, pkgmedianr, _callbacks._source_change_data);
	    }
	}

	//-----------------------------------------------------------
	// fetch (provide) the binary package to install

	Pathname path;

	do		// retry loop for package providing (fetching rpm from possibly remote source)
	{
	    retry = false;

	    // showing a progress bar only makes sense for really remote sources
	    if (is_remote
		&& (_callbacks._provide_start_func != 0))
	    {
		(*_callbacks._provide_start_func)(fullname, (*it)->archivesize(), true, _callbacks._provide_start_data);
	    }

	    err = (*it)->providePkgToInstall(path);	// fetch package from source for later installation

	    if ((err || is_remote)
		&& (_callbacks._provide_done_func != 0))
	    {
		std::string done_result = (*_callbacks._provide_done_func)(err, err.errstr(), (*it)->name(), _callbacks._provide_done_data);

		// check for "" (ignore), "R" retry, "C" cancel/abort all, "S" skip remaining
		if (done_result == "C")
		{
		    err = InstSrcError::E_cancel_media;		// cancel it all
		}
		else if (done_result == "S")
		{
		    err = InstSrcError::E_skip_media;		// skip current media
		}
		else if (done_result == "R")
		{
		    retry = true;				// retry !
		}
	    }
	}
	while (retry);

	switch (err)
	{
	    case PMError::E_ok:
	    break;
	    case InstSrcError::E_cancel_media:		// cancel all
	    {
		while (it != inslist.end())
		{
		    remaining_r.push_back ((*it)->name());
		    ++it;
		}
	    }
	    break;
	    case InstSrcError::E_skip_media:		// skip current for this source
	    {
		while (it != inslist.end())
		{
		    if (((*it)->medianr() != pkgmedianr)		// break on next media
			|| (current_src_ptr != (*it)->source()))	// or next source
		    {
			--it;
			break;
		    }
		    remaining_r.push_back ((*it)->name());
		    ++it;
		}
		if (it != inslist.end())
		    continue;			// go on with next package/source
	    }
	    break;
	    default:
		ERR << "Media can't provide package to install for " << (*it) << ":" << err.errstr() << endl;
		remaining_r.push_back ((*it)->name());
		continue;
	        break;
	}

	// skip_media or cancel_media might have advanced the iterator until the end
	if (it == inslist.end())
	    break;

	//-----------------------------------------------------------
	// install the binary package to install

	do		// retry loop for package deletion
	{
	    retry = false;	// default: don't retry

	    if (_callbacks._package_start_func)
	    {
		go_on = (*_callbacks._package_start_func) (fullname, (*it)->summary(), (*it)->size(), false, _callbacks._package_start_data);
		if (!go_on)
		{
		    break;		// user cancelled it all
		}
	    }

	    err = instTarget().installPackage (path);
	    if ( ! err ) {
	      commitSucceeded( *it );
	    }

	    if (_callbacks._package_done_func)
	    {
		std::string done_result = (*_callbacks._package_done_func) (err, err.errstr(), _callbacks._package_done_data);

		// check for "" (ok), "R" retry, "I" ignore err, "C" cancel all, "S" skip remaining
		if (done_result == "C")
		{
		    err = InstSrcError::E_cancel_media;		// cancel it all
		}
		else if (done_result == "S")
		{
		    err = InstSrcError::E_skip_media;		// skip current media
		}
		else if (done_result == "R")
		{
		    retry = true;				// retry !
		}
	    }
	}
	while (retry);
	if (!go_on)
	{
	    break;
	}

	if (err)
	{
	    errors_r.push_back ((*it)->name());
	}
	else
	{
	    count++;
	}

    } // loop over inslist


    // all binary packages installed
    // install remaining sources

    while (go_on)
    {
	go_on = installSpmFromMedia (current_src_media, current_src_ptr, srclist);	// install sources from it while we have it attached
	if (!go_on)
	    break;

	if (media_nr > 0)				// if a specific media number is requested
	    break;

	if (srclist.size() == 0)			// we're done
	    break;

	// peek to first package in source list to determine which is the next medium

	PMPackagePtr spm = srclist.front();

	if ((spm->source() != current_src_ptr)		// source or media change ?
	    || (spm->medianr() != current_src_media))
	{
	    if ((spm->source() != current_src_ptr)	// source change -> release old source media
		&& (current_src_ptr != 0))		// if we have an old media attached
	    {
		InstSrcPtr ptr = InstSrcPtr::cast_away_const (current_src_ptr);
		ptr->releaseMedia (true);	// release if removable (CD/DVD)
	    }

	    current_src_ptr = spm->source();
	    current_src_media = spm->medianr();

	    if (_callbacks._source_change_func != 0)
	    {
		(*_callbacks._source_change_func)(current_src_ptr, pkgmedianr, _callbacks._source_change_data);
	    }
	}
    }

    if (current_src_ptr != 0)
    {
	InstSrcPtr ptr = InstSrcPtr::cast_away_const (current_src_ptr);
	ptr->releaseMedia (false);		// release any media
    }

    // copy remaining sources to srcremaining_r

    for (std::list<PMPackagePtr>::iterator it = srclist.begin();
	 it != srclist.end(); ++it)
    {
	srcremaining_r.push_back ((*it)->name());
    }

    return (go_on ? count : -count);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::installFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : install a single, locally availabe rpm file, uses callbacks !
PMError
Y2PM::installFile (const Pathname& path)
{
    if (_callbacks._package_start_func)
	(*_callbacks._package_start_func) (path.asString(), "", 0, false, _callbacks._package_start_data);

    PMError err = instTarget().installPackage (path, RpmDb::RPMINST_NONE);

    if (_callbacks._package_done_func)
	(*_callbacks._package_done_func) (err, err.errstr(), _callbacks._package_done_data);

    return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::removePackage
//	METHOD TYPE : PMError
//
//	DESCRIPTION : remove a single package by name, uses callbacks !
PMError
Y2PM::removePackage (const std::string& pkgname)
{
    if (_callbacks._package_start_func)
	(*_callbacks._package_start_func) (pkgname, "", 0, true, _callbacks._package_start_data);

    PMError err = instTarget().removePackage (pkgname);

    if (_callbacks._package_done_func)
	(*_callbacks._package_done_func) (err, err.errstr(), _callbacks._package_done_data);

    return err;
}
