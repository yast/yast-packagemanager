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


void PMManager::buildSets(PkgSet& installed, PkgSet& available, PkgSet& toinstall)
{
    unsigned ni = 0, na = 0, nt = 0;

    for ( PMManager::PMSelectableVec::iterator it = this->begin(); it != this->end(); ++it ) {
	// remove auto state
	if((*it)->by_auto() )
	{
	    (*it)->auto_unset();
	}
	if ( (*it)->to_delete() )
	  continue;
	// installed into installed set
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
	// candidates to available, and those with marked for install also to toinstall
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
}

// set packages not from input list to auto
static void setAutoState(PkgDep::ResultList good)
{
    for(PkgDep::ResultList::iterator it = good.begin();
	    it != good.end(); ++it)
    {
	if(it->solvable == NULL)
	    { ERR << "good result with NULL solvable: " << it->name << endl; continue; }

	if(it->from_input_list)
	    continue;

	PMObjectPtr op = it->solvable;

	if( op == NULL )
	    { ERR << it->name << "is no PMObject" << endl; continue; }

	PMSelectablePtr selp = op->getSelectable();

	if(selp == NULL)
	    { ERR << "good result with NULL selectable: " << it->name << endl; continue; }

	if(!selp->auto_set_install())
	    { ERR << "could not set " << it->name << " to status auto" << endl; continue; }
    }
}

bool PMManager::solveInstall(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad)
{
    PkgSet installed; // already installed
    PkgSet available; // available for installation
    PkgSet toinstall; // user selected for installion

    buildSets(installed, available, toinstall);

    PkgDep engine( installed, available ); // TODO alternative_default
//    engine.set_unresolvable_callback(unresolvable_callback); //TODO

    bool success = engine.install( toinstall, good, bad);

    setAutoState(good);

    return success;
}

bool PMManager::solveUpgrade(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad, PkgDep::SolvableList to_remove)
{
    PkgSet installed; // already installed
    PkgSet available; // available for installation
    PkgSet toinstall; // user selected for installion

    buildSets(installed, available, toinstall);

    PkgDep engine( installed, available ); // TODO alternative_default
//    engine.set_unresolvable_callback(unresolvable_callback); //TODO

    bool success = engine.upgrade( toinstall, good, bad, to_remove);

    setAutoState(good);

    return success;
}
