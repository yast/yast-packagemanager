/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

  File:       InstTargetError.cc

  Author:     Ludwig Nussel <lnussel@suse.de>
  Maintainer: Ludwig Nussel <lnussel@suse.de>

  Purpose: Definition of "InstTarget" error values.

/-*/

#include <iostream>

#include <y2pm/InstTargetError.h>

using namespace std;

///////////////////////////////////////////////////////////////////
#ifndef N_
#  define N_(STR) STR
#endif
///////////////////////////////////////////////////////////////////

const std::string InstTargetError::errclass( "InstTarget" );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTargetError::errtext
//	METHOD TYPE : std::string
//
//	DESCRIPTION : Return textual description or numerical value
//      as string.
//
std::string InstTargetError::errtext( const unsigned e )
{
  switch ( (Error)e ) {

  case E_ok:	return PMError::OKstring;
  case E_error:	return PMError::ERRORstring;
  ///////////////////////////////////////////////////////////////////
  // more specific errors start here:
  // case E_some_err:	return N_("some text");
  ///////////////////////////////////////////////////////////////////
  case E_system:				return N_("Unkown error in subsystem");
  case E_invalid_argument:			return N_("Invalid argument");
  ///////////////////////////////////////////////////////////////////
  // In a hurry? Write:
  // ENUM_OUT( E_some_err );
  // untill you found a beautifull text describing it.
  ///////////////////////////////////////////////////////////////////
#define ENUM_OUT(V) case V: return #V

  ENUM_OUT( E_RpmDB_global_init_failed );
  ENUM_OUT( E_RpmDB_already_open );
  ENUM_OUT( E_RpmDB_init_failed );
  ENUM_OUT( E_RpmDB_open_failed );
  ENUM_OUT( E_RpmDB_convert_failed );
  ENUM_OUT( E_RpmDB_rebuild_failed );
  ENUM_OUT( E_RpmDB_access_blocked );
  ENUM_OUT( E_RpmDB_not_open );

  ENUM_OUT( E_RpmDB_subprocess_failed );

  ENUM_OUT( E_SelDB_already_open );
  ENUM_OUT( E_SelDB_open_failed );
  ENUM_OUT( E_SelDB_not_open );
  ENUM_OUT( E_SelDB_file_not_found );
  ENUM_OUT( E_SelDB_file_bad_magic );
  ENUM_OUT( E_SelDB_install_failed );
  ENUM_OUT( E_SelDB_not_installed );

  ENUM_OUT( E_ProdDB_already_open );
  ENUM_OUT( E_ProdDB_open_failed );
  ENUM_OUT( E_ProdDB_not_open );
  ENUM_OUT( E_ProdDB_file_bad_magic );
  ENUM_OUT( E_ProdDB_install_failed );
  ENUM_OUT( E_ProdDB_not_installed );

  ENUM_OUT( E_UpdateInf_not_found );
  ENUM_OUT( E_UpdateInf_malformed );

#undef ENUM_OUT

  case E_user_abort: return N_("User aborted operation.");

  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  };

  return stringutil::numstring( e );
}
