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

  File:       QueryError.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Definition of "Query" error values.

/-*/

#ifndef QueryError_h
#define QueryError_h

#include <iosfwd>

#include <y2pm/PMError.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : QueryError
/**
 * @short Definition of "InstSrc" error values.
 * @see PMError
 **/
class QueryError {

  private:

    friend class PMError;

    static const std::string errclass;

    static std::string errtext( const unsigned e );

  public:

    enum Error {
	E_ok    = PMError::E_ok,          	// no error
	E_error = PMError::C_QueryError,	// some error
	E_memory,			// out of memory
	E_unterminated_string,
	E_open_paranthesis,		// no matching ')' found
	E_compare_flag,			// compare op with flag operand
	E_andor_const,			// boolean op with const operand
	E_bad_expr,			// node is value not bool
	E_unknown_operation,
	E_unknown_operand,
	E_no_op,			//
	E_bad_value,			// compare keyword used as value
	E_syntax
    };
};

///////////////////////////////////////////////////////////////////

#endif // QueryError_h
