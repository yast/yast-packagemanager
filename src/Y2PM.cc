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

#include <y2util/Y2SLog.h>

#include <Y2PM.h>

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
    MIL << "LANG=" << langstr << endl;
    string::size_type sizepos = langstr.find ("@");	// cut off "@"
    if (sizepos != string::npos)
	langstr = langstr.substr (0, sizepos);
    sizepos = langstr.find (".");		// cut off "."
    if (sizepos != string::npos)
	return LangCode (langstr.substr (0, sizepos));
    return LangCode (langstr);
}
///////////////////////////////////////////////////////////////////
// global settings
///////////////////////////////////////////////////////////////////
#warning MUST INIT GLOBAL SETTINGS

Pathname Y2PM::_instTarget_rootdir( "/" );
Pathname Y2PM::_system_rootdir    ( "/" );
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
Y2PM::setProvideDoneCallback(void (*func)(PMError err, const std::string&, void*), void* data)
{
    _callbacks._provide_done_func = func;
    _callbacks._provide_done_data = data;
}

/**
 * called right before package 'name' is installed or deleted
 * */
void
Y2PM::setPackageStartCallback(void (*func)(const std::string& name, const std::string& summary, const FSize& size, bool is_delete, void*), void* data)
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
Y2PM::setPackageDoneCallback(void (*func)(PMError, const std::string&, void*), void* data)
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
//	DESCRIPTION :
//
InstTarget & Y2PM::instTarget(bool do_start, Pathname root)
{
    if ( !_instTarget )
    {
	MIL << "Launch InstTarget... ()" << endl;
	_instTarget = new InstTarget ();
	MIL << "Created InstTarget" << endl;
    }

    if (do_start)
    {
	WAR << "Fake InstTarget and load installed Packages..." << endl;
	_instTarget_rootdir = root;
	PMError dbstat = Y2PM::instTarget().init(_instTarget_rootdir, false);
	if( dbstat != InstTargetError::E_ok )
	{
	    ERR << "error initializing target: " << dbstat << endl;
	}
	else
	{
	    // this will start the package manager
	    Y2PM::packageManager().poolSetInstalled( Y2PM::instTarget().getPackages () );
	}
    }

    return *_instTarget;
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
    MIL << "Launch YouPatchManager..." << endl;

    _youPatchManager = new PMYouPatchManager;

    _youPatchManager->instYou().initProduct();

    list<PMYouPatchPtr> patches = Y2PM::instTarget().getPatches();

    _youPatchManager->poolSetInstalled( patches );

    list<PMYouPatchPtr>::const_iterator itPatch;
    for( itPatch = patches.begin(); itPatch != patches.end(); ++itPatch ) {
      Y2PM::packageManager().poolAddCandidates( (*itPatch)->packages() );
    }

    MIL << "Created YouPatchManager @" << _youPatchManager << endl;
  }
  return *_youPatchManager;
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
//
//	returns failed packages in 'errors'
//	returns uninstalled packages (because media not available) in 'remaining'
//
int
Y2PM::commitPackages (unsigned int media_nr, std::list<std::string>& errors, std::list<std::string>& remaining)
{
    int count = 0;

    std::list<PMPackagePtr> dellist;
    std::list<PMPackagePtr> inslist;

    packageManager().getPackagesToInsDel (dellist, inslist);	// compute order

    for (std::list<PMPackagePtr>::iterator it = dellist.begin();
	 it != dellist.end(); ++it)
    {
	string fullname = (*it)->nameEd();

	if (_callbacks._package_start_func)
	    (*_callbacks._package_start_func) (fullname, (*it)->summary(), (*it)->size(), true, _callbacks._package_start_data);

	PMError err = instTarget().removePackage (*it);

	if (_callbacks._package_done_func)
	    (*_callbacks._package_done_func) (err, "", _callbacks._package_done_data);

    }

    // install loop

    for (std::list<PMPackagePtr>::iterator it = inslist.begin();
	 it != inslist.end(); ++it)
    {
	unsigned int pkgmedianr = (*it)->medianr();
	if ((media_nr > 0)
	    && (pkgmedianr != media_nr))
	{
#warning loosing version information
	    remaining.push_back ((*it)->name());
	    continue;
	}

	bool is_remote = (*it)->isRemote();
	string fullname = (*it)->nameEd();

	if (is_remote
	    && (_callbacks._provide_start_func != 0))
	    (*_callbacks._provide_start_func)(fullname, (*it)->archivesize(), true, _callbacks._provide_start_data);

	Pathname path;
	PMError err = (*it)->providePkgToInstall(path);

	if (is_remote
	    && (_callbacks._provide_done_func != 0))
	    (*_callbacks._provide_done_func)(err, "", _callbacks._provide_done_data);

	if (err)
	{
	    ERR << "Media can't provide package to install for " << (*it) << ":" << err.errstr() << endl;
	    remaining.push_back ((*it)->name());
	    continue;
	}

	if (_callbacks._package_start_func)
	    (*_callbacks._package_start_func) (fullname, (*it)->summary(), (*it)->size(), false, _callbacks._package_start_data);

	err = instTarget().installPackage (path);

	if (_callbacks._package_done_func)
	    (*_callbacks._package_done_func) (err, "", _callbacks._package_done_data);

	if (err)
	{
	    errors.push_back ((*it)->name());
	}
	else
	{
	    count++;
	}
    } // loop over inslist

    return count;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::installFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : install a single rpm file,  uses callbacks !
PMError
Y2PM::installFile (const Pathname& path)
{
    if (_callbacks._package_start_func)
	(*_callbacks._package_start_func) (path.asString(), "", 0, false, _callbacks._package_start_data);

    PMError err = instTarget().installPackage (path);

    if (_callbacks._package_done_func)
	(*_callbacks._package_done_func) (err, "", _callbacks._package_done_data);

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
	(*_callbacks._package_done_func) (err, "", _callbacks._package_done_data);

    return err;
}
