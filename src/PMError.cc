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

   File:       PMError.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2pm/PMError.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMError::dumpOn
//	METHOD TYPE : std::ostream &
//
//	DESCRIPTION :
//
std::ostream & PMError::dumpOn( std::ostream & str ) const
{
  if ( !_errval )
    return str << "E_OK";

  switch ( _errval / _range * _range) {
#define ENUMOUT(X) case X: str << #X << '('; break
    ENUMOUT( E_INST_SRC_MGR );
    ENUMOUT( E_INST_SRC );
#undef ENUMOUT
  default:
    str << "E_(";
    break;
  }
  return str << _errval << ')';
}

