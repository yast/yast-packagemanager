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

  File:       YouError.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Definition of "You" error values.

  Textdomain "packagemanager"

/-*/

#include <iostream>

#include <y2pm/YouError.h>
#include <y2pm/PMLocale.h>

using namespace std;


const std::string YouError::errclass( "You" );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : YouError::errtext
//	METHOD TYPE : std::string
//
//	DESCRIPTION : Return textual description or numerical value
//      as string.
//
std::string YouError::errtext( const unsigned e )
{
  switch ( (Error)e ) {

  case E_ok:	return PMError::OKstring;
  case E_error:	return PMError::ERRORstring;
  ///////////////////////////////////////////////////////////////////
  // Error messages for YOU in package manager
  //
  // more specific errors start here:
  // case E_some_err:	return N_("some text");
  ///////////////////////////////////////////////////////////////////
  case E_bad_sig_file:  return _("File has invalid signature.");
  case E_bad_sig_rpm:   return _("RPM has invalid signature.");
  case E_get_youservers_failed: return _("Error getting youservers file.");
  case E_write_youservers_failed: return _("Error writing youservers file.");
  case E_read_youservers_failed: return _("Error reading youservers file.");
  case E_user_abort: return _("User aborted operation.");
  case E_user_skip: return _("User selected skip.");
  case E_user_skip_all: return _("User selected skip all.");
  case E_user_retry: return _("User selected retry.");
  case E_parse_error: return _("Error parsing patch description.");
  case E_script_failed: return _("Script failed.");
  case E_prescript_failed: return _("Prescript failed.");
  case E_postscript_failed: return _("Postscript failed.");
  case E_rpm_failed: return _("rpm failed.");
  case E_install_failed: return _("Installation failed.");
  case E_empty_location: return _("Package location is empty.");
  case E_read_mediapatches_failed: return _("Error getting media.1/patches file.");
  case E_callback_missing: return _("No callback function is set.");
  case E_missing_sig_file: return _("Missing signature.");
  case E_wrong_media: return _("Wrong media.");
  case E_md5sum_mismatch: return _("MD5 checksum does not match.");
  case E_mkdir_failed: return _("Could not create directory.");
  case E_reassemble_rpm_from_delta_failed: return _("Could not reassemble RPM from delta.");
  };

  return stringutil::numstring( e );
}
