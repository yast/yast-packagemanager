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

   File:       PMPackageManager.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/PMPackageManager.h>

using namespace std;

///////////////////////////////////////////////////////////////////

PMPackageManager * PMPackageManager::_PM = 0;


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::PM
//	METHOD TYPE : PMPackageManager &
//
//	DESCRIPTION :
//
PMPackageManager & PMPackageManager::PM()
{
  if ( !_PM )
    _PM = new PMPackageManager;
  return *_PM;
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::PMPackageManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMPackageManager::PMPackageManager()
{
  MIL << "Launch PM..." << endl;
  MIL << "Created PM" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::~PMPackageManager
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMPackageManager::~PMPackageManager()
{
  MIL << "Shtudown PM..." << endl;
  MIL << "Deleted PM" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageManager::addPackages
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMPackageManager::addPackages( const PackageList & pkglist_r )
{
  MIL << "Going to add " << pkglist_r.size() << " packages ..." << endl;

  for ( PackageList_const_iterator i = pkglist_r.begin(); i != pkglist_r.end(); ++i ) {
    DBG << "  " << *i << endl;
  }

  MIL << "DONE" << endl;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const PMPackageManager & obj )
{
  return str;
}

