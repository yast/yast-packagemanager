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

   File:       MediaInfo.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2pm/MediaInfo.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaInfo
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(MediaInfo);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaInfo::MediaInfo
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaInfo::MediaInfo()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaInfo::~MediaInfo
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaInfo::~MediaInfo()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaInfo::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & MediaInfo::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

