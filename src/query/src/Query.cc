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

   File:       Query.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/Query.h>
#include <y2pm/PMSelectable.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Query
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER( Query );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Query::Query
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
Query::Query()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Query::~Query
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
Query::~Query()
{
}
