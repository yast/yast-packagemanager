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

   File:       InstSrc.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2pm/InstSrc.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrc
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(InstSrc);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::InstSrc
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstSrc::InstSrc( MediaInfoPtr media_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::~InstSrc
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrc::~InstSrc()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrc::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

