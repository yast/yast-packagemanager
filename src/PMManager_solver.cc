/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       PMManager_solver.cc
   Purpose:    Solver part of PMManager
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>

#include <y2pm/PMManager.h>
#include <y2pm/PkgSet.h>

using namespace std;

bool PMManager::solveInstall(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad)
{
    PkgSet installed; // already installed
    PkgSet available; // available for installation
    PkgSet toinstall; // user selected for installion

    unsigned ni = 0, na = 0, nt = 0;

    for ( PMSelectableVec::iterator it = begin(); it != end(); ++it ) {
	if((*it)->has_installed())
	{
	    PMSolvablePtr sp = (*it)->installedObj();
	    if(sp != NULL)
	    {
		installed.add(sp);
		ni++;
	    }
	    else
		ERR << "oops, got NULL despite Selectable said it has installed obj" << endl;
	}
	if((*it)->has_candidate())
	{
	    PMSolvablePtr sp = (*it)->candidateObj();
	    if(sp != NULL)
	    {
		available.add(sp);
		na++;

		if((*it)->to_install())
		{
		    toinstall.add(sp);
		    nt++;
		}
	    }
	    else
		ERR << "oops, got NULL despite Selectable said it has candidateObj obj" << endl;
	}
    }

    MIL << stringutil::form("%d installed, %d available, %d to install", ni, na, nt) << endl;

    PkgDep engine( installed, available ); // TODO alternative_default
//    engine.set_unresolvable_callback(unresolvable_callback); //TODO

    bool success = engine.install( toinstall, good, bad);

    return success;
}
