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

IMPL_BASE_POINTER(PMSelectable);

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
  if ( _manager ) {
    INT << "SUSPICIOUS: " << *this << ": " << _manager << '|' << _mgr_idx << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::_mgr_attach
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::_mgr_attach( PMManager * mgr_r, const unsigned idx_r )
{
  if ( _manager || !mgr_r || idx_r == no_mgr ) {
    INT << "SUSPICIOUS: " << *this << ": " << _manager << '|' << _mgr_idx << " -> " << mgr_r << '|' << idx_r << endl;
  }
  _manager = mgr_r;
  _mgr_idx = idx_r;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::_mgr_detach
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::_mgr_detach()
{
  if ( !_manager ) {
    INT << "SUSPICIOUS: not attached! " << *this << endl;
  }
  _manager = 0;
  _mgr_idx = no_mgr;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::_attach_obj
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::_attach_obj( PMObjectPtr & obj_r )
{
  if ( obj_r ) {
    obj_r->_selectable = this;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::_detach_obj
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::_detach_obj( PMObjectPtr & obj_r )
{
  if ( obj_r ) {
    obj_r->_selectable = 0;
  }
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
    return E_Ok;

  delInstalledObj();

  if ( obj_r ) {
    _installedObj = obj_r;
    _attach_obj( obj_r );
    _state.set_has_installed( true );
  }

  return E_Ok;
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
    _detach_obj( _installedObj );
    _installedObj = 0;
    _state.set_has_installed( false );
  }
  return E_Ok;
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
  _attach_obj( obj_r );

#warning must rerank on add

  chooseCandidateObj();

  return E_Ok;
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
    ERR << this << " no owner for " << obj_r << endl;
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

  _detach_obj( obj_r );
  _candidateList.erase( it );

#warning must rerank on del

  if ( _userCandidateObj == obj_r ) {
    _userCandidateObj = 0;
  }
  if ( _candidateObj == obj_r ) {
    _candidateObj = 0;
  }
  chooseCandidateObj();

  return E_Ok;
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
    _detach_obj( *it );
  }
  _candidateList.clear();
  _userCandidateObj = 0;
  clearCandidateObj();

  return E_Ok;
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

  return E_Ok;
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
//	METHOD NAME : PMSelectable::chooseCandidateObj
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::chooseCandidateObj()
{
  if ( _state.is_taboo() ) {
    return;
  }

  PMObjectPtr newcand = bestCandidate();
  if ( _candidateObj != newcand ) {
    DBG << "chooseCandidate: old " << _candidateObj << " -> new " << newcand << endl;
    _candidateObj = newcand;
    _state.set_has_candidate( _candidateObj );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clearCandidateObj
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::clearCandidateObj()
{
   if ( _candidateObj ) {
     DBG << "clearCandidate " << _candidateObj << endl;
     _candidateObj = 0;
     _state.set_has_candidate( false );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::setUserCandidate
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSelectable::setUserCandidate( const PMObjectPtr & obj_r )
{
  if ( obj_r == userCandidate() )
    return true;

  if ( obj_r ) {
    if ( ! obj_r->_selectable ) {
      ERR << this << " no owner for " << obj_r << endl;
      return false;
    }
    if ( obj_r->_selectable != this ) {
      ERR << this << " not owner of " << obj_r << endl;
      return false;
    }
  }

  _userCandidateObj = obj_r;
  chooseCandidateObj();

  return true;
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
    << "(state:" << _state << ")"
    << ']';
  return str;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clearTaboo
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSelectable::clearTaboo( const bool doit )
{
  if ( _state.is_taboo() ) {
    if ( doit ) {
      _state.user_clr_taboo( doit );
      chooseCandidateObj();
    } else {
      return bestCandidate(); // wheter we'd get one if...
    }
  }
  return _candidateObj;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::intern_set_status
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSelectable::intern_set_status( const UI_Status state_r, const bool doit )
{
  switch ( state_r ) {

  case S_Taboo:
    if ( _state.user_set_taboo( doit ) ) {
      if ( doit )
	clearCandidateObj();
      return true;
    }
    return false;
    break;

  case S_Del:
    // TABOO state has no installed and no candidate set!
    // No need for extra test
    return _state.user_set_delete( doit );
    break;

  case S_Update:
    // TABOO state has no installed and no candidate set!
    // No need for extra test
    if ( !_state.has_both_objects() )
      return false;
    return _state.user_set_install( doit );
    break;

  case S_Install:
    if ( ! clearTaboo( doit ) )
      return false; // got no candidateObj
    if ( !_state.has_candidate_only() )
      return false;
    return _state.user_set_install( doit );
    break;

  case S_Auto:
    // TABOO state has no installed and no candidate set!
    // No need for extra test
    if ( !_state.has_candidate_only() )
      return false;
    return _state.auto_set_install( doit );
    break;

  case S_KeepInstalled:
    // TABOO state has no installed and no candidate set!
    // No need for extra test
    if ( ! _state.has_installed() )
      return false;
    return _state.user_unset( doit );
    break;

  case S_NoInst:
    // TABOO state has no installed and no candidate set!
    // MUST TEST
    if ( _state.is_taboo() )
      return false;
    if ( _state.has_installed() )
      return false;
    return _state.user_unset( doit );
    break;

  }

  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::status
//	METHOD TYPE : PMSelectable::UI_Status
//
//	DESCRIPTION :
//
PMSelectable::UI_Status PMSelectable::status() const
{
  if ( !_state.to_modify() ) {
    if ( _state.has_installed() )
      return S_KeepInstalled;
    return( _state.is_taboo() ? S_Taboo : S_NoInst );
  }

  if ( _state.to_install() ) {
    if ( _state.has_installed() )
      return S_Update;
    return( _state.by_user() ? S_Install : S_Auto );
  }

  // _state.to_delete
  return S_Del;
}

