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
#include <y2pm/InstTarget.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::PMSelectionManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
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
//	DESCRIPTION :
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
//	DESCRIPTION :
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
**
**	DESCRIPTION :
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
//	DESCRIPTION :
//
void PMSelectionManager::setLast( const PMSelectablePtr & sel_r, int val_r )
{
  if ( val_r )
    _last_active[sel_r] = val_r < 0 ? -1 : 1;
  else
    _last_active[sel_r] = 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::lastState
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int PMSelectionManager::lastState( const PMSelectablePtr & sel_r ) const
{
  ActiveMap::const_iterator it( _last_active.find( sel_r ) );
  return( it == _last_active.end() ? 0 : it->second);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::getState
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int PMSelectionManager::getState( const PMSelectablePtr & sel_r ) const
{
  if ( sel_r->to_modify() ) {
    return( sel_r->to_delete() ? -1 : 1 );
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::resetSelectionPackages
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectionManager::resetSelectionPackages( const PMSelectionPtr & sel_r, PMPackageManager & package_mgr )
{
  if ( !sel_r )
    return;

  std::set<PMSelectablePtr> packs( sel_r->delpacks_ptrs() );
  for ( std::set<PMSelectablePtr>::const_iterator it = packs.begin(); it != packs.end(); ++it ) {
    (*it)->appl_unset();
  }

  packs = sel_r->inspacks_ptrs();
  for ( std::set<PMSelectablePtr>::const_iterator it = packs.begin(); it != packs.end(); ++it ) {
    (*it)->appl_unset();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::setSelectionPackages
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectionManager::setSelectionPackages( const PMSelectionPtr & sel_r, PMPackageManager & package_mgr )
{
  if ( !sel_r )
    return;

  std::set<PMSelectablePtr> packs( sel_r->delpacks_ptrs() );
  for ( std::set<PMSelectablePtr>::const_iterator it = packs.begin(); it != packs.end(); ++it ) {
    (*it)->appl_set_delete();
  }

  packs = sel_r->inspacks_ptrs();
  for ( std::set<PMSelectablePtr>::const_iterator it = packs.begin(); it != packs.end(); ++it ) {
    (*it)->appl_set_install();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::removeSelectionPackages
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectionManager::removeSelectionPackages( const PMSelectionPtr & sel_r, PMPackageManager & package_mgr )
{
  if ( !sel_r )
    return;

  std::set<PMSelectablePtr> packs( sel_r->delpacks_ptrs() );
  for ( std::set<PMSelectablePtr>::const_iterator it = packs.begin(); it != packs.end(); ++it ) {
    (*it)->appl_unset();
  }

  packs = sel_r->inspacks_ptrs();
  for ( std::set<PMSelectablePtr>::const_iterator it = packs.begin(); it != packs.end(); ++it ) {
    (*it)->appl_set_delete();
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : activate
**	FUNCTION TYPE : PMError
**
**	DESCRIPTION : activate all "selected" selections by
**			going through all their packages and
**			setting them to "selected"
*/
PMError
PMSelectionManager::activate (PMPackageManager & package_mgr)
{
  for ( PMSelectableVec::const_iterator it = begin(); it != end(); ++it ) {
    const PMSelectablePtr & sptr( *it );

    int lstate = lastState( sptr );
    int nstate = getState( sptr );
    if ( lstate != nstate ) {

      // Reset last state to unmodified
      switch ( lstate ) {
      case -1:
	// was to delete
	resetSelectionPackages( sptr->installedObj(), package_mgr );
	break;
      case 0:
	// nothing to do
	break;
      case 1:
	// was to install
	resetSelectionPackages( sptr->candidateObj(), package_mgr );
	break;
      }


      // Now adjust new state
      switch ( nstate ) {
      case -1:
	// set to delete
	removeSelectionPackages( sptr->installedObj(), package_mgr );
	break;
      case 0:
	// nothing to do
	break;
      case 1:
	// set to install
	setSelectionPackages( sptr->candidateObj(), package_mgr );
	break;
      }

      // remember new state
      setLast( sptr, nstate );
    }
  }

  return PMError::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::installOnTarget
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
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
    poolSetInstalled( Y2PM::instTarget().getSelections() );
    for ( PMSelectableVec::const_iterator it = begin(); it != end(); ++it ) {
      (*it)->user_unset();
    }
  }

  return err ? PMError::E_error : PMError::E_ok;
}

