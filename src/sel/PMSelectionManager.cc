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

  File:       PMSelectionManager.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/PMSelectionManager.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::PMSelectionManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSelectionManager::PMSelectionManager()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::~PMSelectionManager
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMSelectionManager::~PMSelectionManager()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionManager::assertObjectType
//	METHOD TYPE : PMObjectPtr
//
//	DESCRIPTION :
//
PMObjectPtr PMSelectionManager::assertObjectType( const PMObjectPtr & object_r ) const
{
  PMSelectionPtr p( object_r );
  if ( object_r && !p ) {
    ERR << "Object is not a Selection: " << object_r << endl;
  }
  return p;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PMSelectionManager & obj )
{
  return str;
}

