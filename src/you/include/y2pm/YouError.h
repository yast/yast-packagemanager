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

  File:       YouError.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Definition of "You" error values.

/-*/
#ifndef YouError_h
#define YouError_h

#include <iosfwd>

#include <y2pm/PMError.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : YouError
/**
 * @short Definition of "You" error values.
 * @see PMError
 **/
class YouError {

  private:

    friend class PMError;

    static const std::string errclass;

    static std::string errtext( const unsigned e );

  public:

    enum Error {
      E_ok    = PMError::E_ok,        	// no error
      E_error = PMError::C_YouError,	// some error
      // more specific errors start here:
      E_bad_sig_file,
      E_bad_sig_rpm
    };
};

///////////////////////////////////////////////////////////////////

#endif // YouError_h

