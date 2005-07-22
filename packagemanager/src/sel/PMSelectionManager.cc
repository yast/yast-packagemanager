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

  File:       PMSelectionManager.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/LangCode.h>

#include <Y2PM.h>
#include <y2pm/PMSelectionManager.h>
#include <y2pm/PMSelectionManagerCallbacks.h>
#include <y2pm/InstTarget.h>
#include <y2pm/InstTargetError.h>

using namespace std;
using namespace PMSelectionManagerCallbacks;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::PMSelectionManager
//	METHOD TYPE : Constructor
//
PMSelectionManager::PMSelectionManager()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::~PMSelectionManager
//	METHOD TYPE : Destructor
//
PMSelectionManager::~PMSelectionManager()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::assertObjectType
//	METHOD TYPE : PMObjectPtr
//
PMObjectPtr PMSelectionManager::assertObjectType( const PMObjectPtr & object_r ) const
{
  PMSelectionPtr p( object_r );
  if ( object_r && !p ) {
    ERR << "Object is not a Selection: " << object_r << endl;
  }
  return p;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::postPSI
//	METHOD TYPE : void
//
void PMSelectionManager::postPSI()
{
  Y2PM::languageManagerTagDirty();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::postPAC
//	METHOD TYPE : void
//
void PMSelectionManager::postPAC()
{
  Y2PM::languageManagerTagDirty();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::postPRC
//	METHOD TYPE : void
//
void PMSelectionManager::postPRC()
{
  Y2PM::languageManagerTagDirty();
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const PMSelectionManager & obj )
{
    str << "PMSelectionManager" << endl;
    return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::setLast
//	METHOD TYPE : void
//
inline void PMSelectionManager::setLast( const PMSelectablePtr & sel_r, PMSelectable::Fate val_r )
{
  _last_active[sel_r] = val_r;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::lastState
//	METHOD TYPE : PMSelectable::Fate
//
inline PMSelectable::Fate PMSelectionManager::lastState( const PMSelectablePtr & sel_r ) const
{
  ActiveMap::const_iterator it( _last_active.find( sel_r ) );
  return( it == _last_active.end() ? PMSelectable::UNMODIFIED : it->second);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::getState
//	METHOD TYPE : PMSelectable::Fate
//
inline PMSelectable::Fate PMSelectionManager::getState( const PMSelectablePtr & sel_r ) const
{
  return sel_r->fate();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::activate
//	METHOD TYPE : PMError
//
PMError PMSelectionManager::activate()
{
  return activate( Y2PM::packageManager() );
}

/******************************************************************
**
**
**	FUNCTION NAME : setAdd
**	FUNCTION TYPE : inline void
*/
inline void setAdd( PMManager::PMSelectableVec & lhs, const PMManager::PMSelectableVec & rhs )
{
  lhs.insert( rhs.begin(), rhs.end() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::syncToPkgMgr
//	METHOD TYPE : PMError
//
PMError PMSelectionManager::syncToPkgMgr( PkgFates & fates_r )
{
  // First of all unset everyting collected in pkgUnmodified. Might be
  // some of them are set again below, but we don't mind.
  for ( PMSelectableVec::const_iterator it = fates_r.pkgUnmodified.begin(); it != fates_r.pkgUnmodified.end(); ++it ) {
    (*it)->appl_unset();
  }
  // Now: all remaining modification requests are user requests!
  fates_r.setNothingProcessed();


  // always delete delpacks
  for ( PMSelectableVec::const_iterator it = fates_r.pkgToDelete.begin(); it != fates_r.pkgToDelete.end(); ++it ) {
    (*it)->appl_set_offSystem(); // hard delete
  }
  fates_r.processedToDelete();


  // install inspacks
  for ( PMSelectableVec::const_iterator it = fates_r.pkgToInstall.begin(); it != fates_r.pkgToInstall.end(); ++it ) {
    if ( fates_r.didProcess( *it ) ) {
      continue; // don't revert previous setting
    }
    (*it)->appl_set_install();
  }
  fates_r.processedToInstall();


  // protect unmodified but installed inspacks from being deleted below,
  // by setting them prosessed.
  fates_r.processedOnSystem();


  // remove deleted selections inspacks
  for ( PMSelectableVec::const_iterator it = fates_r.pkgOffSystem.begin(); it != fates_r.pkgOffSystem.end(); ++it ) {
    if ( fates_r.didProcess( *it ) ) {
      continue; // don't revert previous setting
    }
    (*it)->appl_set_delete();
  }

  return PMError::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::activate
//	METHOD TYPE : PMError
//
PMError PMSelectionManager::activate( PMPackageManager & package_mgr )
{
  PkgFates fates;
  // selections to install:
  //     ToDelete   - hard: get rid of it
  //     ToInstall  - hard: (re)install it
  // unmodified but installed selections: protect inspacks from deletion
  //     OnSystem   - soft: Keep it OnSystem.
  // selections to delete:
  //     OffSystem  - soft: if no one minds, bring it OffSystem
  // EVERYTHING ELSE:
  //     Unmodified - is set to unmodified

  for ( PMSelectableVec::const_iterator it = begin(); it != end(); ++it ) {
    const PMSelectablePtr & sptr( *it );

    PMSelectable::Fate lstate = lastState( sptr );
    PMSelectable::Fate nstate = getState( sptr );

    if ( lstate != nstate ) {
      // tag previously selected packages to unmodify.
      // !!! Here we must check, if installed/candidate objects needed do exist.
      //     E.g even if lstate was TO_DELETE, it might be the installedObj isn't present anymore.
      switch ( lstate ) {
      case PMSelectable::TO_DELETE:
      case PMSelectable::UNMODIFIED: // for savety
	if ( sptr->installedObj() ) {
	  fates.addUnmodified( PMSelectionPtr( sptr->installedObj() )->inspacks_ptrs() );
	  fates.addUnmodified( PMSelectionPtr( sptr->installedObj() )->delpacks_ptrs() );
	}
	break;
      case PMSelectable::TO_INSTALL:
	if ( sptr->candidateObj() ) {
	  fates.addUnmodified( PMSelectionPtr( sptr->candidateObj() )->inspacks_ptrs() );
	  fates.addUnmodified( PMSelectionPtr( sptr->candidateObj() )->delpacks_ptrs() );
	}
	break;
      }
    }

    switch ( nstate ) {
      // Here we know installed/candidate objects needed do exist.
      // E.g. state can't be TO_DELETE, if there's no installedObj.
    case PMSelectable::TO_DELETE:
      // set to delete
      fates.addOffSystem( PMSelectionPtr( sptr->installedObj() )->inspacks_ptrs() );
      fates.addUnmodified( PMSelectionPtr( sptr->installedObj() )->delpacks_ptrs() );
      break;
    case PMSelectable::UNMODIFIED:
      // keep an installed one
      if ( sptr->has_installed() ) {
	fates.addOnSystem( PMSelectionPtr( sptr->installedObj() )->inspacks_ptrs() );
	fates.addUnmodified( PMSelectionPtr( sptr->installedObj() )->delpacks_ptrs() );
      }
      break;
    case PMSelectable::TO_INSTALL:
      // set to install
      fates.addToInstall( PMSelectionPtr( sptr->candidateObj() )->inspacks_ptrs() );
      fates.addToDelete( PMSelectionPtr( sptr->candidateObj() )->delpacks_ptrs() );
      break;
    }

    // remember new state
    setLast( sptr, nstate );
  }

  ///////////////////////////////////////////////////////////////////
  // Adjust packagemanager states
  ///////////////////////////////////////////////////////////////////
  return syncToPkgMgr( fates );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::requestedLocalesChanged
//	METHOD TYPE : PMError
//
PMError PMSelectionManager::requestedLocalesChanged( const PM::LocaleSet & addLocales_r,
						     const PM::LocaleSet & delLocales_r )
{
  PkgFates fates;
  // !!! Adjust changes in language specific packages according to lastState
  // (that's what's been synced to the PMPackageManager).

  for ( PMSelectableVec::const_iterator it = begin(); it != end(); ++it ) {
    const PMSelectablePtr & sptr( *it );

    PMSelectable::Fate lstate = lastState( sptr );

    switch ( lstate ) {
    case PMSelectable::TO_DELETE:
      // nothing to do
      break;
    case PMSelectable::UNMODIFIED:
      if ( sptr->installedObj() ) {
	PMSelectionPtr csel = sptr->installedObj();	// adjust installed selection
	fates.addOnSystem( csel->inspacks_ptrs( addLocales_r ) );
	fates.addOffSystem( csel->inspacks_ptrs( delLocales_r ) );
      }
      break;
    case PMSelectable::TO_INSTALL:
      if ( sptr->candidateObj() ) {
	PMSelectionPtr csel = sptr->candidateObj();	// adjust selection to install
	fates.addToInstall( csel->inspacks_ptrs( addLocales_r ) );
	fates.addOffSystem( csel->inspacks_ptrs( delLocales_r ) );
      }
      break;
    }
  }

  // process fates
  fates.setNothingProcessed();

  // added packages for selections to install are (re)installed
  for ( PMSelectableVec::const_iterator it = fates.pkgToInstall.begin(); it != fates.pkgToInstall.end(); ++it ) {
    (*it)->appl_set_install();
  }
  fates.processedToInstall();

  // added packages for already installed selections are installed if missing
  for ( PMSelectableVec::const_iterator it = fates.pkgOnSystem.begin(); it != fates.pkgOnSystem.end(); ++it ) {
    if ( fates.didProcess( *it ) ) {
      continue; // don't revert previous setting
    }
    (*it)->appl_set_onSystem();
  }
  fates.processedOnSystem();

  // deleted packages are
  for ( PMSelectableVec::const_iterator it = fates.pkgOffSystem.begin(); it != fates.pkgOffSystem.end(); ++it ) {
    if ( fates.didProcess( *it ) ) {
      continue; // don't revert previous setting
    }
    (*it)->appl_set_offSystem();
  }

  return PMError::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::installOnTarget
//	METHOD TYPE : PMError
//
PMError PMSelectionManager::installOnTarget()
{
  if ( ! Y2PM::instTarget().mayAccessSelections() ) {
    ERR << InstTarget::Error::E_SelDB_not_open << endl;
    return InstTarget::Error::E_SelDB_not_open;
  }

  PMError err;
  bool must_reload = false;

  for ( PMSelectableVec::const_iterator it = begin(); it != end(); ++it ) {
    const PMSelectablePtr & sptr( *it );

    switch ( getState( sptr ) ) {
    case -1:
      // is to delete
      must_reload = true;
      {
	Pathname selfile;
	err = PMSelectionPtr( sptr->installedObj() )->provideSelToInstall( selfile );
	if ( err ) {
	  ERR << sptr->name() << ": can't get selfile to remove: " << err << endl;
	  break;
	}
	err = Y2PM::instTarget().removeSelection( selfile );
	if ( err ) {
	  ERR << sptr->name() << ": can't remove selfile: " << err << endl;
	  break;
	}
      }
      break;
    case 0:
      // nothing to do
      break;
    case 1:
      // is to install
      must_reload = true;
      {
	Pathname selfile;
	err = PMSelectionPtr( sptr->candidateObj() )->provideSelToInstall( selfile );
	if ( err ) {
	  ERR << sptr->name() << ": can't get selfile to install: " << err << endl;
	  break;
	}
	err = Y2PM::instTarget().installSelection( selfile );
	if ( err ) {
	  ERR << sptr->name() << ": can't install selfile: " << err << endl;
	  break;
	}
      }
      break;
    }
  }

  if ( must_reload ) {
    MIL << "Installed Selections have canged. Reloading from InstTarget" << endl;
    // reload from InstTarget and reset all active selections
    _last_active.clear();
    ClearSaveState();
    poolSetInstalled( Y2PM::instTarget().getSelections() );
    for ( PMSelectableVec::const_iterator it = begin(); it != end(); ++it ) {
      (*it)->user_unset();
    }
  }

  return err ? PMError::E_error : PMError::E_ok;
}

