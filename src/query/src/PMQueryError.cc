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

  File:       PMQueryError.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Definition of "InstSrc" error values.

/-*/

#include <iostream>

#include <y2pm/PMQueryError.h>

using namespace std;

///////////////////////////////////////////////////////////////////
#ifndef N_
#  define N_(STR) STR
#endif
///////////////////////////////////////////////////////////////////

const std::string PMQueryError::errclass( "Query" );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMQueryError::errtext
//	METHOD TYPE : std::string
//
//	DESCRIPTION : Return textual description or numerical value
//      as string.
//
std::string PMQueryError::errtext( const unsigned e )
{
    switch ( (Error)e ) {

	case E_ok:	return PMError::OKstring;
	case E_error:	return PMError::ERRORstring;

  ///////////////////////////////////////////////////////////////////
  // more specific errors start here:
  // case E_some_err:	return N_("some text");
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // In a hurry? Write:
  // ENUM_OUT( E_some_err );
  // untill you found a beautifull text describing it.
  ///////////////////////////////////////////////////////////////////
#define ENUM_OUT(V) case V: return #V

	ENUM_OUT(E_memory);			// out of memory
	ENUM_OUT(E_unterminated_string);
	ENUM_OUT(E_open_paranthesis);		// no matching ')' found
	ENUM_OUT(E_compare_flag);			// compare op with flag operand
	ENUM_OUT(E_andor_const);			// boolean op with const operand
	ENUM_OUT(E_bad_expr);			// node is value not bool
	ENUM_OUT(E_unknown_operation);
	ENUM_OUT(E_unknown_operand);
	ENUM_OUT(E_no_op);			//
	ENUM_OUT(E_bad_value);			// compare keyword used as value
	ENUM_OUT(E_syntax);

#undef ENUM_OUT
  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  };

  return stringutil::numstring( e );
}
