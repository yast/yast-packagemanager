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
      user_unset();
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
    clr( B_IS_C );
    if ( to_install() )
      user_unset();
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
bool SelState::user_unset()
{
  clr( M_TO | B_BY_USER );
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
bool SelState::user_set_delete()
{
  if ( ! has_installed() )
    return false;
  clr( M_TO );
  set( B_TO_DEL | B_BY_USER );
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
bool SelState::user_set_install()
{
  if ( ! has_candidate() )
    return false;
  clr( M_TO );
  set( B_TO_INS | B_BY_USER );
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
bool SelState::auto_unset()
{
  if ( by_user() )
    return false;
  clr( M_TO );
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
bool SelState::auto_set_delete()
{
  if ( to_delete() )
    return true;
  if ( by_user() || ! has_installed() )
    return false;
  clr( M_TO );
  set( B_TO_DEL );
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
bool SelState::auto_set_install()
{
  if ( to_install() )
    return true;
  if ( by_user() || ! has_candidate() )
    return false;
  clr( M_TO );
  set( B_TO_INS );
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
  str << '['
    << ( obj.has_installed() ? 'i' : ' ' )
    << ( obj.has_candidate() ? 'c' : ' ' )
      << '|'
    << ( obj.to_delete()  ? 'D' : ' ' )
    << ( obj.to_install() ? 'I' : ' ' )
      << '|'
    << ( obj.by_user() ? 'U' : ' ' )
      << '|'
    << ( obj.is_taboo() ? 'T' : ' ' )
      << ']';
  return str;
}

