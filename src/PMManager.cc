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
  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
    (*it)->clearAll();
    (*it)->_mgr_detach();
  }
  _items.clear();
  _itemPool.clear();
  _savedList.clear();
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
    item->_mgr_attach( this );

    _itemPool.insert( PMSelectablePool::value_type( item->name(), item ) );
    _items.insert( item );
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
  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
    (*it)->delInstalledObj();
  }

  for ( iter_r.setBegin(); !iter_r.atEnd(); iter_r.setNext() ) {
    D__ << "--set installed object " << *iter_r << endl;
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
  DBG << "installed objects set!" << endl;
  poolAdjust();
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
    D__ << "--add object " << *iter_r << endl;
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
  DBG << "objects added!" << endl;
  poolAdjust();
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
//    DBG << "--remove object " << *iter_r << endl;
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
  }
  DBG << "objects removed!" << endl;
  poolAdjust();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::poolAdjust
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMManager::poolAdjust()
{
  DBG << "START Pool size " << _items.size() << endl;
  Rep::dumpRepStats( DBG ) << endl;

  ///////////////////////////////////////////////////////////////////
  // Clear any saved state!
  ///////////////////////////////////////////////////////////////////
  ClearSaveState();
  ///////////////////////////////////////////////////////////////////

  for ( PMSelectableVec::iterator it = begin(); it != end(); /*advanced inside*/ ) {
    if ( ! (*it) ) {
      INT << "  Null selectable" << endl;
      continue;
    }

    if ( (*it)->isEmpty() ) {
      PMSelectableVec::iterator tdel = it;
      ++it;

      // delete tdel
      if ( (*tdel)->rep_cnt() != 2 )
	DBG << "(OUTSIDE REFERENCED) ";

      // pool first
      PMSelectablePool::iterator iter = _itemPool.find( (*tdel)->name() );
      if ( iter != _itemPool.end() ) {
	_itemPool.erase( iter );
      } else
	INT << "item not in pool" << endl;
      // then _items
      (*tdel)->_mgr_detach();
      _items.erase( tdel );

    } else {
      ++it;
    }
  }

  Rep::dumpRepStats( DBG ) << endl;
  DBG << "END Pool size " << _items.size() << endl;

  checkPool(); // for sanity
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
  if ( _itemPool.size() == _items.size() )
    DBG << "Pool size " << _items.size() << endl;
  else
    INT << "Pool size missmatch " << _itemPool.size() << " <-> " << _items.size() << endl;
  Rep::dumpRepStats( DBG ) << endl;

  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
    const PMSelectablePtr & c = *it;
    if ( ! c ) {
      INT << "  Null selectable" << endl;
    } else {
      if ( c->_manager != this )
	INT << "  wrong manager->" << endl;
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::SaveState
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMManager::SaveState()
{
  _savedList.clear();
  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
    _savedList.push_back( PMSelectable::SavedState( *it ) );
  }
  DBG << "SaveState for " << _savedList.size() << " objects (" << size() << ")" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::RestoreState
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMManager::RestoreState()
{
  if ( _savedList.size() != size() ) {
    INT << "size check failed: saved " << _savedList.size() << ", current " << size() << endl;
    return false;
  }

  if ( !size() )
    return true; // nothing to do

  // quick check if mayReplay
  for ( SavedList::iterator it = _savedList.begin(); it != _savedList.end(); ++it ) {
    if ( ! it->mayReplay() ) {
      INT << "mayReplay failed on " << it->_item << endl;
      _savedList.clear();
      return false;
    }
  }

  // replay
  for ( SavedList::iterator it = _savedList.begin(); it != _savedList.end(); ++it ) {
    if ( ! it->replay() ) {
      INT << "Replay failed on " << it->_item << endl;
      INT << "Replay failed: set nothing selected in case something is messed up!" << endl;
      _savedList.clear();
      setNothingSelected();
      checkPool();
      return false;
    }
  }

  checkPool();
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::ClearSaveState
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMManager::ClearSaveState()
{
  _savedList.clear();
  DBG << "SaveState cleared!" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::setNothingSelected
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMManager::setNothingSelected()
{
  DBG << "setNothingSelected" << endl;
  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
    (*it)->setNothingSelected();
  }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::updateAllInstalled
//	METHOD TYPE : unsigned int
//
//	DESCRIPTION : go through all objects and set those 'to_install'
//		      which have an installed and a (newer) candidate
//		      return number of affected (installed,deleted) selectables
//
unsigned int
PMManager::updateAllInstalled (bool only_newer)
{
    unsigned int count = 0;

    for (PMSelectableVec::iterator it = _items.begin();
	 it != _items.end(); ++it)
    {
	if ((*it)->by_user()			// don't touch user choices
	    || (*it)->is_taboo()		// neither taboos
	    || !(*it)->has_both_objects())	// only if both exists
	{
	    continue;
	}

	// set all affected objects to_install()

	if (!only_newer
	    || (*it)->installedObj()->edition() < (*it)->candidateObj()->edition())
	{
	    (*it)->appl_set_install();
	    count++;

	    PMObjectPtr candidate = (*it)->candidateObj();

	    // look for matching provides/obsoletes

	    for (PMSolvable::PkgRelList_type::const_iterator prvIt = candidate->provides().begin();
		 prvIt != candidate->provides().end(); ++prvIt)
	    {
		for (PMSolvable::PkgRelList_type::const_iterator obsIt = candidate->obsoletes().begin();
		     obsIt != candidate->obsoletes().end(); ++obsIt)
		{
		    if (*prvIt == *obsIt)
		    {
			PMSelectablePtr target = getItem ((const std::string&)((*obsIt).name()));
			if (target
			    && !target->is_taboo()
			    && !target->by_user())
			{
			    target->appl_set_delete();
			    count++;
			}
		    }  // provides == obsoletes
		} // loop over obsoletes
	    } // loop over provides
	} // if affected by version
    } // loop over selectables

    return count;
}

FSize PMManager::SpaceDifference()
{
    FSize size = 0;
    for ( PMManager::PMSelectableVec::iterator it = this->begin(); it != this->end(); ++it )
    {
	if((*it)->to_install())
	{
	    // sub installed
	    if((*it)->has_installed())
	    {
		PMObjectPtr ptr = (*it)->installedObj();
		if(ptr)
		    size -= ptr->size();
		else
		    { INT << "ptr is NULL" << endl; }
	    }
	    // add new
	    PMObjectPtr ptr = (*it)->candidateObj();
	    if(!ptr)
		{ INT << "ptr is NULL" << endl; }

	    size += ptr->size();
	}
	else if((*it)->to_delete())
	{
	    PMObjectPtr ptr = (*it)->installedObj();
	    if(ptr)
		size -= ptr->size();
	    else
		{ INT << "ptr is NULL" << endl; }
	}
    }

    return size;
}
