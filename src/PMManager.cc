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

}

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
//
//	METHOD NAME : PMManager::poolLookup
//	METHOD TYPE : PMSelectablePtr
//
//	DESCRIPTION :
//
PMSelectablePtr PMManager::poolLookup( const PkgName & name_r ) const
{
  PMSelectablePool::const_iterator iter = _itemPool.find( name_r );
  if ( iter != _itemPool.end() )
    return iter->second;
  return 0;
}

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
#warning TBD set installed items
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
    DBG << "  add object " << *iter_r << endl;
    ///////////////////////////////////////////////////////////////////
    // check the Object.
    ///////////////////////////////////////////////////////////////////
    if ( ! *iter_r ) {
      ERR << "Refuse to add NULL object" << endl;
      continue;
    }
    if ( iter_r->_selectable ) {
      ERR << "Refuse to add object owned by " << iter_r->_selectable << endl;
      continue;
    }

    ///////////////////////////////////////////////////////////////////
    // assert there's a Selectable in the pool that gets the Object.
    ///////////////////////////////////////////////////////////////////
    PMSelectablePtr pitem = poolLookup( iter_r->name() );
    DBG << "    lookup found " << pitem << endl;
    if ( !pitem ) {
      // provide a new selectable and add it to the pool
      pitem = newSelectable( iter_r->name() );
      DBG << "    newSelectable " << pitem << endl;
#warning must save pools state on adding new items
      _itemPool.insert( PMSelectablePool::value_type( pitem->name(), pitem ) );
      pitem->_manager = this;
      DBG << "    add new selectable " << pitem << endl;
    }

    ///////////////////////////////////////////////////////////////////
    // place the Object inside its Selectable.
    ///////////////////////////////////////////////////////////////////
#warning TBD duplicate check and ranking for new object
    iter_r->_selectable = pitem;
    pitem->_candidateList.push_back( *iter_r );
    DBG << "    add object " << pitem << "; candidateList size " << pitem->_candidateList.size() << endl;
    if ( !pitem->_candidateObj ) {
      pitem->_candidateObj = *iter_r;
      DBG << "    select object as new candidate" << endl;
    }
  }

  DBG << "objects added!" << endl;
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
    DBG << "  remove object " << *iter_r << endl;
    ///////////////////////////////////////////////////////////////////
    // check the Object.
    ///////////////////////////////////////////////////////////////////
    if ( ! *iter_r ) {
      ERR << "Refuse to remove NULL object" << endl;
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
    iter_r->_selectable = 0;
    unsigned oclsize = pitem->_candidateList.size();
    pitem->_candidateList.remove( *iter_r );
    if ( pitem->_candidateList.size() + 1 != oclsize ) {
      ERR << "Suspicious remove from candidateList: size " << oclsize << " -> " << pitem->_candidateList.size() << endl;
    }
    DBG << "    remove object " << pitem << "; candidateList size " << pitem->_candidateList.size() << endl;
    if ( pitem->_candidateObj == *iter_r ) {
#warning TBD ranking after dropped object
      if ( pitem->_candidateList.size() ) {
	pitem->_candidateObj = *pitem->_candidateList.begin();
	DBG << "    select new candidate " << pitem->_candidateObj << endl;
      } else {
	pitem->_candidateObj = 0;
	DBG << "    drop candidate" << endl;
      }
    }

    ///////////////////////////////////////////////////////////////////
    // check whether to drop the selectable.
    // ASSUMES _candidateList is empty if no _candidateObj.
    ///////////////////////////////////////////////////////////////////
    if ( ! ( pitem->_installedObj || pitem->_candidateObj ) ) {
#warning must save pools state on dropping items
	DBG << "    drop selectable" << endl;
	_itemPool.erase( pitem->name() );
	pitem->_manager = 0;
    }
  }

  DBG << "objects removed!" << endl;
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
  return str;
}

