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

   File:       FAKEMediaInfo.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2pm/FAKEMediaInfo.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : FAKEMediaInfo
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(FAKEMediaInfo);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : FAKEMediaInfo::FAKEMediaInfo
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
FAKEMediaInfo::FAKEMediaInfo()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : FAKEMediaInfo::~FAKEMediaInfo
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
FAKEMediaInfo::~FAKEMediaInfo()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : FAKEMediaInfo::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & FAKEMediaInfo::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

