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

  File:       InstSrcError.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Definition of "InstSrc" error values.

/-*/

#include <iostream>

#include <y2pm/InstSrcError.h>

using namespace std;

///////////////////////////////////////////////////////////////////
#ifndef N_
#  define N_(STR) STR
#endif
///////////////////////////////////////////////////////////////////

const std::string InstSrcError::errclass( "InstSrc" );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcError::errtext
//	METHOD TYPE : std::string
//
//	DESCRIPTION : Return textual description or numerical value
//      as string.
//
std::string InstSrcError::errtext( const unsigned e )
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

    case E_TBD: return "not yet implemented";

    ENUM_OUT( E_cache_dir_exists );
    ENUM_OUT( E_cache_dir_create );
    ENUM_OUT( E_bad_cache_dir );
    ENUM_OUT( E_bad_cache_descr );
    ENUM_OUT( E_open_file );
    ENUM_OUT( E_create_file );
    ENUM_OUT( E_write_file );
    //
    ENUM_OUT( E_bad_url );
    ENUM_OUT( E_no_instsrc_on_media );
    ENUM_OUT( E_no_instsrcdata_on_media );
    ENUM_OUT( E_src_no_description );
    ENUM_OUT( E_src_already_enabled );
    ENUM_OUT( E_src_not_enabled );
    ENUM_OUT( E_src_cache_disabled );
    //
    ENUM_OUT( E_bad_id );
    ENUM_OUT( E_isrc_cache_invalid );
    ENUM_OUT( E_isrc_cache_duplicate );

#undef ENUM_OUT
  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  };

  return stringutil::numstring( e );
}
