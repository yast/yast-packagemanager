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

   File:       InstSrcDescr.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2pm/InstSrcDescr.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDescr
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(InstSrcDescr);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::InstSrcDescr
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstSrcDescr::InstSrcDescr()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::~InstSrcDescr
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcDescr::~InstSrcDescr()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrcDescr::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

