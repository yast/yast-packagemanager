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
#include "PMRcValues.h"
#include <y2pm/Y2PMCallbacks.h>

#include <y2pm/InstTarget.h>
#include <y2pm/InstTargetError.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcManager.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectionManager.h>
#include <y2pm/PMLanguageManager.h>
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/InstYou.h>
#include <y2pm/PMLocale.h>

using namespace std;
using namespace Y2PMCallbacks;

///////////////////////////////////////////////////////////////////
#undef Y2LOG
#define Y2LOG "Y2PM"
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// global settings
///////////////////////////////////////////////////////////////////
#warning MUST INIT GLOBAL SETTINGS

Pathname Y2PM::_system_rootdir( "/" );
bool Y2PM::_cache_to_ramdisk( true );
PkgArch Y2PM::_base_arch;
std::list<PkgArch> Y2PM::_allowed_archs;

///////////////////////////////////////////////////////////////////
// components provided
///////////////////////////////////////////////////////////////////

InstTarget * Y2PM::_instTarget = 0;

InstSrcManager * Y2PM::_instSrcManager = 0;

PMPackageManager *   Y2PM::_packageManager = 0;

PMSelectionManager * Y2PM::_selectionManager = 0;

PMLanguageManager *   Y2PM::_languageManager = 0;

PMYouPatchManager *  Y2PM::_youPatchManager = 0;

bool Y2PM::_languageManagerDirty = true;

void Y2PM::cleanupAtExit()
{
  if ( _youPatchManager ) {
    delete _youPatchManager;
    _youPatchManager = 0;
  }
}

///////////////////////////////////////////////////////////////////
//
// Locale settings
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::preferredLocaleChanged
//	METHOD TYPE : PMError
//
PMError Y2PM::preferredLocaleChanged()
{
  MIL << "New preferred locale: '" << getPreferredLocale() << "'" << endl;

  if ( hasInstSrcManager() ) {
    instSrcManager().preferredLocaleChanged();
  }

  return PMError::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::requestedLocalesChanged
//	METHOD TYPE : PMError
//
PMError Y2PM::requestedLocalesChanged( const LocaleSet & addLocales_r, const LocaleSet & delLocales_r )
{
  MIL << "New requested locales: {";
  for ( LocaleSet::const_iterator it = getRequestedLocales().begin(); it != getRequestedLocales().end(); ++it ) {
    MIL << " " << *it;
  }
  MIL << " }" << endl;
  PM::rcValues().requestedLocales = getRequestedLocales();
  selectionManager().requestedLocalesChanged( addLocales_r, delLocales_r );
  return PMError::E_ok;
}

///////////////////////////////////////////////////////////////////
//
// package/selection candidate selection
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::setCandidateOrder
//	METHOD TYPE : PMError
//
PMError Y2PM::setCandidateOrder( PM::CandidateOrder neworder_r )
{
  MIL << "setCandidateOrder " << neworder_r << endl;
  if ( PMSelectable::setCandidateOrder( neworder_r ) ) {
    // order actually changed -> sort candidate lists
    PM::rcValues().candidateOrder = neworder_r;
    if ( _packageManager ) {
      packageManager().poolSortCandidates();
    }
    if ( _selectionManager ) {
      selectionManager().poolSortCandidates();
    }
    languageManagerTagDirty();
  }
  return PMError::E_ok;
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
    _allowed_archs.clear();
    if ( _base_arch == "i386" ) {
      _allowed_archs.push_back( PkgArch( "i386" ) );
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    } else if ( _base_arch == "i486" ) {
      _allowed_archs.push_back( PkgArch( "i486" ) );
      _allowed_archs.push_back( PkgArch( "i386" ) );
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    } else if ( _base_arch == "i586" ) {
      _allowed_archs.push_back( PkgArch( "i586" ) );
      _allowed_archs.push_back( PkgArch( "i486" ) );
      _allowed_archs.push_back( PkgArch( "i386" ) );
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    } else if ( _base_arch == "i686" ) {
      _allowed_archs.push_back( PkgArch( "i686" ) );
      _allowed_archs.push_back( PkgArch( "i586" ) );
      _allowed_archs.push_back( PkgArch( "i486" ) );
      _allowed_archs.push_back( PkgArch( "i386" ) );
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    } else if ( _base_arch == "ia64" ) {
      _allowed_archs.push_back( PkgArch( "ia64" ) );
      _allowed_archs.push_back( PkgArch( "i686" ) );
      _allowed_archs.push_back( PkgArch( "i586" ) );
      _allowed_archs.push_back( PkgArch( "i486" ) );
      _allowed_archs.push_back( PkgArch( "i386" ) );
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    } else if ( _base_arch == "athlon" ) {
      _allowed_archs.push_back( PkgArch( "athlon" ) );
      _allowed_archs.push_back( PkgArch( "i686" ) );
      _allowed_archs.push_back( PkgArch( "i586" ) );
      _allowed_archs.push_back( PkgArch( "i486" ) );
      _allowed_archs.push_back( PkgArch( "i386" ) );
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    } else if ( _base_arch == "x86_64" ) {
      _allowed_archs.push_back( PkgArch( "x86_64" ) );
      _allowed_archs.push_back( PkgArch( "athlon" ) );
      _allowed_archs.push_back( PkgArch( "i686" ) );
      _allowed_archs.push_back( PkgArch( "i586" ) );
      _allowed_archs.push_back( PkgArch( "i486" ) );
      _allowed_archs.push_back( PkgArch( "i386" ) );
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    } else if ( _base_arch == "s390" ) {
      _allowed_archs.push_back( PkgArch( "s390" ) );
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    } else if ( _base_arch == "s390x" ) {
      _allowed_archs.push_back( PkgArch( "s390x" ) );
      _allowed_archs.push_back( PkgArch( "s390" ) );
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    } else if ( _base_arch == "ppc" ) {
      _allowed_archs.push_back( PkgArch( "ppc" ) );
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    } else if ( _base_arch == "ppc64" ) {
      _allowed_archs.push_back( PkgArch( "ppc64" ) );
      _allowed_archs.push_back( PkgArch( "ppc" ) );
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    } else if ( _base_arch == "noarch" ) {
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    } else {
      _allowed_archs.push_back( PkgArch( "_base_arch" ) );
      _allowed_archs.push_back( PkgArch( "noarch" ) );
    }
  }
  return _base_arch;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::allowedArchs
//	METHOD TYPE : const PM::ArchSet &
//
const PM::ArchSet & Y2PM::allowedArchs( void )
{
  if ( _base_arch->empty() ) {
    baseArch(); // initializes _allowed_archs
  }
  return _allowed_archs;
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
    MIL << "Created InstTarget (not yet initialized)" << endl;
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
      rcInit();
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
    packageManager().setMountPoints( instTarget().getMountPoints() );
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
    rcSave();
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
//	METHOD NAME : Y2PM::ISM_RanksCanged
//	METHOD TYPE : void
//
void Y2PM::ISM_RanksCanged()
{
#warning Further actions on new Src ranks, except sorting candidate lists?
  // sort candidate lists
  packageManager().poolSortCandidates();
  selectionManager().poolSortCandidates();
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
//	METHOD NAME : Y2PM::languageManager
//	METHOD TYPE : PMLanguageManager &
//
//	DESCRIPTION :
//
PMLanguageManager & Y2PM::languageManager()
{
  if ( !_languageManager )
  {
    MIL << "Launch LanguageManager..." << endl;
    _languageManager = new PMLanguageManager;
    MIL << "Created LanguageManager @" << _languageManager << endl;
  }
  if ( _languageManagerDirty ) {
    _languageManagerDirty = false;
    languageManager().rescan();
  }
  return *_languageManager;
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

    // The text domain should rather be set at a more central place at startup
    // of the packagemanager.
#warning FIXME: Move setTextdomain call to a more central place
    PMLocale::setTextdomain();

    _youPatchManager = new PMYouPatchManager;

    _youPatchManager->instYou().initProduct();

    vector<PMYouPatchPtr> patches = Y2PM::instTarget().getPatches();

    _youPatchManager->poolSetInstalled( patches );

    vector<PMYouPatchPtr>::const_iterator itPatch;
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
**	FUNCTION NAME : extractSpmFromMedia
**	FUNCTION TYPE : static list<PMPackagePtr>
**
** Take all spms matching source current_src_ptr, and media number current_src_media
** out of srclist and return them in a new list.
*/
static list<PMPackagePtr> extractSpmFromMedia( constInstSrcPtr current_src_ptr_r,
					       unsigned current_src_media_r,
					       list<PMPackagePtr> & srclist_r )
{
  list<PMPackagePtr> ret;

  // no-op if we don't have a medium yet
  if ( ! ( current_src_ptr_r && current_src_media_r ) || srclist_r.empty() )
    return ret;

  bool eraseit = false; // whether to advance iterator or to erase the current element
  for ( list<PMPackagePtr>::iterator it = srclist_r.begin(); it != srclist_r.end();
	( eraseit ? it = srclist_r.erase( it ) : ++it ) ) {

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

    // wanted:
    ret.push_back( cpkg );
    eraseit = true;
  }

  return ret;
}

/******************************************************************
**
**
**	FUNCTION NAME : copySpm
**	FUNCTION TYPE : void
**
** Unfortunately srcremaining_r strores only names, not PMPackagePtr.
*/
static void copySpm( const list<PMPackagePtr> & todolist_r, std::list<std::string> & srcremaining_r )
{
  for ( std::list<PMPackagePtr>::const_iterator it = todolist_r.begin(); it != todolist_r.end(); ++it ) {
    srcremaining_r.push_back( (*it)->name() );
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : rememberSpmFromMedia
**	FUNCTION TYPE : void
**
** Take all spms matching source current_src_ptr, and media number current_src_media
** out of srclist and store them in srcremaining_r.
*/
static void rememberSpmFromMedia( constInstSrcPtr current_src_ptr_r,
				  unsigned current_src_media_r,
				  list<PMPackagePtr> & srclist_r,
				  std::list<std::string> & srcremaining_r )
{
  list<PMPackagePtr> todolist( extractSpmFromMedia( current_src_ptr_r, current_src_media_r, srclist_r ) );
  copySpm( todolist, srcremaining_r );
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
				    list<PMPackagePtr> & srclist_r,
				    std::list<std::string> & srcremaining_r )
{
  PMError ret; // error returned

  // extract todolist from srclist_r
  list<PMPackagePtr> todolist( extractSpmFromMedia( current_src_ptr_r, current_src_media_r, srclist_r ) );
  if ( todolist.empty() )
    return ret;

  // Loop through todolist and remove packages after successfull install.
  PMError err; // error within loop
  bool eraseit = false; // whether to advance iterator or to erase the current element
  for ( list<PMPackagePtr>::iterator it = todolist.begin(); it != todolist.end();
	( eraseit ? it = todolist.erase( it ) : ++it ) ) {

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
    // report whatever actually happened
    break;
  }

  // copy remaining sources to srcremaining_r
  copySpm( todolist, srcremaining_r );
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
#warning Must distinguish between action error and callback request
#define COMMIT_ERROR  -99999
#define COMMIT_ABORT -100000

static int internal_commitPackages( unsigned mediaNr_r,
				    std::list<std::string> & errors_r,
				    std::list<std::string> & remaining_r,
				    std::list<std::string> & srcremaining_r )
{
  CommitReport::Send report( commitReport );

  ///////////////////////////////////////////////////////////////////
  // One may argue whether selection data should be installed before
  // or after any packages. Doing it before has the benefit, that the
  // selection DB reflects what the user wanted. In case of trouble it
  // should be easier to check and manualy repair.
  //
  // But we could think about moving it to instTargetUpdate().
  //
  ///////////////////////////////////////////////////////////////////
  {
    Y2PM::rcSave(); // saves e.g. LanguageManager settings
    PMError res = Y2PM::selectionManager().installOnTarget();
    if ( res ) {
      ERR << "Error installing selection data: " << res << endl;
    }
  }
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Before taking care of packages make shure the package signing
  // keys are up to date.
  ///////////////////////////////////////////////////////////////////
  {
    PkgPubkeyCache & keyCache( Y2PM::instSrcManager().pkgPubkeyCache() );
    PMError res = keyCache.sync();
    if ( res ) {
      ERR << "Error importing package signing keys: " << res << endl;
    }
  }
  ///////////////////////////////////////////////////////////////////

#warning loosing version information when using remaining_r etc.
  errors_r.clear();
  remaining_r.clear();
  srcremaining_r.clear();

  // get packages to process
  std::list<PMPackagePtr> dellist;
  std::list<PMPackagePtr> inslist;
  std::list<PMPackagePtr> srclist;
  Y2PM::packageManager().getPackagesToInsDel( dellist, inslist, srclist );
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
    return COMMIT_ABORT;
    break;
  default:
    error = PMError::E_ok;
    break;
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
      res = installSpmFromMedia( current_src_ptr, current_src_media, srclist, srcremaining_r );
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
	MIL << "Process media " << current_src_media << " of " << current_src_ptr << endl;
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
      WAR << "Received " << res << endl;
      ++it; // current package already pushed to remaining or error list
      for ( ; it != inslist.end(); ++it ) {
	remaining_r.push_back( (*it)->name() ); // package unprocessed
      }
      --it; // not to miss loop end
      // Take care about sourcepackages.
      copySpm( srclist, srcremaining_r );
      srclist.clear();
      error = res;
      break;
    case InstSrcError::E_skip_media:   // skip current media
      WAR << "Received " << res << ": skip media " << current_src_media << " of " << current_src_ptr << endl;
      ++it; // current package already pushed to remaining or error list
      for ( ; it != inslist.end(); ++it ) {
	if ( (*it)->source() == current_src_ptr && (*it)->medianr() == current_src_media ) {
	  remaining_r.push_back( (*it)->name() ); // package unprocessed
	} else {
	  break; // next media or end
	}
      }
      --it; // not to miss the fist package of next media or loop end
      // Take care about sourcepackages.
      rememberSpmFromMedia( current_src_ptr, current_src_media, srclist, srcremaining_r );
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
  // sorts the list of source rpms to install by media number.
  //
  // NOTE: Aborting the installation here will NOT be passed back to the
  //       caller. The binary packages is what's interesting.
  ///////////////////////////////////////////////////////////////////
  if ( ! error && srclist.size() ) {
    PMError res;

    ///////////////////////////////////////////////////////////////////
    // If a media is avialable, install any source packages before we're
    // going to change it.
    ///////////////////////////////////////////////////////////////////
    if ( current_src_ptr && current_src_media ) {
      res = installSpmFromMedia( current_src_ptr, current_src_media, srclist, srcremaining_r );
      switch ( res ) {
      case InstSrcError::E_cancel_media: // cancel all
	// keep error
	break;
      case InstSrcError::E_skip_media:   // skiped current media
      default:
	res = PMError::E_ok; // hide error
	break;
      }
    }

    ///////////////////////////////////////////////////////////////////
    // now process what's left
    ///////////////////////////////////////////////////////////////////
    while ( ! res && srclist.size() ) {

      PMPackagePtr & cpkg( *srclist.begin() );

      unsigned cpkgMedianr = atoi( cpkg->sourceloc().c_str() );
      constInstSrcPtr cpkgSource = cpkg->source();

      // Check whether package fits a requested mediaNr_r
      if ( mediaNr_r && cpkgMedianr != mediaNr_r ) {
	// unwanted source -> remember packages.
	rememberSpmFromMedia( cpkgSource, cpkgMedianr, srclist, srcremaining_r );
	continue;
      }

      ///////////////////////////////////////////////////////////////////
      // Change the media
      ///////////////////////////////////////////////////////////////////
      // If source changes, physically release any old media attached
      if ( current_src_ptr && cpkgSource != current_src_ptr ) {
	current_src_ptr->releaseMedia( /*if_removable_r*/true );
      }

      // Change the media (physically changed when accessed access)
      current_src_ptr = cpkgSource;
      current_src_media = cpkgMedianr;
      report->advanceToMedia( current_src_ptr, current_src_media );
      MIL << "Process media " << current_src_media << " of " << current_src_ptr << endl;

      // install them
      res = installSpmFromMedia( current_src_ptr, current_src_media, srclist, srcremaining_r );
      switch ( res ) {
      case InstSrcError::E_cancel_media: // cancel all
	// keep error
	break;
      case InstSrcError::E_skip_media:   // skiped current media
      default:
	res = PMError::E_ok; // hide error
	break;
      }
    }
  }

  ///////////////////////////////////////////////////////////////////
  // cleanup
  ///////////////////////////////////////////////////////////////////

  // media release is handled in Y2PM::commitPackages

  // copy remaining sources to srcremaining_r
  copySpm( srclist, srcremaining_r );

  return (error ? (COMMIT_ABORT - count) : count);
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
#warning DEPRECATED commitPackages using sourcerank
  INT << "Call to obsolete commitPackages using sourcerank argument" << endl;
  return commitPackages( mediaNr_r, errors_r, remaining_r, srcremaining_r );
}

int Y2PM::commitPackages( unsigned mediaNr_r,
			  std::list<std::string> & errors_r,
			  std::list<std::string> & remaining_r,
			  std::list<std::string> & srcremaining_r )
{
  if ( ! ( _instTarget && instTarget().initialized() ) ) {
    ERR << "Can't commit packages: " << InstTargetError::E_not_initialized << endl;
    return -99999;
  }

  if ( mediaNr_r == 9999 ) {
#warning Using faked medianr 9999 to trigger instTargetUpdate
    instTargetUpdate();
    return 0;
  }

  MIL << "Commiting packages..." << endl;
  int ret = internal_commitPackages( mediaNr_r, errors_r, remaining_r, srcremaining_r );

  // Release all source media
  if ( runningFromSystem() || cacheToRamdisk() ) {
    instSrcManager().releaseAllMedia();
  } // else: we are in instsys and the InstSrc runs directly from CD

  if ( ! mediaNr_r ) {
    instTargetUpdate(); // reread modified databases
  }

  if ( ret < 0 ) {
    WAR << "commitPackages aborted after " << (-ret + COMMIT_ABORT) << " package(s) installed" << endl;
  } else {
    MIL << "commitPackages installed " << ret << " package(s)" << endl;
  }
    MIL << "commitPackages errors(" << errors_r.size() << "), remaining(" << remaining_r.size()
      << "), srcremaining(" << srcremaining_r.size() << ")" << endl;
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
  if ( ! ( _instTarget && instTarget().initialized() ) ) {
    ERR << "Can't install package '" << path_r << "': " << InstTargetError::E_not_initialized << endl;
    return InstTargetError::E_not_initialized;
  }
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
  if ( ! ( _instTarget && instTarget().initialized() ) ) {
    ERR << "Can't remove package '" << pkgname_r << "': " << InstTargetError::E_not_initialized <<endl;
    return InstTargetError::E_not_initialized;
  }
  return instTarget().removePackage( pkgname_r );
}
