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

   File:       PMManager.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>
#include <y2util/Y2SLog.h>

#include <y2pm/PMManager.h>
#include <y2pm/PMSelectable.h>
#include <y2pm/PMObject.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::PMManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMManager::PMManager()
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::~PMManager
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMManager::~PMManager()
{
  clearAll();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::clearAll
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMManager::clearAll()
{
  DBG << "clearAll" << endl;
  for ( unsigned i = 0; i < _items.size(); ++i ) {
    _items[i]->clearAll();
    _items[i]->_mgr_detach();
  }
  _items.clear();
  _itemPool.clear();
}

///////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::newSelectable
//	METHOD TYPE : PMSelectablePtr
//
//	DESCRIPTION :
//
PMSelectablePtr PMManager::newSelectable( const PkgName & name_r ) const
{
  PMSelectablePtr ret( new PMSelectable( name_r ) );
  return ret;
}

///////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::poolLookup
//	METHOD TYPE : PMSelectablePtr
//
//	DESCRIPTION :
//
PMSelectablePtr PMManager::poolLookup( unsigned idx_r ) const
{
  if ( idx_r < _items.size() )
    return _items[idx_r];
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::poolLookup
//	METHOD TYPE : PMSelectablePtr
//
//	DESCRIPTION :
//
PMSelectablePtr PMManager::poolLookup( const std::string & name_r ) const
{
  PMSelectablePool::const_iterator iter = _itemPool.find( name_r );
  if ( iter != _itemPool.end() )
    return iter->second;
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::poolProvide
//	METHOD TYPE : PMSelectablePtr
//
//	DESCRIPTION :
//
PMSelectablePtr PMManager::poolProvide( const std::string & name_r )
{
  PMSelectablePtr item = poolLookup( name_r );
  if ( !item ) {
    // create a new one
    item = newSelectable( PkgName( name_r ) );
    item->_mgr_attach( this, _items.size() );

    _itemPool.insert( PMSelectablePool::value_type( item->name(), item ) );
    _items.push_back( item );

    M__ << "    new Selectable " << item << endl;
  } else {
    M__ << "    lookup found " << item << endl;
  }
  return item;
}

///////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::poolSetInstalled
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMManager::poolSetInstalled( PMObjectContainerIter iter_r )
{
  MIL << "Going to set " << iter_r.size() << " installed objects..." << endl;

  ///////////////////////////////////////////////////////////////////
  // set nothing installed
  ///////////////////////////////////////////////////////////////////
  for ( unsigned i = 0; i < _items.size(); ++ i ) {
    _items[i]->delInstalledObj();
  }

  for ( iter_r.setBegin(); !iter_r.atEnd(); iter_r.setNext() ) {
    M__ << "  set installed object " << *iter_r << endl;
    ///////////////////////////////////////////////////////////////////
    // check the Object.
    ///////////////////////////////////////////////////////////////////
    if ( ! *iter_r ) {
      ERR << "Refuse to set NULL object" << endl;
      continue;
    }
    if ( ! assertObjectType( *iter_r ) ) {
      // Error reported by concrete Manager
      continue;
    }
    if ( iter_r->_selectable ) {
      ERR << "Refuse to set object owned by " << iter_r->_selectable << endl;
      continue;
    }

    ///////////////////////////////////////////////////////////////////
    // assert there's a Selectable in the pool that gets the Object.
    // place the Object inside its Selectable.
    ///////////////////////////////////////////////////////////////////
    PMSelectablePtr pitem = poolProvide( iter_r->name() );
    if ( pitem->setInstalledObj( *iter_r ) ) {
      INT << "setInstalledObj failed" << endl;
    }
  }
  M__ << "installed objects set!" << endl;
  checkPool();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::poolAddCandidates
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMManager::poolAddCandidates( PMObjectContainerIter iter_r )
{
  if ( iter_r.empty() ) {
    MIL << "Request to add zero objects ignored." << endl;
    return;
  }
  MIL << "Going to add " << iter_r.size() << " objects..." << endl;

  for ( iter_r.setBegin(); !iter_r.atEnd(); iter_r.setNext() ) {
    M__ << "  add object " << *iter_r << endl;
    ///////////////////////////////////////////////////////////////////
    // check the Object.
    ///////////////////////////////////////////////////////////////////
    if ( ! *iter_r ) {
      ERR << "Refuse to add NULL object" << endl;
      continue;
    }
    if ( ! assertObjectType( *iter_r ) ) {
      // Error reported by concrete Manager
      continue;
    }
    if ( iter_r->_selectable ) {
      ERR << "Refuse to add object owned by " << iter_r->_selectable << endl;
      continue;
    }

    ///////////////////////////////////////////////////////////////////
    // assert there's a Selectable in the pool that gets the Object.
    // place the Object inside its Selectable.
    ///////////////////////////////////////////////////////////////////
    PMSelectablePtr pitem = poolProvide( iter_r->name() );
    if ( pitem->clistAdd( *iter_r ) ) {
      INT << "clistAdd failed" << endl;
    }
  }
  M__ << "objects added!" << endl;
  checkPool();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::poolRemoveCandidates
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMManager::poolRemoveCandidates( PMObjectContainerIter iter_r )
{
  if ( iter_r.empty() ) {
    MIL << "Request to remove zero objects ignored." << endl;
    return;
  }

  MIL << "Going to remove " << iter_r.size() << " objects..." << endl;

  for ( iter_r.setBegin(); !iter_r.atEnd(); iter_r.setNext() ) {
    M__ << "  remove object " << *iter_r << endl;
    ///////////////////////////////////////////////////////////////////
    // check the Object.
    ///////////////////////////////////////////////////////////////////
    if ( ! *iter_r ) {
      ERR << "Refuse to remove NULL object" << endl;
      continue;
    }
    if ( ! assertObjectType( *iter_r ) ) {
      // Error reported by concrete Manager
      continue;
    }
    if ( ! iter_r->_selectable ) {
      ERR << "Refuse to remove object without selectable" << endl;
      continue;
    }
    if ( iter_r->_selectable->_manager != this ) {
      ERR << "Refuse to remove object not managed by me" << endl;
      continue;
    }

    ///////////////////////////////////////////////////////////////////
    // remove object from it's selectable.
    ///////////////////////////////////////////////////////////////////
    PMSelectablePtr pitem = iter_r->_selectable;
    pitem->clistDel( *iter_r );

#if 0
    ///////////////////////////////////////////////////////////////////
    // check whether to drop the selectable.
    // ASSUMES _candidateList is empty if no _candidateObj.
    ///////////////////////////////////////////////////////////////////
    if ( ! ( pitem->_installedObj || pitem->_candidateObj ) ) {
#warning must save pools state on dropping items
	D__ << "    drop selectable" << endl;
	_itemPool.erase( pitem->name() );
	pitem->_mgr_detach = 0;
    }
#endif
  }
  M__ << "objects removed!" << endl;
  checkPool();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::checkPool
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMManager::checkPool() const
{
  // adjust BitFields of added items

  // test whether to remove empty items


  if ( _itemPool.size() == _items.size() )
    S__ << "Pool size " << _items.size() << endl;
  else
    E__ << "Pool size missmatch " << _itemPool.size() << " <-> " << _items.size() << endl;


  for ( unsigned i = 0; i < _items.size(); ++i ) {
    PMSelectablePtr c = _items[i];
    S__ << "[" << i << "] " << c << endl;

    if ( ! c ) {
      E__ << "  Null selectable" << endl;
    } else {
      if ( c->_manager != this )
	E__ << "  wrong manager->" << endl;
      if ( c->_mgr_idx != i )
	E__ << "  wrong _mgr_idx " << c->_mgr_idx << endl;
      c->check();
    }
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PMManager & obj )
{
  str << "PMManager" << endl;
  return str;
}

