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

  File:       Y2PM.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Core class providing access to all components of the
  Package Management creating them on demand.

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>

using namespace std;

///////////////////////////////////////////////////////////////////
#undef Y2LOG
#define Y2LOG "Y2PM"
///////////////////////////////////////////////////////////////////

//InstTarget * Y2PM::_instTarget = 0;

//InstSrcManager * Y2PM::_instSrcManager = 0;

PMPackageManager *   Y2PM::_packageManager = 0;

PMSelectionManager * Y2PM::_selectionManager = 0;

PMYouPatchManager *  Y2PM::_youPatchManager = 0;

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::instTarget
//	METHOD TYPE : InstTarget &
//
//	DESCRIPTION :
//
#if 0
InstTarget & Y2PM::instTarget()
{
  if ( !_instTarget ) {
    MIL << "Launch InstTarget..." << endl;
    _instTarget = new InstTarget;
    MIL << "Created InstTarget" << endl;
  }
  return *_instTarget;
}
#endif

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::instSrcManager
//	METHOD TYPE : InstSrcManager &
//
//	DESCRIPTION :
//
#if 0
InstSrcManager & Y2PM::instSrcManager()
{
  if ( !_instSrcManager ) {
    MIL << "Launch InstSrcManager..." << endl;
    _instSrcManager = new InstSrcManager;
    MIL << "Created InstSrcManager" << endl;
  }
  return *_instSrcManager;
}
#endif

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::packageManager
//	METHOD TYPE : PMPackageManager &
//
//	DESCRIPTION :
//
PMPackageManager & Y2PM::packageManager()
{
  if ( !_packageManager ) {
    MIL << "Launch PackageManager..." << endl;
    _packageManager = new PMPackageManager;
    MIL << "Created PackageManager" << endl;

    WAR << "Fake InstTarget and load installed Packages..." << endl;
    RpmDbPtr rpmdb( new RpmDb("/") );
    list<PMPackagePtr> plist;
    RpmDb::DbStatus dbstat = rpmdb->initDatabase(true);
    if( ! (dbstat == RpmDb::RPMDB_OK || dbstat == RpmDb::RPMDB_NEW_CREATED)) {
      ERR << "error initializing rpmdb: " << dbstat << endl;
    } else {
      dbstat = rpmdb->getPackages(plist);
      Y2PM::packageManager().poolSetInstalled( plist );
    }
  }
  return *_packageManager;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::selectionManager
//	METHOD TYPE : PMSelectionManager &
//
//	DESCRIPTION :
//
PMSelectionManager & Y2PM::selectionManager()
{
  if ( !_selectionManager ) {
    MIL << "Launch SelectionManager..." << endl;
    _selectionManager = new PMSelectionManager;
    MIL << "Created SelectionManager" << endl;
  }
  return *_selectionManager;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::youPatchManager
//	METHOD TYPE : PMYouPatchManager &
//
//	DESCRIPTION :
//
PMYouPatchManager & Y2PM::youPatchManager()
{
  if ( !_youPatchManager ) {
    MIL << "Launch YouPatchManager..." << endl;
    _youPatchManager = new PMYouPatchManager;
    MIL << "Created YouPatchManager" << endl;
  }
  return *_youPatchManager;
}

