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

   File:       PMSelectable.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMSelectable.h>
#include <y2pm/PMObject.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectable
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(PMSelectable);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::PMSelectable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSelectable::PMSelectable()
    : _manager( 0 )
    , _mgr_idx( no_mgr )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::PMSelectable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSelectable::PMSelectable( const PkgName& name_r )
    : _manager( 0 )
    , _mgr_idx( no_mgr )
    , _name( name_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::~PMSelectable
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMSelectable::~PMSelectable()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clistLookup
//	METHOD TYPE : PMSelectable::PMObjectList::iterator
//
//	DESCRIPTION :
//
PMSelectable::PMObjectList::iterator PMSelectable::clistLookup( PMObjectPtr obj_r )
{
  PMObjectList::iterator it = _candidateList.begin();
  for ( ; it != _candidateList.end(); ++it ) {
    if ( *it == obj_r )
      break;
  }
  return it;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::setInstalledObj
//	METHOD TYPE : PMSelectable::Error
//
//	DESCRIPTION :
//
PMSelectable::Error PMSelectable::setInstalledObj( PMObjectPtr obj_r )
{
  if ( _installedObj == obj_r )
    return E_OK;

  delInstalledObj();

  if ( obj_r ) {
    _installedObj = obj_r;
    _installedObj->_selectable = this;
  }

  return E_OK;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::delInstalledObj
//	METHOD TYPE : PMSelectable::Error
//
//	DESCRIPTION :
//
PMSelectable::Error PMSelectable::delInstalledObj()
{
  if ( _installedObj ) {
    _installedObj->_selectable = 0;
    _installedObj = 0;
  }
  return E_OK;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clistAdd
//	METHOD TYPE : PMSelectable::Error
//
//	DESCRIPTION :
//
PMSelectable::Error PMSelectable::clistAdd( PMObjectPtr obj_r )
{
  if ( !obj_r ) {
    ERR << this << " won't add NULL object" << endl;
    return E_Error;
  }
  if ( obj_r->_selectable ) {
    ERR << this << " won't add object owned by " << obj_r->_selectable << endl;
    return E_Error;
  }

  _candidateList.push_back( obj_r );
  obj_r->_selectable = this;

#warning must rerank on add
  return E_OK;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clistDel
//	METHOD TYPE : PMSelectable::Error
//
//	DESCRIPTION :
//
PMSelectable::Error PMSelectable::clistDel( PMObjectPtr obj_r )
{
  if ( !obj_r ) {
    ERR << this << " won't delete NULL object" << endl;
    return E_Error;
  }
  if ( !obj_r->_selectable ) {
    ERR << this << " now owner for " << obj_r << endl;
    return E_Error;
  }
  if ( obj_r->_selectable != this ) {
    ERR << this << " not owner of " << obj_r << endl;
    return E_Error;
  }

  PMObjectList::iterator it = clistLookup( obj_r );
  if ( it == _candidateList.end() ) {
    ERR << this << " object not in candidate list " << obj_r << endl;
    return E_Error;
  }

  obj_r->_selectable = 0;
  _candidateList.erase( it );

  // check wheter it's been the current candidate
  if ( _candidateObj == obj_r ) {
    _candidateObj = 0;
#warning must rerank on del
  }

  return E_OK;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clistClearAll
//	METHOD TYPE : PMSelectable::Error
//
//	DESCRIPTION :
//
PMSelectable::Error PMSelectable::clistClearAll()
{
  for ( PMObjectList::iterator it = _candidateList.begin(); it != _candidateList.end(); ++it ) {
    (*it)->_selectable = 0;
  }
  _candidateList.clear();

  return E_OK;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clearAll
//	METHOD TYPE : PMSelectable::Error
//
//	DESCRIPTION :
//
PMSelectable::Error PMSelectable::clearAll()
{
  delInstalledObj();
  clistClearAll();

  return E_OK;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::isEmpty
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSelectable::isEmpty() const
{
  return ! ( _installedObj || _candidateObj || _candidateList.size() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::check
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::check() const
{
  if ( isEmpty() )
    E__ << "  EMPTY SELEECTABLE" << endl;

  if ( _installedObj && _installedObj->_selectable != this )
	E__ << "  _installedObj wrong selectable-> " << _installedObj->_selectable << endl;
  if ( _candidateObj && _candidateObj->_selectable != this )
	E__ << "  _candidateObj wrong selectable-> " << _candidateObj->_selectable << endl;
  for ( PMObjectList::const_iterator it = _candidateList.begin(); it != _candidateList.end(); ++it ) {
    if ( *it ) {
      if ( (*it)->_selectable != this )
	E__ << "  in clist wrong selectable-> " << (*it)->_selectable << " for " << *it << endl;
    } else {
      E__ << "  NULL object in clist" << endl;
    }
  }
  if ( _candidateObj && _candidateList.empty() )
    E__ << "  _candidateObj but empty clist" << endl;
  if ( !_candidateObj && _candidateList.size() )
    E__ << "  clist but no candidate" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMSelectable::dumpOn( ostream & str ) const
{
  str << _name << '[' << Rep::dumpOn( str )
    << "(inst:" << _installedObj << ")"
    << "(cand:" << _candidateObj << ")"
    << "(avai:" << _candidateList.size() << ")"
    << ']';
  return str;
}

