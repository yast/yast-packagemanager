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

  File:       InstSrcError.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Definition of "InstSrc" error values.

/-*/

#ifndef InstSrcError_h
#define InstSrcError_h

#include <iosfwd>

#include <y2pm/PMError.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcError
/**
 * @short Definition of "InstSrc" error values.
 * @see PMError
 **/
class InstSrcError {

  private:

    friend class PMError;

    static const std::string errclass;

    static std::string errtext( const unsigned e );

  public:

    enum Error {
      E_ok    = PMError::E_ok,          	// no error
      E_error = PMError::C_InstSrcError,	// some error
      // more specific errors start here:
      E_cache_dir_exists,
      E_cache_dir_create,
      E_bad_cache_dir,
      E_bad_cache_descr,
      //
      E_bad_url,
      E_no_media
    };
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcError_h
