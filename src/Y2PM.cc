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

///////////////////////////////////////////////////////////////////
// global settings
///////////////////////////////////////////////////////////////////
#warning MUST INIT GLOBAL SETTINGS

Pathname Y2PM::_instTarget_rootdir( "/" );
Pathname Y2PM::_system_rootdir    ( "/" );
LangCode Y2PM::_preferred_locale ("en");
std::list<LangCode> Y2PM::_requested_locales;
PkgArch Y2PM::_base_arch;
std::list<PkgArch> Y2PM::_allowed_archs;

///////////////////////////////////////////////////////////////////
// components provided
///////////////////////////////////////////////////////////////////

InstTarget * Y2PM::_instTarget = 0;

InstSrcManager * Y2PM::_instSrcManager = 0;

PMPackageManager *   Y2PM::_packageManager = 0;

PMSelectionManager * Y2PM::_selectionManager = 0;

PMYouPatchManager *  Y2PM::_youPatchManager = 0;

///////////////////////////////////////////////////////////////////
// CallBacks
///////////////////////////////////////////////////////////////////

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


PkgArch
Y2PM::baseArch(void)
{
#warning TBD init _base_arch from product
    if (((const std::string &)_base_arch).empty())
    {
	_base_arch = instTarget().baseArch();
    }

    return _base_arch;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::instTarget
//	METHOD TYPE : InstTarget &
//
//	DESCRIPTION :
//
InstTarget & Y2PM::instTarget(bool do_start, Pathname root)
{
    if ( !_instTarget )
    {
	MIL << "Launch InstTarget... ()" << endl;
	_instTarget = new InstTarget ();
	MIL << "Created InstTarget" << endl;
    }

    if (do_start)
    {
	WAR << "Fake InstTarget and load installed Packages..." << endl;
	_instTarget_rootdir = root;
	PMError dbstat = Y2PM::instTarget().init(_instTarget_rootdir, false);
	if( dbstat != InstTargetError::E_ok )
	{
	    ERR << "error initializing target: " << dbstat << endl;
	}
	else
	{
	    // this will start the package manager
	    Y2PM::packageManager().poolSetInstalled( Y2PM::instTarget().getPackages () );
	}
    }

    return *_instTarget;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::instSrcManager
//	METHOD TYPE : InstSrcManager &
//
//	DESCRIPTION :
//
InstSrcManager & Y2PM::instSrcManager()
{
  if ( !_instSrcManager ) {
    MIL << "Launch InstSrcManager..." << endl;
    _instSrcManager = new InstSrcManager ();
    MIL << "Created InstSrcManager @" << _instSrcManager << endl;
  }
  return *_instSrcManager;
}

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
  if ( !_packageManager )
  {
    MIL << "Launch PackageManager..." << endl;
    _packageManager = new PMPackageManager;
    MIL << "Created PackageManager @" << _packageManager << endl;

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
    MIL << "Created SelectionManager @" << _selectionManager << endl;
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
    _youPatchManager->poolSetInstalled( Y2PM::instTarget().getPatches () );
    MIL << "Created YouPatchManager @" << _youPatchManager << endl;
  }
  return *_youPatchManager;
}



///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::commitPackages
//	METHOD TYPE : int
//
//	DESCRIPTION : commit all changes in the package manager
//		==> actually delete/install packages
//		relies on callbacks for media change
//		!! DOES NOT SOLVE !!
//
//	if media_nr is != 0, only packages from this media are
//	installed. media_nr==0 means install all packages from all media.
//
//	returns failed packages in 'errors'
//	returns uninstalled packages (because media not available) in 'remaining'
//
bool
Y2PM::commitPackages (unsigned int media_nr, std::list<std::string>& errors, std::list<std::string>& remaining)
{
    bool ret = true;

    std::list<PMPackagePtr> dellist;
    std::list<PMPackagePtr> inslist;

    packageManager().getPackagesToInsDel (dellist, inslist);	// compute order

    instTarget().removePackages (dellist);			// delete first


    for (std::list<PMPackagePtr>::iterator it = inslist.begin();
	 it != inslist.end(); ++it)
    {
	unsigned int pkgmedianr = (*it)->medianr();
	if ((media_nr > 0)
	    && (pkgmedianr != media_nr))
	{
#warning loosing version information
	    remaining.push_back ((*it)->name());
	    continue;
	}
#warning commitPackages NEEDS REVIEW
#if 0
	if ((*it)->source() == 0)
	{
	    ERR << "No source for " << *it << endl;
	    remaining.push_back ((*it)->name());
	    ret = false;
	    continue;
	}
	// determine directory and rpm name
	Pathname name = (*it)->location();
	Pathname dir;
	string::size_type dirpos = name.asString().find_first_of (" ");
	if (dirpos == string::npos)
	{
	    // directory == architecture
	    dir = Pathname ((const std::string &)((*it)->arch()));
	}
	else
	{
	    // directory in location
	    dir = Pathname (name.asString().substr (dirpos+1));
	    name = Pathname (name.asString().substr (0, dirpos));
	}
	Pathname path = (*it)->source()->providePackage (pkgmedianr, name, dir);
	if (path.asString().empty())
	{
	    ERR << "Media can't provide " << pkgmedianr << ":" << dir << "/" << name << endl;
	    remaining.push_back ((*it)->name());
	    ret = false;
	    continue;
	}
#else
	Pathname path = (*it)->providePkgToInstall();
	if (path.empty())
	{
	    ERR << "Media can't provide package to install for " << (*it) << endl;
	    remaining.push_back ((*it)->name());
	    ret = false;
	    continue;
	}
#endif
	PMError err = instTarget().installPackage (path);
	if (err)
	{
	    errors.push_back ((*it)->name());
	}

    } // loop over inslist

    return ret;
}


