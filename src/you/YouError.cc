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

/-*/

#include <iostream>

#include <y2pm/YouError.h>

using namespace std;

///////////////////////////////////////////////////////////////////
#ifndef N_
#  define N_(STR) STR
#endif
///////////////////////////////////////////////////////////////////

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
  case E_bad_sig_file:  return N_("File has invalid signature.");
  case E_bad_sig_rpm:   return N_("RPM has invalid signature.");
  case E_get_youservers_failed: return N_("Error getting youservers file.");
  case E_write_youservers_failed: return N_("Error writing youservers file.");
  case E_read_youservers_failed: return N_("Error reading youservers file.");
  case E_user_abort: return N_("User aborted operation.");
  case E_parse_error: return N_("Error parsing patch description.");
  case E_script_failed: return N_("Script failed.");
  case E_prescript_failed: return N_("Prescript failed.");
  case E_postscript_failed: return N_("Postscript failed.");
  case E_rpm_failed: return N_("rpm failed.");
  case E_install_failed: return N_("Installation failed.");
  case E_empty_location: return N_("Package location is empty.");
  case E_read_mediapatches_failed: return N_("Error getting media.1/patches file.");
  case E_callback_missing: return N_("No callback function is set.");
  };

  return stringutil::numstring( e );
}
