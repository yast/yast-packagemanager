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

InstTarget *  Y2PM::_instTarget = 0;

string* Y2PM::_rootdir = 0;

Y2PM::CallBacks Y2PM::_callbacks = Y2PM::CallBacks::CallBacks();

Y2PM::CallBacks::CallBacks()
{
    _installation_package_start_func = NULL;
    _installation_package_start_data = NULL;
    _installation_package_progress_func = NULL;
    _installation_package_progress_data = NULL;
    _installation_package_done_func = NULL;
    _installation_package_done_data = NULL;
};

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
    list<PMPackagePtr> plist;
    PMError dbstat = Y2PM::instTarget().init();
    if( dbstat != InstTargetError::E_ok ) {
      ERR << "error initializing target: " << dbstat << endl;
    } else {
      dbstat = Y2PM::instTarget().getPackages(plist);
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

static void progresscallback(double p, void* nix)
{
    cout << p << endl;
}

/**
 * Access to InstTarget
 * */
InstTarget & Y2PM::instTarget()
{
    if(!_rootdir)
    {
	_rootdir = new string("/");
    }
    if(!_instTarget)
    {
	MIL << "Create InstTarget" << endl;
       _instTarget = new InstTarget(*_rootdir);
       _instTarget->setPackageInstallProgressCallback(progresscallback,NULL);

    }

    return *_instTarget;
}

/**
 * set path where root fs is mounted
 * */
bool Y2PM::setRoot(const std::string& r)
{
    if( _rootdir)
    {
	if( _instTarget )
	{
	    ERR << "root dir already set" << endl;
	    return false;
	}
	else
	{
	    delete _rootdir;
	    _rootdir = NULL;
	}
    }

    _rootdir = new string(r);

    return true;
}
