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
#include <y2pm/Y2PMCallbacks.h>

#include <y2pm/InstTarget.h>
#include <y2pm/InstTargetError.h>
#include <y2pm/InstSrcManager.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectionManager.h>
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/InstYou.h>

using namespace std;
using namespace Y2PMCallbacks;

///////////////////////////////////////////////////////////////////
#undef Y2LOG
#define Y2LOG "Y2PM"
///////////////////////////////////////////////////////////////////

/******************************************************************
**
**
**	FUNCTION NAME : getLangEnvironment
**	FUNCTION TYPE : static LangCode
**
** Get LangCode from LANG environment variable.
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

///////////////////////////////////////////////////////////////////
// global settings
///////////////////////////////////////////////////////////////////
#warning MUST INIT GLOBAL SETTINGS
#warning Provide a serial number for localesttings. DP could cache localedependent data.

Pathname Y2PM::_system_rootdir( "/" );
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::baseArch
//	METHOD TYPE : PkgArch
//
PkgArch Y2PM::baseArch()
{
#warning TBD init _base_arch from product
  if ( _base_arch->empty() ) {
    _base_arch = InstTarget::baseArch();
  }
  return _base_arch;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::instTarget
//	METHOD TYPE : InstTarget &
//
InstTarget & Y2PM::instTarget()
{
  if ( !_instTarget ) {
    MIL << "Launch InstTarget..." << endl;
    _instTarget = new InstTarget();
    MIL << "Created InstTarget" << endl;
  }
  return *_instTarget;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::instTargetInit
//	METHOD TYPE : PMError
//
PMError Y2PM::instTargetInit( Pathname root_r )
{
  PMError err;

  if ( instTarget().initialized() ) {

    if ( ! root_r.empty() && root_r != instTarget().rootdir() ) {
      err = InstTargetError::E_already_initialized;
      ERR << "Init InstTarget at '" << root_r << "' failed: " << err << " (" << instTarget().rootdir() << ")" << endl;
    }

  } else {

    // initialize target
    if ( root_r.empty() ) {
      root_r = "/";
    }
    MIL << "Init InstTarget at '" << root_r << "'..." << endl;
    err = instTarget().init( root_r );

    if ( err ) {
      ERR << "Init InstTarget at '" << root_r << "' returned " << err << endl;

    } else {
      MIL << "InstTarget initialized at '" << root_r << "'" << endl;

      // provide data to already existing managers
      if ( _packageManager ) {
	_packageManager->poolSetInstalled( instTarget().getPackages() );
      }
      if ( _selectionManager ) {
	_selectionManager->poolSetInstalled( instTarget().getSelections() );
      }
    }

  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::instTargetUpdate
//	METHOD TYPE : PMError
//
PMError Y2PM::instTargetUpdate()
{
  MIL << "InstTarget Update ..." << endl;
  packageManager();               // make shure it exists
  selectionManager();             // make shure it exists
  PMError err = instTargetInit(); // make shure its initialized
  if ( err ) {
    return err;
  }
  if ( instTarget().needsUpdate() ) {
    // getPackages() will reread data:
    packageManager().poolSetInstalled( instTarget().getPackages() );

    // Currently selections need not to be handled here, they are
    // are handled in CommitPackages and the Selection manager keeps
    // data uptodate.
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::instTargetClose
//	METHOD TYPE : PMError
//
PMError Y2PM::instTargetClose()
{
  if ( _instTarget && instTarget().initialized() ) {
    MIL << "Shutdown InstTarget..." << endl;

    // withdraw data from already existing managers
    if ( _packageManager ) {
      std::list<PMPackagePtr> empty;
      _packageManager->poolSetInstalled( empty );
    }
    if ( _selectionManager ) {
      std::list<PMSelectionPtr> empty;
      _selectionManager->poolSetInstalled( empty );
    }

    // close target
    delete _instTarget;
    _instTarget = 0;

    MIL << "InstTarget down" << endl;
  }

  return PMError::E_ok;
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
    MIL << "Launch InstSrcManager..." << endl;
    _instSrcManager = new InstSrcManager;
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
    if ( _instTarget && instTarget().initialized() ) {
      packageManager().poolSetInstalled( instTarget().getPackages() );
    }
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
    if ( _instTarget && instTarget().initialized() ) {
      selectionManager().poolSetInstalled( instTarget().getSelections() );
    }
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

    atexit( &cleanupAtExit );

    MIL << "Created YouPatchManager @" << _youPatchManager << endl;
  }
  return *_youPatchManager;
}


/******************************************************************
**
**
**	FUNCTION NAME : commitProvide
**	FUNCTION TYPE : PMError
**
** Handle package provision for src/bin(srcpkg_r) packages. Callback request
** to RETRY on error is handled here. Request to SKIP/CANCEL is passed back as
** InstSrcError::E_skip_media/InstSrcError::E_cancel_media.
**
** On success the local path to the provided package is returned via retpath_r.
*/
static PMError commitProvide( PMPackagePtr pkg_r, bool srcpkg_r, Pathname & retpath_r )
{
  retpath_r = "";

  CommitProvideReport::Send report( commitProvideReport );
  report->start( pkg_r, srcpkg_r );

  PMError err;
  Pathname localpath;
  unsigned attempt = 0;
  CBSuggest proceed;

  do {
    proceed = report->attempt( ++attempt );
    switch ( proceed ) {
    case CBSuggest::SKIP:
      err = InstSrcError::E_skip_media;   // skip current media
      break;
    case CBSuggest::CANCEL:
      err = InstSrcError::E_cancel_media; // cancel all
      break;
    case CBSuggest::PROCEED:              // proceed
    case CBSuggest::RETRY:                // ignored
      err = PMError::E_ok;
      break;
    }
    if ( err ) {
      break; // canceled
    }

    if ( srcpkg_r ) {
      err = pkg_r->provideSrcPkgToInstall( localpath );
    } else {
      err = pkg_r->providePkgToInstall( localpath );
    }

    proceed = report->result( err, localpath );
    switch ( proceed ) {
    case CBSuggest::PROCEED:              // return original error
      retpath_r = localpath;
      break;
    case CBSuggest::SKIP:
      err = InstSrcError::E_skip_media;   // skip current media
      break;
    case CBSuggest::CANCEL:
      err = InstSrcError::E_cancel_media; // cancel all
      break;
    case CBSuggest::RETRY:                // retry !
      break;
    }
  } while( err && proceed == CBSuggest::RETRY );

  report->stop( err, retpath_r );
  return err;
}

inline PMError commitProvideSrc( PMPackagePtr pkg_r, Pathname & retpath_r ) {
  return commitProvide( pkg_r, /*srcpkg_r*/true, retpath_r );
}

inline PMError commitProvidePkg( PMPackagePtr pkg_r, Pathname & retpath_r ) {
  return commitProvide( pkg_r, /*srcpkg_r*/false, retpath_r );
}

/******************************************************************
**
**
**	FUNCTION NAME : commitSrcSucceeded
**	FUNCTION TYPE : void
**
** Adjust an associated selectables state after successfull
** source package install.
*/
inline void commitSrcSucceeded( const PMPackagePtr & pkg_r )
{
  if ( pkg_r && pkg_r->hasSelectable() ) {
    pkg_r->getSelectable()->set_source_install( false );
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : commitPkgSucceeded
**	FUNCTION TYPE : void
**
** Adjust an associated selectables state after successfull
** package install/delete.
*/
inline void commitPkgSucceeded( const PMPackagePtr & pkg_r )
{
  if ( pkg_r && pkg_r->hasSelectable() ) {
    pkg_r->getSelectable()->user_unset();
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : commitInstall
**	FUNCTION TYPE : PMError
**
** Install the src/bin(srcpkg_r) package available at path_r. Callback request
** to RETRY on error is handled here. Request to SKIP/CANCEL is passed back as
** InstSrcError::E_skip_media/InstSrcError::E_cancel_media.
*/
static PMError commitInstall( PMPackagePtr pkg_r, bool srcpkg_r, const Pathname & path_r )
{
  CommitInstallReport::Send report( commitInstallReport );
  report->start( pkg_r, srcpkg_r, path_r );

  PMError err;
  unsigned attempt = 0;
  CBSuggest proceed;

  do {
    proceed = report->attempt( ++attempt );
    switch ( proceed ) {
    case CBSuggest::SKIP:
      err = InstSrcError::E_skip_media;   // skip current media
      break;
    case CBSuggest::CANCEL:
      err = InstSrcError::E_cancel_media; // cancel all
      break;
    case CBSuggest::PROCEED:              // proceed
    case CBSuggest::RETRY:                // ignored
      err = PMError::E_ok;
      break;
    }
    if ( err ) {
      break; // canceled
    }

    err = Y2PM::instTarget().installPackage( path_r );
    if ( ! err ) {
      if ( srcpkg_r ) {
	commitSrcSucceeded( pkg_r );
      } else {
	commitPkgSucceeded( pkg_r );
      }
    }

    proceed = report->result( err );
    switch ( proceed ) {
    case CBSuggest::PROCEED:              // return original error
      break;
    case CBSuggest::SKIP:
      err = InstSrcError::E_skip_media;   // skip current media
      break;
    case CBSuggest::CANCEL:
      err = InstSrcError::E_cancel_media; // cancel all
      break;
    case CBSuggest::RETRY:                // retry !
      break;
    }
  } while( err && proceed == CBSuggest::RETRY );

  report->stop( err );
  return err;
}

inline PMError commitInstallSrc( PMPackagePtr pkg_r, const Pathname & path_r ) {
  return commitInstall( pkg_r, /*srcpkg_r*/true, path_r );
}

inline PMError commitInstallPkg( PMPackagePtr pkg_r, const Pathname & path_r ) {
  return commitInstall( pkg_r, /*srcpkg_r*/false, path_r );
}

/******************************************************************
**
**
**	FUNCTION NAME : commitRemovePkg
**	FUNCTION TYPE : PMError
**
** Remove the package. Callback request to RETRY on error is handled here.
** Request to SKIP/CANCEL is passed back as InstSrcError::E_skip_media/
** InstSrcError::E_cancel_media.
*/
static PMError commitRemovePkg( PMPackagePtr pkg_r )
{
  CommitRemoveReport::Send report( commitRemoveReport );
  report->start( pkg_r );

  PMError err;
  unsigned attempt = 0;
  CBSuggest proceed;

  do {
    proceed = report->attempt( ++attempt );
    switch ( proceed ) {
    case CBSuggest::SKIP:
      err = InstSrcError::E_skip_media;   // skip current media
      break;
    case CBSuggest::CANCEL:
      err = InstSrcError::E_cancel_media; // cancel all
      break;
    case CBSuggest::PROCEED:              // proceed
    case CBSuggest::RETRY:                // ignored
      err = PMError::E_ok;
      break;
    }
    if ( err ) {
      break; // canceled
    }

    err = Y2PM::instTarget().removePackage( pkg_r );
    if ( ! err ) {
      commitPkgSucceeded( pkg_r );
    }

    proceed = report->result( err );
    switch ( proceed ) {
    case CBSuggest::PROCEED:              // return original error
      break;
    case CBSuggest::SKIP:
      err = InstSrcError::E_skip_media;   // skip current media
      break;
    case CBSuggest::CANCEL:
      err = InstSrcError::E_cancel_media; // cancel all
      break;
    case CBSuggest::RETRY:                // retry !
      break;
    }
  } while( err && proceed == CBSuggest::RETRY );

  report->stop( err );
  return err;
}

/******************************************************************
**
**
**	FUNCTION NAME : installSpmFromMedia
**	FUNCTION TYPE : PMError
**
** Loop through srclist and pick all spms matching source current_src_ptr,
** and media number current_src_media. The media is expected to be available.
**
** On successfull installation the entry is removed fom srclist.
**
** Returns PMError::E_ok if all selected packages were installed, otherwise PMError::E_error.
** Callback requests to SKIP/CANCEL however are passed back as InstSrcError::E_skip_media/
** InstSrcError::E_cancel_media, hiding previous errors.
*/
static PMError installSpmFromMedia( constInstSrcPtr current_src_ptr_r, unsigned current_src_media_r,
				    list<PMPackagePtr> & srclist_r )
{
  PMError ret; // error returned

  // no-op if we don't have a medium yet
  if ( ! ( current_src_ptr_r && current_src_media_r ) )
    return ret;

  // Loop through srclist
  PMError err; // error within loop
  bool eraseit = false; // whether to advance iterator or to erase the current element
  for ( list<PMPackagePtr>::iterator it = srclist_r.begin(); it != srclist_r.end();
	( eraseit ? it = srclist_r.erase( it ) : ++it ) ) {

    // first of all check for cancel request in previous loop:
    switch ( err ) {
    case InstSrcError::E_skip_media:   // skip current media
    case InstSrcError::E_cancel_media: // cancel all
      break;
    default:
      err = PMError::E_ok;
      break;
    }
    if ( err ) {
      break; // canceled
    }
    // go

    PMPackagePtr & cpkg( *it );
    eraseit = false;

    if ( cpkg->source() != current_src_ptr_r ) {
      continue; // unwanted source
    }

    unsigned cpkgMedia = atoi( cpkg->sourceloc().c_str() );

    if ( cpkgMedia != current_src_media_r ) {
      if ( ! cpkgMedia ) {
	// will never get installed
	ERR << "No source location for " << cpkg << endl;
      }
      continue; // unwanted media number
    }

    // let source provide the package
    Pathname path;
    err = commitProvideSrc( cpkg, path );
    if ( err ) {
      ret = PMError::E_error;
      continue; // SKIP/CANCEL evaluated at loop start/end
    }

    // install provided source package
    err = commitInstallSrc( cpkg, path );
    if ( err ) {
      // check cpkg settings in case any error occurred after
      // successfull install.
      if ( cpkg && cpkg->hasSelectable() && ! cpkg->getSelectable()->source_install() ) {
	eraseit = true;
      }
      ret = PMError::E_error;
      continue; // SKIP/CANCEL evaluated at loop start/end
    } else {
      eraseit = true;
    }

  } // loop over source packages


  // check for cancel request in loop:
  switch ( err ) {
  case InstSrcError::E_skip_media:   // skip current media
  case InstSrcError::E_cancel_media: // cancel all
    // hides any previous PMError::E_error
    ret = err;
    break;
  default:
    // report what actually happened
    break;
  }
  return ret;
}

/******************************************************************
**
**
**	FUNCTION NAME : internal_commitPackages
**	FUNCTION TYPE : int
**
**      commit all changes in the package manager
**		==> actually delete/install packages
**		relies on callbacks for media change
**		!! DOES NOT SOLVE !!
**
**	if media_nr is != 0, only packages from this media are
**	installed. media_nr==0 means install all packages from all media.
**
**	returns number of sucessfully installed packages
**	  negated if the installation was aborted
**
**	returns failed packages in 'errors_r'
**	returns uninstalled packages (because media not available) in 'remaining_r'
**	returns uninstalled source packages in 'srcremaining_r'
*/
static int internal_commitPackages( unsigned mediaNr_r,
				    std::list<std::string> & errors_r,
				    std::list<std::string> & remaining_r,
				    std::list<std::string> & srcremaining_r,
				    InstSrcManager::ISrcIdList installrank_r )
{
  CommitReport::Send report( commitReport );

#warning loosing version information when using remaining_r etc.
  errors_r.clear();
  remaining_r.clear();
  srcremaining_r.clear();

  // get packages to process
  std::list<PMPackagePtr> dellist;
  std::list<PMPackagePtr> inslist;
  std::list<PMPackagePtr> srclist;
  Y2PM::packageManager().getPackagesToInsDel( dellist, inslist, srclist, installrank_r );
  if ( mediaNr_r ) {
    MIL << "Restrict to media number " << mediaNr_r << endl;
  }

  PMError error;

  ///////////////////////////////////////////////////////////////////
  // first, remove all packages marked for deletion
  ///////////////////////////////////////////////////////////////////
  for ( list<PMPackagePtr>::iterator it = dellist.begin(); it != dellist.end(); ++it ) {

    PMError res = commitRemovePkg( *it );

    switch ( res ) {
    case InstSrcError::E_cancel_media: // cancel all
    case InstSrcError::E_skip_media:   // skip remaining
      error = res;
      break;
    default:                           // continue
      break;
    }
    if ( error ) {
      break; // canceled
    }
  }

  switch ( error ) {
  case InstSrcError::E_cancel_media: // cancel all
    return 0;
    break;
  default:
    error = PMError::E_ok;
    break;
  }

  ///////////////////////////////////////////////////////////////////
  // One may argue whether selection data should be installed before
  // or after any packages. Doing it before has the benefit, that the
  // selection DB reflects what the user wanted. In case of trouble it
  // should be easier to check and manualy repair.
  //
  // But we could thing about moving it to instTargetUpdate().
  //
  ///////////////////////////////////////////////////////////////////
  {
    PMError res = Y2PM::selectionManager().installOnTarget();
    if ( res ) {
      ERR << "Error installing selection data: " << res << endl;
    }
  }

  ///////////////////////////////////////////////////////////////////
  // install loop
  ///////////////////////////////////////////////////////////////////

  int             count = 0;
  constInstSrcPtr current_src_ptr = 0;   // current source
  unsigned        current_src_media = 0; // current media number

  for ( list<PMPackagePtr>::iterator it = inslist.begin(); it != inslist.end(); ++it ) {

    PMPackagePtr & cpkg( *it );

    // Check whether package fits a requested mediaNr_r
    unsigned cpkgMedianr = cpkg->medianr();

    if ( mediaNr_r && cpkgMedianr != mediaNr_r ) {
      remaining_r.push_back ( cpkg->name() ); // package unprocessed
      continue;
    }

    ///////////////////////////////////////////////////////////////////
    // !!! Always update statistics for the current package, but make
    // shure any error reaches the end of the loop, where SKIP/CANCEL
    // requests are processed.
    ///////////////////////////////////////////////////////////////////
    PMError res;

    ///////////////////////////////////////////////////////////////////
    // Check whether we need to change the media
    ///////////////////////////////////////////////////////////////////
    constInstSrcPtr cpkgSource = cpkg->source();

    if ( ! ( cpkgSource == current_src_ptr && cpkgMedianr == current_src_media ) ) {

      ///////////////////////////////////////////////////////////////////
      // Install any source packages before we're going to change media
      ///////////////////////////////////////////////////////////////////
      res = installSpmFromMedia( current_src_ptr, current_src_media, srclist );
      if ( res ) {
#warning Unevaluated SKIP/CANCEL from installSpmFromMedia
	res = PMError::E_ok;
      }

      ///////////////////////////////////////////////////////////////////
      // Change the media
      ///////////////////////////////////////////////////////////////////
      if ( ! res ) {
	// If source changes, physically release any old media attached
	if ( current_src_ptr && cpkgSource != current_src_ptr ) {
	  current_src_ptr->releaseMedia( /*if_removable_r*/true );
	}

	// Change the media (physically changed when accessed access)
	current_src_ptr = cpkgSource;
	current_src_media = cpkgMedianr;
	report->advanceToMedia( current_src_ptr, current_src_media );
      }

    }
    if ( res ) {
      remaining_r.push_back ( cpkg->name() ); // package unprocessed
    }

    ///////////////////////////////////////////////////////////////////
    // Provide the binary package to install
    ///////////////////////////////////////////////////////////////////
    Pathname cpkgPath;

    if ( ! res ) {
      res = commitProvidePkg( cpkg, cpkgPath );
      if ( res ) {
	remaining_r.push_back ( cpkg->name() ); // package unprocessed
      }
    }

    ///////////////////////////////////////////////////////////////////
    // Install the binary package
    ///////////////////////////////////////////////////////////////////
    if ( ! res ) {
      res = commitInstallPkg( cpkg, cpkgPath );
      if ( res ) {
	// check cpkg settings in case any error occurred after
	// successfull install.
	if ( cpkg && cpkg->hasSelectable() && ! cpkg->getSelectable()->to_install() ) {
	  ++count;
	} else {
#warning Classification may be incorrect. Must distinguish between action error and callback request
	  // InstSrcError:: hides install failed/succeeded.
	  switch ( res ) {
	  case InstSrcError::E_cancel_media: // cancel all
	  case InstSrcError::E_skip_media:   // skip current media
	    remaining_r.push_back ( cpkg->name() ); // package unprocessed
	    break;
	  default:
	    errors_r.push_back( cpkg->name() );
	    break;
	  }
	}
      } else {
	++count;
      }
    }

    ///////////////////////////////////////////////////////////////////
    // Evaluate res
    ///////////////////////////////////////////////////////////////////
    switch ( res ) {
    case InstSrcError::E_cancel_media: // cancel all
      for ( ; it != inslist.end(); ++it ) {
	remaining_r.push_back( (*it)->name() ); // package unprocessed
      }
      error = res;
      break;
    case InstSrcError::E_skip_media:   // skip current media
      for ( ; it != inslist.end(); ++it ) {
	if ( (*it)->source() == current_src_ptr && (*it)->medianr() == current_src_media ) {
	  remaining_r.push_back( (*it)->name() ); // package unprocessed
	} else {
	  --it; // not to miss the fist package of next medis
	  break;
	}
      }
      break;
    default:                           // continue
      break;
    }
    if ( error ) {
      break; // canceled
    }

  } // install loop

  switch ( error ) {
  case InstSrcError::E_cancel_media: // cancel all
    // delay until commit end
    break;
  default:
    error = PMError::E_ok;
    break;
  }

  ///////////////////////////////////////////////////////////////////
  // Now loop over srclist and install remaining sources.
  // Start with the currently attached media, if any and loop through
  // all allowed media numbers (limited by media_nr), this effectively
  // sorts the list of source rpms to install by media number
  ///////////////////////////////////////////////////////////////////
  if ( ! error && srclist.size() ) {
#warning Actually we want to install all remaining sourcepkgs not on media we already skipped. But we miss some!
    // we retry previously skipped media if it matches current_src_media
    // we will omitt any medianr below current_src_media if it didn't contain a binpkg

    unsigned int next_src_media = current_src_media; // number of currently attached media, if any
    bool go_on = true;

    while (go_on)
    {
	if (srclist.size() == 0)			// we're done
	    break;

	// find first package in source list which matches next medium
	unsigned pkgmedianr = 0;
	std::list<PMPackagePtr>::iterator it = srclist.begin();
	for (; it != srclist.end(); ++it)
	{
	    string srcloc = (*it)->sourceloc();
	    if (srcloc.empty())
	    {
		continue;
	    }
	    pkgmedianr = atoi (srcloc.c_str());
	    if ( !pkgmedianr ) {
	      continue;	// can not install this.
	    }
	    if (  ((next_src_media > 0)				// if we already have an attached/wanted media number
		    && (pkgmedianr != next_src_media))	// and the current package is not on this media
	        ||((mediaNr_r > 0)				// or we only want a specific media number
		    && (pkgmedianr != mediaNr_r)))		// and the current package is not on this media
	    {
		continue;					// keep on searching
	    }
	    break;
	}

	if (it == srclist.end())				// no matching package found
	{
	    break;
	}

	// ok, we have a matching package

	if (((*it)->source() != current_src_ptr)		// source or media change ?
	    || (pkgmedianr != current_src_media))
	{
	    if (((*it)->source() != current_src_ptr)	// source change -> release old source media
		&& (current_src_ptr != 0))		// if we have an old media attached
	    {
		current_src_ptr->releaseMedia (true);	// release if removable (CD/DVD)
	    }

	    current_src_ptr = (*it)->source();
	    current_src_media = pkgmedianr;
	    report->advanceToMedia( current_src_ptr, current_src_media );
	}

	PMError res = installSpmFromMedia (current_src_ptr, current_src_media, srclist);	// install sources from it while we have it attached
	if ( res == InstSrcError::E_cancel_media ) {
	  error = res;
	  go_on = false;
	}
	if (!go_on)
	    break;

	if (mediaNr_r > 0)				// if a specific media number is requested
	    break;

	next_src_media++;				// go on with loop and next medium
    }
  }

  ///////////////////////////////////////////////////////////////////
  // cleanup
  ///////////////////////////////////////////////////////////////////

  // media release is handled in Y2PM::commitPackages

  // copy remaining sources to srcremaining_r
  for ( std::list<PMPackagePtr>::iterator it = srclist.begin(); it != srclist.end(); ++it ) {
    srcremaining_r.push_back ((*it)->name());
  }

  return (error ? -count : count);
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
int Y2PM::commitPackages( unsigned mediaNr_r,
			  std::list<std::string> & errors_r,
			  std::list<std::string> & remaining_r,
			  std::list<std::string> & srcremaining_r,
			  InstSrcManager::ISrcIdList installrank_r )
{
  if ( ! ( _instTarget && instTarget().initialized() ) ) {
    ERR << "Can't commit packages without instTarget being initialized!" << endl;
    return 0;
  }

  if ( mediaNr_r == 9999 ) {
#warning Using faked medianr 9999 to trigger instTargetUpdate
    instTargetUpdate();
    return 0;
  }

  MIL << "Commiting packages..." << endl;
  int ret = internal_commitPackages( mediaNr_r, errors_r, remaining_r, srcremaining_r, installrank_r );

  // Release all source media

  MIL << "Commiting packages..." << endl;
  instSrcManager().releaseAllMedia();

  if ( ! mediaNr_r ) {
    instTargetUpdate(); // reread modified databases
  }

  MIL << "Commiting packages returns " << ret << endl;
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::installFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : install a single, locally availabe rpm file, uses callbacks !
PMError Y2PM::installFile( const Pathname & path_r )
{
#warning Check for initialized target?
  return instTarget().installPackage( path_r, RpmDb::RPMINST_NONE );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::removePackage
//	METHOD TYPE : PMError
//
//	DESCRIPTION : remove a single package by name, uses callbacks !
PMError Y2PM::removePackage( const std::string & pkgname_r )
{
#warning Check for initialized target?
  return instTarget().removePackage( pkgname_r );
}
