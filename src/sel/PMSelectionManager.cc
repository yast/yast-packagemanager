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
//	METHOD NAME : PMSelectionManager::activate
//	METHOD TYPE : PMError
//
PMError PMSelectionManager::activate( PMPackageManager & package_mgr )
{
  ///////////////////////////////////////////////////////////////////
  // Remember selection packages desired fate.
  ///////////////////////////////////////////////////////////////////
  // selections to install:
  PMSelectableVec pkgToDelete;   // hard: get rid of it
  PMSelectableVec pkgToInstall;  // hard: (re)install it
  // unmodified but installed selections: protect inspacks from deletion
  PMSelectableVec pkgOnSystem;   // soft: Keep it OnSystem.
  // selections to delete:
  PMSelectableVec pkgOffSystem;  // soft: if no one minds, bring it OffSystem
  // EVERYTHING ELSE:
  PMSelectableVec pkgUnmodified; // is set to unmodified

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
	  setAdd( pkgUnmodified, PMSelectionPtr( sptr->installedObj() )->inspacks_ptrs() );
	  setAdd( pkgUnmodified, PMSelectionPtr( sptr->installedObj() )->delpacks_ptrs() );
	}
	break;
      case PMSelectable::TO_INSTALL:
	if ( sptr->candidateObj() ) {
	  setAdd( pkgUnmodified, PMSelectionPtr( sptr->candidateObj() )->inspacks_ptrs() );
	  setAdd( pkgUnmodified, PMSelectionPtr( sptr->candidateObj() )->delpacks_ptrs() );
	}
	break;
	break;
      }
    }

    switch ( nstate ) {
      // Here we know installed/candidate objects needed do exist.
      // E.g. state can't be TO_DELETE, if there's no installedObj.
    case PMSelectable::TO_DELETE:
      // set to delete
      setAdd( pkgOffSystem, PMSelectionPtr( sptr->installedObj() )->inspacks_ptrs() );
      setAdd( pkgUnmodified, PMSelectionPtr( sptr->installedObj() )->delpacks_ptrs() );
      break;
    case PMSelectable::UNMODIFIED:
      // keep an installed one
      if ( sptr->has_installed() ) {
	setAdd( pkgOnSystem, PMSelectionPtr( sptr->installedObj() )->inspacks_ptrs() );
	setAdd( pkgUnmodified, PMSelectionPtr( sptr->installedObj() )->delpacks_ptrs() );
      }
      break;
    case PMSelectable::TO_INSTALL:
      // set to install
      setAdd( pkgToInstall, PMSelectionPtr( sptr->candidateObj() )->inspacks_ptrs() );
      setAdd( pkgToDelete, PMSelectionPtr( sptr->candidateObj() )->delpacks_ptrs() );
      break;
    }

    // remember new state
    setLast( sptr, nstate );
  }

  ///////////////////////////////////////////////////////////////////
  // Adjust packagemanager states
  ///////////////////////////////////////////////////////////////////
  // First of all unset everyting collected in pkgUnmodified. Might be some of them
  // are set again below, but we don't mind.
  for ( PMSelectableVec::const_iterator it = pkgUnmodified.begin(); it != pkgUnmodified.end(); ++it ) {
    (*it)->appl_unset();
  }
  // now all modification requests are user requests
  PMSelectableVec pkgProcessed;


  // always delete delpacks
  for ( PMSelectableVec::const_iterator it = pkgToDelete.begin(); it != pkgToDelete.end(); ++it ) {
    (*it)->appl_set_delete(); // hard delete
  }
  setAdd( pkgProcessed, pkgToDelete );
  pkgToDelete.clear();


  // install inspacks
  for ( PMSelectableVec::const_iterator it = pkgToInstall.begin(); it != pkgToInstall.end(); ++it ) {
    if ( pkgProcessed.find( *it ) != pkgProcessed.end() ) {
      continue; // unmodified selections wants to keep it.
    }
    (*it)->appl_set_install();
  }
  setAdd( pkgProcessed, pkgToInstall );
  pkgToInstall.clear();


  // protect unmodified but installed inspacks
  setAdd( pkgProcessed, pkgOnSystem );
  pkgOnSystem.clear();


  // remove deleted selections inspacks
  for ( PMSelectableVec::const_iterator it = pkgOffSystem.begin(); it != pkgOffSystem.end(); ++it ) {
    if ( pkgProcessed.find( *it ) != pkgProcessed.end() ) {
      continue; // unmodified selections wants to keep it.
    }
    (*it)->appl_set_delete();
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

