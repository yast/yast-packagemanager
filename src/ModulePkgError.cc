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

  File:       ModulePkgError.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Hook for "PkgModule" error values.

/-*/

#include <iostream>

#include <y2pm/ModulePkgError.h>

using namespace std;

///////////////////////////////////////////////////////////////////
#ifndef N_
#  define N_(STR) STR
#endif
///////////////////////////////////////////////////////////////////

const std::string ModulePkgError::errclass( "ModulePkg" );

ModulePkgError::errtextFnc ModulePkgError::errtextfnc = 0;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : ModulePkgError::errtext
//	METHOD TYPE : std::string
//
//	DESCRIPTION : Return textual description or numerical value
//      as string.
//
std::string ModulePkgError::errtext( const unsigned e )
{
  if ( errtextfnc ) {
    return errtextfnc( e );
  }

  return stringutil::numstring( e );
}
