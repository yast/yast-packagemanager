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

  File:       MediaError.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Definition of "Media" error values.

/-*/

#include <iostream>

#include <y2pm/MediaError.h>

using namespace std;

///////////////////////////////////////////////////////////////////
#ifndef N_
#  define N_(STR) STR
#endif
///////////////////////////////////////////////////////////////////

const std::string MediaError::errclass( "Media" );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaError::errtext
//	METHOD TYPE : std::string
//
//	DESCRIPTION : Return textual description or numerical value
//      as string.
//
std::string MediaError::errtext( const unsigned e )
{
  switch ( (Error)e ) {

  case E_ok:	return PMError::OKstring;
  case E_error:	return PMError::ERRORstring;
  ///////////////////////////////////////////////////////////////////
  // Error messages for media access in package manager
  //
  // more specific errors start here:
  // case E_some_err:	return N_("some text");
  ///////////////////////////////////////////////////////////////////
  case E_system:		return N_("Unkown error in subsystem");
  case E_not_a_directory:	return N_("not a directory");
  case E_no_access:		return N_("access denied");
  case E_no_permission:		return N_("insufficient permissions");
  case E_no_space:		return N_("no space left");
  case E_bad_url:		return N_("invalid URL");
  case E_bad_media_type:	return N_("invalid media type");
  case E_bad_filename:		return N_("invalid filename");
  case E_already_attached:	return N_("already attached");
  case E_not_attached:		return N_("not attached");
  case E_bad_attachpoint:	return N_("bad attach point");
  case E_not_open:		return N_("not opened");
  case E_no_destination:	return N_("no destination");
  case E_no_source:		return N_("no source");
  case E_invalid_device:	return N_("invalid device");
  case E_invalid_filesystem:	return N_("invalid filesystem");
  case E_no_host_specified:	return N_("no hostname specified");
  // wget errors
  case E_file_not_found:	return N_("file not found");
  case E_login_failed:		return N_("login failed");
  case E_connection_failed:	return N_("connection failed");
  case E_proxyauth_failed:	return N_("proxy authentication failed");
  //mount
  case E_mount_failed:		return N_("mount failed");
  case E_already_mounted:	return N_("source already mounted");
  case E_busy:			return N_("ressource busy");

  case E_attachpoint_fixed:	return N_("attach point can not be changed");
  case E_not_supported_by_media:return N_("operation is not supported by media");

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
