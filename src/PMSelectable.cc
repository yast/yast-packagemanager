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
#include <list>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>
#include <y2pm/PMSelectable.h>
#include <y2pm/PMObject.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMVendorAttr.h>
#include <y2pm/PMManager.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectable
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMSelectable);

///////////////////////////////////////////////////////////////////

#define DEAFAULT_CLISTCOMPARE clistAVS

PMSelectable::ClistCompare PMSelectable::_clistCompare = DEAFAULT_CLISTCOMPARE;

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::setCandidateOrder
//	METHOD TYPE : bool
//
bool PMSelectable::setCandidateOrder( PM::CandidateOrder neworder_r )
{
  ClistCompare newcomp_r = DEAFAULT_CLISTCOMPARE;

  switch ( neworder_r ) {
  case PM::CO_AVS:
    newcomp_r = clistAVS;
    break;
  case PM::CO_ASV:
    newcomp_r = clistASV;
    break;
  case PM::CO_DEFAULT:
    // default
    break;
  }

  if ( _clistCompare == newcomp_r )
    return false;

  _clistCompare = newcomp_r;
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clistAVS
//	METHOD TYPE : bool
//
//	DESCRIPTION : Compare objects by Arch, Version then instSrc priority.
//
bool PMSelectable::clistAVS( const PMObjectPtr & lhs, const PMObjectPtr & rhs )
{
  ///////////////////////////////////////////////////////////////////
  // arch
  ///////////////////////////////////////////////////////////////////
  int acmp = PkgArch::compare( lhs->arch(), rhs->arch() );
  if ( acmp < 0 )
    return true; // lhs better arch

  if ( acmp == 0 ) {
    ///////////////////////////////////////////////////////////////////
    // version
    ///////////////////////////////////////////////////////////////////
    if ( lhs->edition() > rhs->edition() )
      return true; // lhs better version

    if ( lhs->edition() == rhs->edition() ) {
      ///////////////////////////////////////////////////////////////////
      // instSrc priority
      ///////////////////////////////////////////////////////////////////
      if ( lhs->instSrcRank() < rhs->instSrcRank() )
	return true; // lhs lower rank -> higher priority
    }
  }
  ///////////////////////////////////////////////////////////////////
  // ran out of sort criteria
  ///////////////////////////////////////////////////////////////////
  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clistASV
//	METHOD TYPE : bool
//
//	DESCRIPTION : Compare objects by Arch, instSrc priority then Version.
//
bool PMSelectable::clistASV( const PMObjectPtr & lhs, const PMObjectPtr & rhs )
{
  ///////////////////////////////////////////////////////////////////
  // arch
  ///////////////////////////////////////////////////////////////////
  int acmp = PkgArch::compare( lhs->arch(), rhs->arch() );
  if ( acmp < 0 )
    return true; // lhs better arch

  if ( acmp == 0 ) {
    ///////////////////////////////////////////////////////////////////
    // instSrc priority
    ///////////////////////////////////////////////////////////////////
    if ( lhs->instSrcRank() < rhs->instSrcRank() )
      return true; // lhs lower rank -> higher priority

    if ( lhs->instSrcRank() == rhs->instSrcRank() ) {
      ///////////////////////////////////////////////////////////////////
      // version
      ///////////////////////////////////////////////////////////////////
      if ( lhs->edition() > rhs->edition() )
	return true; // lhs better version
    }
  }
  ///////////////////////////////////////////////////////////////////
  // ran out of sort criteria
  ///////////////////////////////////////////////////////////////////
  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::PMSelectable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
//PMSelectable::PMSelectable()
//    : _manager( 0 )
//    , _observedState( _state, _name, _manager, 0 )
//{
//}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::PMSelectable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSelectable::PMSelectable( const PkgName& name_r, SelectableNotify notify_Fnc_r )
    : _manager( 0 )
    , _name( name_r )
    , _observedState( _state, this, _manager, notify_Fnc_r )
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
    INT << "SUSPICIOUS: still managed " << *this << ": " << _manager << endl;
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
void PMSelectable::_mgr_attach( PMManager * mgr_r )
{
  if ( _manager || !mgr_r ) {
    INT << "SUSPICIOUS: " << *this << ": " << _manager << " -> " << mgr_r << endl;
  }
  _manager = mgr_r;
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
//	METHOD NAME : PMSelectable::autoCandidate
//	METHOD TYPE : PMObjectPtr
//
//	DESCRIPTION : Best among the availableObjs(). May be NULL, if no
//                    available is better than the installed.
//
//      - candidateList is sorted, best candidate first.
//      - if we have an installed object, it determines the desired architecture.
//        (Except there is a candidate that comes from a source with prefererCandidate fag set).
//      - Otherwise the fist is the best.
//
PMObjectPtr PMSelectable::autoCandidate() const
{
  if ( _candidateList.empty() )
    return  PMObjectPtr();

  if ( _installedObj ) {
    for ( PMObjectList::const_iterator it = _candidateList.begin(); it != _candidateList.end(); ++it ) {
      if ( (*it)->prefererCandidate() || (*it)->arch() == _installedObj->arch() )
	return *it;
    }
  }

  return *_candidateList.begin();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::setInstalledObj
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::setInstalledObj( PMObjectPtr obj_r )
{
  if ( _installedObj == obj_r )
    return;

  if ( _installedObj ) {
    _detach_obj( _installedObj );
    _installedObj = 0;
  }
  if ( obj_r ) {
    _installedObj = obj_r;
    _attach_obj( obj_r );
  }
  _state.set_has_installed( _installedObj );

  chooseCandidateObj(); // installed arch influences it
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clistIsBetter
//	METHOD TYPE : bool
//
//	DESCRIPTION : Return true if lhs is better candidate than rhs.
//
bool PMSelectable::clistIsBetter( const PMObjectPtr & lhs, const PMObjectPtr & rhs )
{
  if ( lhs == rhs )
    return false;

  if ( !lhs )
    return false; // any rhs better than nothing

  if ( !rhs )
    return true; // any lhs better than nothing

  if ( lhs->prefererCandidate() != rhs->prefererCandidate() ) {
    return lhs->prefererCandidate(); // either lhs or rhs is always best.
  }

  return _clistCompare( lhs, rhs ); // compare objects
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clistAdd
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::clistAdd( PMObjectPtr obj_r )
{
  if ( !obj_r ) {
    INT << this << " won't add NULL object" << endl;
    return;
  }

  // sorted add: autocandidate rely's on this!
  PMObjectList::iterator it = _candidateList.begin();
  for ( ; it != _candidateList.end(); ++it ) {
    if ( ! clistIsBetter( *it, obj_r ) )
      break;
  }
  // Here: insert
  _candidateList.insert( it, obj_r );
  _attach_obj( obj_r );

  chooseCandidateObj();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clistDel
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::clistDel( PMObjectPtr obj_r )
{
  if ( !obj_r ) {
    INT << this << " won't delete NULL object" << endl;
    return;
  }

  PMObjectList::iterator it = clistLookup( obj_r );
  if ( it == _candidateList.end() ) {
    INT << this << " object not in candidate list " << obj_r << endl;
    return;
  }

  _detach_obj( obj_r );
  _candidateList.erase( it );

  if ( _userCandidateObj == obj_r ) {
    _userCandidateObj = 0;
  }
  if ( _candidateObj == obj_r ) {
    _candidateObj = 0;
  }
  chooseCandidateObj();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clistSort
//	METHOD TYPE : void
//
void PMSelectable::clistSort()
{
  _candidateList.sort( clistIsBetter );
  chooseCandidateObj();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clistClearAll
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::clistClearAll()
{
  for ( PMObjectList::iterator it = _candidateList.begin(); it != _candidateList.end(); ++it ) {
    _detach_obj( *it );
  }
  _candidateList.clear();
  _userCandidateObj = 0;
  clearCandidateObj();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::clearAll
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::clearAll()
{
  delInstalledObj();
  clistClearAll();
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
//	METHOD NAME : PMSelectable::chooseCandidateObj
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMSelectable::chooseCandidateObj()
{
  PMObjectPtr newcand = bestCandidate();
  if ( _candidateObj != newcand ) {
    _candidateObj = newcand;
  }
  _state.set_has_candidate( _candidateObj );
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
    _candidateObj = 0;
  }
  _state.set_has_candidate( false );
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
//	METHOD NAME : PMSelectable::intern_set_status
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSelectable::intern_set_status( const UI_Status state_r, const bool doit )
{
  switch ( state_r ) {

  case S_Protected:
    if ( !_state.has_installed() )
      return false;
    return _observedState( &SelState::user_set_taboo, doit );
    break;

  case S_Taboo:
    if ( _state.has_installed() )
      return false;
    return _observedState( &SelState::user_set_taboo, doit );
    break;

  case S_Del:
    return _observedState( &SelState::user_set_delete, doit );
    break;

  case S_Install:
    if ( !_state.has_candidate_only() )
      return false;
    return _observedState( &SelState::user_set_install, doit );
    break;

  case S_Update:
    if ( !_state.has_both_objects() )
      return false;
    return _observedState( &SelState::user_set_install, doit );
    break;

  case S_AutoDel:
    return _observedState( &SelState::auto_set_delete, doit );
    break;

  case S_AutoInstall:
    if ( !_state.has_candidate_only() )
      return false;
    return _observedState( &SelState::auto_set_install, doit );
    break;

  case S_AutoUpdate:
    if ( !_state.has_both_objects() )
      return false;
    return _observedState( &SelState::auto_set_install, doit );
    break;

  case S_KeepInstalled:
    if ( ! _state.has_installed() )
      return false;
    if ( doit ) {
      _observedState( &SelState::user_clr_taboo, doit ); // not done by user_unset
      _observedState( &SelState::user_unset, doit );
    }
    return true;
    break;

  case S_NoInst:
    if ( _state.has_installed() )
      return false;
    if ( doit ) {
      _observedState( &SelState::user_clr_taboo, doit ); // not done by user_unset
      _observedState( &SelState::user_unset, doit );
    }
    return true;
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
      return ( _state.is_taboo() ? S_Protected :S_KeepInstalled );
    return( _state.is_taboo() ? S_Taboo : S_NoInst );
  }

  if ( _state.to_install() ) {
    if ( _state.has_installed() )
      return ( _state.by_user() ? S_Update : S_AutoUpdate );
    return( _state.by_user() ? S_Install : S_AutoInstall );
  }

  // _state.to_delete
  return( _state.by_user() ? S_Del : S_AutoDel );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::set_status
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSelectable::set_status( const UI_Status state_r )
{
  bool ret = intern_set_status( state_r, true );

#warning SET_STATUS CHECKS ENABLED
  if ( ret && status() != state_r ) {
    INT << "SET_STATUS MISSMATCH: wanted " << state_r << " got " << status() << endl;
  }

  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::downgrade_condition
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSelectable::downgrade_condition() const
{
  if ( !has_both_objects() )
    return false; // either nothing to install or nothing to downgrade.

  PMPackagePtr inst( installedObj() );
  if ( !inst )
    return false; // not package objects

  PMPackagePtr cand( candidateObj() );

  if ( inst->edition() < cand->edition() )
    return false; // candidate is newer

  // Here: downgrade. candidate is same or older version.

  // SuSE specific exeption:
  // Version downgrade during update or due to newer buildtime is ok.
  if (    PMVendorAttr::isKnown( inst->vendor() )
       && PMVendorAttr::isKnown( cand->vendor() ) ) {
    return( Y2PM::runningFromSystem() && inst->buildtime() >= cand->buildtime() );
  }

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::providesSources
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSelectable::providesSources() const
{
  return candidateObj() && candidateObj()->providesSources();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::set_source_install
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSelectable::set_source_install( const bool install_r )
{
  if ( install_r == source_install() )
    return true;

  if ( !install_r )
    return _state.user_clr_srcins( true );

  // install
  return( providesSources() && _state.user_set_srcins( true ) );
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : std::ostream &
**
**	DESCRIPTION :
*/
std::ostream & operator<<( std::ostream & str, PMSelectable::UI_Status obj )
{
  switch ( obj ) {
#define ENUM_OUT(V) case PMSelectable::V: return str << #V; break

    ENUM_OUT( S_Protected );
    ENUM_OUT( S_Taboo );
    ENUM_OUT( S_Del );
    ENUM_OUT( S_Install );
    ENUM_OUT( S_Update );
    ENUM_OUT( S_AutoDel );
    ENUM_OUT( S_AutoInstall );
    ENUM_OUT( S_AutoUpdate );
    ENUM_OUT( S_NoInst );
    ENUM_OUT( S_KeepInstalled );

#undef ENUM_OUT
  }

  return str << "UI_Status(UNKNOWN)";
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
  str << _name << '{' << _state;
  if ( _installedObj )
    str << " i:" << _installedObj->edition() << '.' << _installedObj->arch();
  else
    str << " i:-";
  if ( _candidateObj )
    str << " c:" << _candidateObj->edition() << '.' << _candidateObj->arch();
  else
    str << " c:-";
  str << " av:" << _candidateList.size();
  return str << '}';

#if 0
  str << _name << '{';
  Rep::dumpOn( str );
  str << _state
    << " i:" << _installedObj
    << " c:" << _candidateObj
    << " av:" << _candidateList.size();
  return str << '}';
#endif
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::dumpStateOn
//	METHOD TYPE : std::ostream &
//
//	DESCRIPTION :
//
std::ostream & PMSelectable::dumpStateOn( std::ostream & str ) const
{
  return str << _state << " " << _name;
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
  bool goterr = false;

#define CHKLOG(X) do {                                                        \
  if ( !goterr ) { goterr = true; INT << "CHECK FAILED: " << *this << endl; } \
  INT << "    " << X << endl;                                                 \
} while( 0 )

  if ( isEmpty() )
    CHKLOG( "EMPTY SELEECTABLE" );

  if ( _installedObj && _installedObj->_selectable != this )
	CHKLOG( "_installedObj wrong selectable-> " << _installedObj->_selectable );

  if ( _candidateObj && _candidateObj->_selectable != this )
	CHKLOG( "_candidateObj wrong selectable-> " << _candidateObj->_selectable );

  if ( _userCandidateObj && _userCandidateObj->_selectable != this )
	CHKLOG( "_userCandidateObj wrong selectable-> " << _candidateObj->_selectable );

  for ( PMObjectList::const_iterator it = _candidateList.begin(); it != _candidateList.end(); ++it ) {
    if ( *it ) {
      if ( (*it)->_selectable != this )
	CHKLOG( "in clist wrong selectable-> " << (*it)->_selectable << " for " << *it );
    } else {
      CHKLOG( "NULL object in clist" );
    }
  }

  if ( _candidateList.empty() ) {
    if ( _candidateObj )
      CHKLOG( "_candidateObj but empty clist" );
    if ( _userCandidateObj )
      CHKLOG( "_userCandidateObj but empty clist" );
  }

  bool hi = _installedObj;
  bool hc = _candidateObj;
  if ( hi != _state.has_installed() || hc != _state.has_candidate() ) {
    CHKLOG( "state <-> object missmatch" );
  }

  if ( _state.is_taboo() && _state.to_modify() ) {
    CHKLOG( "taboo and modification set" );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::SavedState::SavedState
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSelectable::SavedState::SavedState( const PMSelectablePtr & item_r )
    : _item( item_r )
{
  if ( _item ) {
    _state            = _item->_state;
    _userCandidateObj = _item->_userCandidateObj;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::SavedState::mayReplay
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSelectable::SavedState::mayReplay() const
{
  if ( ! ( _item && _item->_manager ) )
    return false;

  if ( _userCandidateObj
       && ! (    _userCandidateObj->hasSelectable()
	      && _userCandidateObj->getSelectable() == _item ) )
    return false;

  if ( _state.has_installed() != _item->_state.has_installed() )
    return false;

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::SavedState::replay
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSelectable::SavedState::replay()
{
  _item->clearCandidateObj();
  _item->_userCandidateObj = _userCandidateObj;
  _item->_state            = _state;
  _item->chooseCandidateObj();
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::SavedState::diff
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMSelectable::SavedState::diff() const
{
  return( _state != _item->_state || _userCandidateObj != _item->_userCandidateObj );
}
