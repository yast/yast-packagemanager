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
#include <fstream>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>

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
    : installed(NULL), available(NULL), toinstall(NULL), nowinstalled(NULL)
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
//	METHOD NAME : PMManager::selectableNotify
//	METHOD TYPE : void
//
void PMManager::selectableNotify( constPMSelectablePtr item_r, SelState old_r, SelState new_r )
{
  _D__("SelectableNotify") << item_r->name() << old_r << " -> " << new_r << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::invalidateSolverSets
//	METHOD TYPE : void
//
void PMManager::invalidateSolverSets()
{
  delete installed;
  installed = 0;
  delete available;
  available = 0;;
  delete toinstall;
  toinstall = 0;;
  delete nowinstalled;
  nowinstalled = 0;;
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

  ClearSaveState();
  invalidateSolverSets();

  for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
    (*it)->clearAll();
    (*it)->_mgr_detach();
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
  PMSelectablePtr ret( new PMSelectable( name_r, &PMManager::selectableNotify ) );
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
  prePSI();

  ///////////////////////////////////////////////////////////////////
  // Get a copy of all items. Items processed when evaluating iter_r
  // content will be removed. What's remaining will get it's installed
  // object unset.
  ///////////////////////////////////////////////////////////////////
  PMSelectableVec unprocessed( _items );

  ///////////////////////////////////////////////////////////////////
  // Evaluate iter_r
  ///////////////////////////////////////////////////////////////////
  for ( iter_r.setBegin(); !iter_r.atEnd(); iter_r.setNext() ) {
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
    // Assert there's a Selectable in the pool that gets the Object.
    // Place the Object inside its Selectable.
    ///////////////////////////////////////////////////////////////////
    PMSelectablePtr pitem = poolProvide( iter_r->name() );
    pitem->setInstalledObj( *iter_r );

    unprocessed.erase( pitem );
  }

  ///////////////////////////////////////////////////////////////////
  // Remove installed object from unprocessed items
  ///////////////////////////////////////////////////////////////////
  for ( PMSelectableVec::iterator it = unprocessed.begin(); it != unprocessed.end(); ++it ) {
    (*it)->delInstalledObj();
  }

  DBG << "installed objects set!" << endl;
  unprocessed.clear(); // drop references
  poolAdjust();
  postPSI();
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
  prePAC();

  for ( iter_r.setBegin(); !iter_r.atEnd(); iter_r.setNext() ) {
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
    pitem->clistAdd( *iter_r );
  }
  DBG << "objects added!" << endl;
  poolAdjust();
  postPAC();
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
  prePRC();

  for ( iter_r.setBegin(); !iter_r.atEnd(); iter_r.setNext() ) {
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
  postPRC();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::poolSortCandidates
//	METHOD TYPE : void
//
void PMManager::poolSortCandidates()
{
  if ( !_items.size() ) {
    return;
  }
  MIL << "Going to sort " << _items.size() << " candidate lists..." << endl;
  for( PMSelectableVec::const_iterator it = begin(); it != end(); ++it ) {
    (*it)->clistSort();
  }
  DBG << "candidate lists sorted!" << endl;
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
  MIL << "START " << _items.size() << " Selectables" << endl;

  ///////////////////////////////////////////////////////////////////
  // Trow away stuff we can't reuse, after pool changed
  ///////////////////////////////////////////////////////////////////
  ClearSaveState();
  invalidateSolverSets();
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
      if ( (*tdel)->refCount() != 2 )
	D__ << "(OUTSIDE REFERENCED) delete " << *tdel << endl;;

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

  checkPool(); // for sanity

  MIL << "END " << _items.size() << " Selectables" << endl;
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
    D__ << "Pool size " << _items.size() << endl;
  else
    INT << "Pool size missmatch " << _itemPool.size() << " <-> " << _items.size() << endl;

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
//	METHOD NAME : PMManager::DiffState
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMManager::DiffState() const
{
  if ( _savedList.size() != size() ) {
    INT << "size check failed: saved " << _savedList.size() << ", current " << size() << endl;
    return true;
  }

  if ( !size() )
    return false; // nothing to do

  // check
  for ( SavedList::const_iterator it = _savedList.begin(); it != _savedList.end(); ++it ) {
    if ( ! it->mayReplay() ) {
      INT << "mayReplay failed on " << it->_item << endl;
      return true;
    }
    if ( it->diff() ) {
      return true;
    }
  }

  return false;
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
//	METHOD NAME : PMManager::anyMatch
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMManager::anyMatch( PMSelectable::Test_method fnc_r ) const
{
  if ( !fnc_r )
    return false;
  for ( PMSelectableVec::const_iterator it = begin(); it != end(); ++it ) {
    if ( PMSelectable::test( *it, fnc_r ) )
      return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::anyMatch
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMManager::anyMatch( PMSelectable::Test_fnc fnc_r ) const
{
  if ( !fnc_r )
    return false;
  for ( PMSelectableVec::const_iterator it = begin(); it != end(); ++it ) {
    if ( (*fnc_r)( *it ) )
      return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::readSettings
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMManager::readSettings()
{
  Pathname file = settingsFile();

  if ( file.empty() ) return;

  PathInfo pi( file );
  if ( !pi.isExist() ) {
    DBG << "Settings file '" << file << "' doesn't exist." << endl;
    return;
  }

  ifstream in( file.asString().c_str() );
  if ( in.fail() ) {
    ERR << "Can't read settings from '" << file << "'" << endl;
  }

  bool parseTaboo = false;

  string line;
  while( getline( in, line ) ) {
    if ( *line.begin() == '[' ) {
      if ( line == "[Taboo]" ) parseTaboo = true;
    } else {
      if ( parseTaboo ) {
        PMSelectablePtr selectable = poolLookup( line );
        if ( selectable ) {
          selectable->set_status( PMSelectable::S_Taboo );
        } else {
          WAR << "Not in pool: " << line << endl;
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::writeSettings
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMManager::writeSettings()
{
  D__ << "writeSettings()" << endl;

  Pathname file = settingsFile();
  if ( file.empty() ) return;

  D__ << "file: " << file << endl;

  ofstream out( file.asString().c_str() );
  if ( out.fail() ) {
    ERR << "Can't write settings to '" << file << "'" << endl;
    return;
  }

  out << "[Taboo]" << endl;

  PMSelectableVec::const_iterator it;
  for( it = begin(); it != end(); ++it ) {
    if ( (*it)->is_taboo() ) {
      D__ << "taboo: " << (*it)->name() << endl;
      out << (*it)->name() << endl;
    }
  }
}
