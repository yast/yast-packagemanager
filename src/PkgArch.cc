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

   File:       PkgArch.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2pm/PkgArch.h>

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////

UstringHash PkgArch::_nameHash;

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgArch::compare
//	METHOD TYPE : int
//
//	DESCRIPTION :
//
int PkgArch::compare( const PkgArch & lhs, const PkgArch & rhs )
{
  if ( lhs == rhs )
    return 0;
  const std::list<PkgArch> & allowed( Y2PM::allowedArchs() );
  for ( std::list<PkgArch>::const_iterator it = allowed.begin(); it != allowed.end(); ++it ) {
    if ( *it == lhs )
      return -1;
    if ( *it == rhs )
      return 1;
  }
  return 0; // both not allowed
}

