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

  File:       PMPackageImEx.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>
#include <y2pm/PMPackageImEx.h>
#include <y2pm/PMPackageManager.h>

using namespace std;

#undef Y2LOG
#define Y2LOG "PMPackageImEx"

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMPackageImExPtr
//	CLASS NAME : constPMPackageImExPtr
///////////////////////////////////////////////////////////////////
IMPL_BASE_POINTER(PMPackageImEx);

/******************************************************************
**
**
**	FUNCTION NAME : pkgNameEd
**	FUNCTION TYPE : PkgNameEd
**
**	DESCRIPTION :
*/
inline PkgNameEd pkgNameEd( const constPMObjectPtr & obj_r )
{
  return PkgNameEd( obj_r->name(), obj_r->edition() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::PMPackageImEx
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMPackageImEx::PMPackageImEx()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::~PMPackageImEx
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMPackageImEx::~PMPackageImEx()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMPackageImEx::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::reset
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
inline void PMPackageImEx::reset()
{
  _onSystemPkg.clear();
  _onSystemTabooPkg.clear();
  _offSystemPkg.clear();
  _offSystemTabooPkg.clear();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::remember_onSystemPkg
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
inline bool PMPackageImEx::collect_Pkg( const constPMSelectablePtr & sel_r )
{
  if ( !sel_r )
    return false;

  if ( sel_r->by_auto() ) {
    DBG << "DROP AUTO: " << sel_r << endl;
    return false;
  }

  if ( sel_r->to_delete() ) {
    _offSystemPkg.insert( sel_r->name() );
    ERR << "offSystemPkg: " << sel_r << endl;
    return true;
  }

  if ( sel_r->to_install() ) {
    _onSystemPkg.insert( pkgNameEd( sel_r->candidateObj() ) );
    INT << "onSystemPkg: " << sel_r << endl;
    return true;
  }

  // unmodified packages:

  if ( sel_r->has_installed() ) {
    (sel_r->is_taboo() ? _onSystemTabooPkg : _onSystemPkg).insert( pkgNameEd( sel_r->installedObj() ) );
    INT << (sel_r->is_taboo() ? "onSystemTabooPkg: " : "onSystemPkg: ") << sel_r << endl;
  } else if ( sel_r->is_taboo() ) {
    _offSystemTabooPkg.insert( sel_r->name() );
    ERR << "offSystemTabooPkg: " << sel_r << endl;
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::doImport
//	METHOD TYPE : std::istream &
//
//	DESCRIPTION :
//
std::istream & PMPackageImEx::doImport( std::istream & str )
{
  return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::doExport
//	METHOD TYPE : std::ostream &
//
//	DESCRIPTION :
//
std::ostream & PMPackageImEx::doExport( std::ostream & str ) const
{
  return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::getPMState
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMPackageImEx::getPMState()
{
  reset();
  for ( PMManager::PMSelectableVec::const_iterator it = Y2PM::packageManager().begin();
	it != Y2PM::packageManager().end(); ++it ) {
    collect_Pkg( *it );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::setPMState
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMPackageImEx::setPMState()
{
  INT << "TBD" << endl;
}

