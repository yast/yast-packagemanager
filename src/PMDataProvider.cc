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

   File:       PMDataProvider.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <y2pm/PMDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMDataProvider);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMDataProvider::PMDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMDataProvider::PMDataProvider()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMDataProvider::~PMDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMDataProvider::~PMDataProvider()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMDataProvider::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMDataProvider::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}


