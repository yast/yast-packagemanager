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

   File:       PMQuery.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMQuery.h>
#include <y2pm/PMSelectable.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMQuery
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER( PMQuery );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMQuery::PMQuery
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMQuery::PMQuery()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMQuery::~PMQuery
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMQuery::~PMQuery()
{
}
