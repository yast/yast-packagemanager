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

   File:       InstSrcManager.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/InstSrcManager.h>

using namespace std;

///////////////////////////////////////////////////////////////////

InstSrcManager * InstSrcManager::_ISM = 0;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::ISM
//	METHOD TYPE : InstSrcManager &
//
//	DESCRIPTION :
//
InstSrcManager & InstSrcManager::ISM()
{
  if ( !_ISM )
    _ISM = new InstSrcManager;
  return *_ISM;
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::InstSrcManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstSrcManager::InstSrcManager()
{
  MIL << "Launch ISM..." << endl;
  MIL << "Created ISM" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::~InstSrcManager
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcManager::~InstSrcManager()
{
  MIL << "Shtudown ISM..." << endl;
  MIL << "Deleted ISM" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::scanMedia
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::scanMedia( constInstSrcPtr & isrc_r,
				   MediaInfoPtr      media_r,
				   const ISrcType    type_r )
{
  MIL << "scanMedia (" << type_r << ") " << media_r << endl;
  DBG << "scanMedia " << isrc_r << endl;

  isrc_r = 0;

  if ( !media_r )
    return E_NO_MEDIA;

  return E_Error;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const InstSrcManager & obj )
{
  return str;
}


