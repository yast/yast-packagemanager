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

  File:       SelState.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2pm/SelState.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::SelState
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
SelState::SelState()
    : _bits( 0 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::~SelState
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
SelState::~SelState()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::set_has_installed
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void SelState::set_has_installed( bool b )
{
  if ( has_installed() == b )
    return;
  if ( b ) {
    set( B_IS_I );
  } else {
    clr( B_IS_I );
    if ( to_delete() )
      user_unset( true );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::set_has_candidate
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void SelState::set_has_candidate( bool b )
{
  if ( has_candidate() == b )
    return;
  if ( b ) {
    set( B_IS_C );
  } else {
    clr( B_IS_C | B_F_SRCINS );
    if ( to_install() )
      user_unset( true );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::user_unset
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::user_unset( const bool doit )
{
  if ( doit ) {
    clr( M_TO | M_BY ); // don't clear B_F_TABOO
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::user_set_delete
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::user_set_delete( const bool doit )
{
  if ( ! has_installed() )
    return false;
  if ( doit ) {
    clr( M_TO | M_BY | B_F_TABOO );
    set( B_TO_DEL | B_BY_USER );
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::user_set_install
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::user_set_install( const bool doit )
{
  if ( ! has_candidate() )
    return false;
  if ( doit ) {
    clr( M_TO | M_BY | B_F_TABOO );
    set( B_TO_INS | B_BY_USER );
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::user_set_taboo
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::user_set_taboo( const bool doit )
{
  if ( doit ) {
    user_unset( true );
    set( B_F_TABOO );
  }

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::user_clr_taboo
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::user_clr_taboo( const bool doit )
{
  if ( doit ) {
    clr( B_F_TABOO );
  }

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::user_set_srcins
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::user_set_srcins( const bool doit )
{
  if ( ! has_candidate() )
    return false;
  if ( doit ) {
    set( B_F_SRCINS );
  }

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::user_clr_srcins
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::user_clr_srcins( const bool doit )
{
  if ( doit ) {
    clr( B_F_SRCINS );
  }

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::appl_unset
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::appl_unset( const bool doit )
{
  if ( by_user() )
    return false;
  if ( doit ) {
    clr( M_TO | M_BY );
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::appl_set_delete
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::appl_set_delete( const bool doit )
{
  if ( to_delete() )
    return true;
  if ( by_user() || !has_installed() || is_taboo() )
    return false;
  if ( doit ) {
    clr( M_TO | M_BY );
    set( B_TO_DEL | B_BY_APPL );
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::appl_set_install
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::appl_set_install( const bool doit )
{
  if ( to_install() )
    return true;
  if ( by_user() || !has_candidate() || is_taboo() )
    return false;
  if ( doit ) {
    clr( M_TO | M_BY );
    set( B_TO_INS | B_BY_APPL );
  }
  return true;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::auto_unset
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::auto_unset( const bool doit )
{
  if ( !by_auto() )
    return false;
  if ( doit ) {
    clr( M_TO );
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::auto_set_delete
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::auto_set_delete( const bool doit )
{
  if ( to_delete() )
    return true;
  if ( !by_auto() || !has_installed() || is_taboo() )
    return false;
  if ( doit ) {
    clr( M_TO );
    set( B_TO_DEL );
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : SelState::auto_set_install
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool SelState::auto_set_install( const bool doit )
{
  if ( to_install() )
    return true;
  if ( !by_auto() || !has_candidate() || is_taboo() )
    return false;
  if ( doit ) {
    clr( M_TO );
    set( B_TO_INS );
  }
  return true;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const SelState & obj )
{
  str << "["
    << ( obj.has_installed() ? 'i' : '-' )
    << ( obj.has_candidate() ? 'c' : '-' )
      << "|"
    << ( obj.to_delete()  ? 'D' : '.' )
    << ( obj.to_install() ? 'I' : '.' )
    << ( obj.is_taboo()   ? 'T' : '.' )
      << "|"
    << ( obj.by_user() ? 'u' : '.' )
    << ( obj.by_appl() ? 's' : '.' )
      << "|"
    << ( obj.is_srcins() ? 'X' : '.' )
      << ']';
  return str;
}

