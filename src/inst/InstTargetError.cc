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
  case E_RpmDB_not_found:			return N_("RPM Database not found");
  case E_RpmDB_create_failed:			return N_("Creating RPM Database failed");
  case E_RpmDB_check_old_version_failed:	return N_("Checking for old version of rpm failed");
  case E_RpmDB_mkdir_failed:			return N_("mkdir failed");
  case E_RpmDB_initdb_failed:			return N_("Initializing the RPM Database failed");
  case E_RpmDB_copy_tmpdb_failed:		return N_("Copying temporary Database failed");
  case E_RpmDB_rebuilddb_failed:		return N_("Rebuilding RPM Database failed");
  case E_RpmDB_not_initialized:			return N_("RPM Database not initialized");
  case E_RpmDB_subprocess_failed:		return N_("Some subprocess failed");
  ///////////////////////////////////////////////////////////////////
  // In a hurry? Write:
  // ENUM_OUT( E_some_err );
  // untill you found a beautifull text describing it.
  ///////////////////////////////////////////////////////////////////
#define ENUM_OUT(V) case V: return #V

#undef ENUM_OUT
  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  };

  return stringutil::numstring( e );
}
