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

   File:       PMManager.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>
#include <y2util/Y2SLog.h>

#include <y2pm/PMManager.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::PMManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMManager::PMManager()
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMManager::~PMManager
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMManager::~PMManager()
{

}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PMManager & obj )
{
  return str;
}

