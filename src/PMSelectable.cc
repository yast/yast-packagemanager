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

#include <y2pm/PMSelectable.h>
#include <y2pm/PMObject.h>

#include <Y2PM.h>

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
// private
//
//	METHOD NAME : PMSelectable::archCandidate
//	METHOD TYPE : PMObjectPtr
//
//	DESCRIPTION : find candidate by arch
//		May be NULL, if no candidate matching the given arch is available.
//

PMObjectPtr
PMSelectable::archCandidate (const PkgArch& arch) const
{
    for (PMObjectList::const_iterator objpos = _candidateList.begin();
	 objpos != _candidateList.end(); ++objpos)
    {
	if (arch == (*objpos)->arch())
	    return *objpos;
    }
    return PMObjectPtr();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectable::autoCandidate
//	METHOD TYPE : PMObjectPtr
//
//	DESCRIPTION : Best among the availableObjs() Determined by ranking.
//			May be NULL, if no available is better than the installed.
//

PMObjectPtr
PMSelectable::autoCandidate() const
{
#warning TBD auto candidate by source priority
    PMObjectPtr object;
    if ( !_candidateList.empty() )
    {
	// if we have an installed object, it determines the architecture
	if (_installedObj)
	{
	    object = archCandidate (_installedObj->arch());
	    if (object)
		return object;
	}

	// find best candidate by architecture
	for (std::list<PkgArch>::const_iterator archpos = Y2PM::allowedArchs().begin();
	     archpos != Y2PM::allowedArchs().end(); ++archpos)
	{
	    object = archCandidate(*archpos);
	    if (object)
		return object;
	}
	return _candidateList.front();
    }
    return object;
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
    DBG << "clearCandidate " << _candidateObj << endl;
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

  case S_Install:
    // TABOO state has no installed and no candidate set!
    // No need for extra test
    if ( !_state.has_candidate_only() )
      return false;
    return _state.user_set_install( doit );
    break;

  case S_Update:
    // TABOO state has no installed and no candidate set!
    // No need for extra test
    if ( !_state.has_both_objects() )
      return false;
    return _state.user_set_install( doit );
    break;

  case S_AutoDel:
    // TABOO state has no installed and no candidate set!
    // No need for extra test
    return _state.auto_set_delete( doit );
    break;

  case S_AutoInstall:
    // TABOO state has no installed and no candidate set!
    // No need for extra test
    if ( !_state.has_candidate_only() )
      return false;
    return _state.auto_set_install( doit );
    break;

  case S_AutoUpdate:
    // TABOO state has no installed and no candidate set!
    // No need for extra test
    if ( !_state.has_both_objects() )
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
    if ( ! clearTaboo( doit ) )
      return false; // got no candidateObj
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
  str << _name << '[';
  Rep::dumpOn( str );
  str
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
}

