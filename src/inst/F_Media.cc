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

  File:       F_Media.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2pm/F_Media.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : F_Media::F_Media
//	METHOD TYPE : Constructor
//
F_Media::F_Media()
{

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : F_Media::~F_Media
//	METHOD TYPE : Destructor
//
F_Media::~F_Media()
{

}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const F_Media & obj )
{
  return str;
}

