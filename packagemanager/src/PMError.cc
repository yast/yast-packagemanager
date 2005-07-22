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

  File:       PMError.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Transport class for error values

  Textdomain "packagemanager"

/-*/

#include <iostream>

#include <y2pm/PMError.h>

#include <y2pm/InstSrcError.h>
#include <y2pm/MediaError.h>
#include <y2pm/InstTargetError.h>
#include <y2pm/YouError.h>
#include <y2pm/ModulePkgError.h>
#include <y2pm/PMLocale.h>

using namespace std;

///////////////////////////////////////////////////////////////////

const std::string PMError::errstrPrefix( _("ERROR") ); // "ERROR(some text or number)"

const unsigned    PMError::E_ok;    // initialized in header!
const unsigned    PMError::E_error; // initialized in header!
const std::string PMError::OKstring   ( _("OK") );
const std::string PMError::ERRORstring( _("error") );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMError::errClass
//	METHOD TYPE : PMError::ErrClass
//
//	DESCRIPTION : Return the error class coded in error value.
//
PMError::ErrClass PMError::errClass( const unsigned e )
{
  switch ( (ErrClass)( e & _repmask) ) {

#define ENUM_OUT(V) case C_##V: return C_##V; break

  ENUM_OUT( InstSrcError );
  ENUM_OUT( MediaError );
  ENUM_OUT( InstTargetError );
  ENUM_OUT( YouError );
  ENUM_OUT( ModulePkgError );

#undef ENUM_OUT

  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  case C_Error:
    break;
  }

  return C_Error;
}
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMError::errstr
//	METHOD TYPE : std::string
//
//	DESCRIPTION : Call known error classes errtext(). Otherwise
//      build a generic string showing the numerical error value.
//
std::string PMError::errstr( const unsigned e )
{
  if ( ! e )
    return OKstring;

  switch ( errClass( e ) ) {

#define ENUM_OUT(V) case C_##V: return defaulterrstr( V::errclass, V::errtext( e ) ); break

    ENUM_OUT( InstSrcError );
    ENUM_OUT( MediaError );
    ENUM_OUT( InstTargetError );
    ENUM_OUT( YouError );
    ENUM_OUT( ModulePkgError );

#undef ENUM_OUT

  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  case C_Error:
    break;
  }

  return stringutil::form( "%s(%d)", errstrPrefix.c_str(), e );
}

//////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMError::defaulterrstr
//	METHOD TYPE : std::string
//
//	DESCRIPTION : Build a generic string showing error class and text.
//
std::string PMError::defaulterrstr( const std::string & cl, const std::string & txt )
{
  return stringutil::form( "%s(%s:%s)", errstrPrefix.c_str(), cl.c_str(), txt.c_str() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMError::asString
//	METHOD TYPE : string
//
string PMError::asString() const
{
  if ( details().empty() )
    return errstr();
  // has details:
  return stringutil::form( "%s[%s]", errstr().c_str(), details().c_str() );
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : std::ostream &
**
**	DESCRIPTION :
*/
std::ostream & operator<<( std::ostream & str, const PMError & obj )
{
  return str << obj.asString();
}

void PMError::addDetails( const std::string &txt )
{
  if ( !_errdetails.empty() ) _errdetails += "\n";
  _errdetails += txt;
}
