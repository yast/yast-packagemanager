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

/*
#warning file dependencies
static PkgDep::WhatToDoWithUnresolvable unresolvable_callback(
    PkgDep* solver, const PkgRelation& rel, PMSolvablePtr& p)
{

    if(rel.name()->find("rpmlib(") != std::string::npos)
	return PkgDep::UNRES_IGNORE;

    if(rel.name() == "/bin/bash" || rel.name() == "/bin/sh")
    {
	const PkgSet set = solver->current_installed();
	p = set.lookup(PkgName("bash"));
	if(p==NULL)
	{
	    const PkgSet set = solver->current_available();
	    p = set.lookup(PkgName("bash"));
	}
	return PkgDep::UNRES_TAKETHIS;
    }
    if(rel.name() == "/usr/bin/perl")
    {
	const PkgSet set = solver->current_installed();
	p = set.lookup(PkgName("perl"));
	if(p==NULL)
	{
	    const PkgSet set = solver->current_available();
	    p = set.lookup(PkgName("perl"));
	}
	return PkgDep::UNRES_TAKETHIS;
    }
    if(rel.name()->operator[](0)=='/')
    {
	DBG << "ignoring file requirement " << rel.name() << endl;
	return PkgDep::UNRES_IGNORE;
    }

    return PkgDep::UNRES_FAIL;
}
*/


void PMManager::buildSets(PkgSet& installed, PkgSet& available, PkgSet& toinstall)
{
    unsigned ni = 0, na = 0, nt = 0;

    for ( PMManager::PMSelectableVec::iterator it = this->begin(); it != this->end(); ++it ) {
	// remove auto state
	if((*it)->by_auto() )
	{
	    (*it)->auto_unset();
	}

/*
	if ( (*it)->to_delete() )
	{
	    DBG << "skip " << (*it)->name() << endl;
	    continue;
	}
*/
	// installed into installed set
	if((*it)->has_installed() && !(*it)->to_delete())
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
static void setAutoState(PkgDep::ResultList& good, PkgDep::SolvableList& to_remove)
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

	if(it->pertains == PkgDep::Result::Res_IsInstalled)
	{
	    if(selp->status() != PMSelectable::S_KeepInstalled)
	    {
		INT << "PkgDep said " << it->name << " is installed, but it isn't" << endl;
	    }
	    continue;
	}

	if(!selp->auto_set_install())
	    {
		ERR << "could not set " << it->name <<
		    " to status auto install, current state is " << selp->status() << endl;
		continue;
	    }
    }

    for(PkgDep::SolvableList::iterator it = to_remove.begin();
	    it != to_remove.end(); ++it)
    {
	PMObjectPtr op = *it;

	if( op == NULL )
	    { ERR << (*it)->name() << "is no PMObject" << endl; continue; }

	PMSelectablePtr selp = op->getSelectable();

	if(selp == NULL)
	    { ERR << "good result with NULL selectable: " << (*it)->name() << endl; continue; }

	if(!selp->auto_set_delete())
	    { ERR << "could not set " << (*it)->name() << " to status auto delete" << endl; continue; }
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

//    bool success = engine.install( toinstall, good, bad);
    bool success = engine.solvesystemnoauto( toinstall, good, bad);
    PkgDep::SolvableList to_remove;

    setAutoState(good, to_remove);

    return success;
}

bool PMManager::solveConsistent(PkgDep::ErrorResultList& bad)
{
    PkgSet installed; // already installed
    PkgSet available; // available for installation
    PkgSet toinstall; // user selected for installion

    buildSets(installed, available, toinstall);

    PkgDep engine( installed, available ); // TODO alternative_default
//    engine.set_unresolvable_callback(unresolvable_callback); //TODO

    bool success = engine.consistent(bad);

    return success;
}


bool PMManager::solveUpgrade(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad, PkgDep::SolvableList& to_remove)
{
    PkgSet installed; // already installed
    PkgSet available; // available for installation
    PkgSet toinstall; // user selected for installion

    buildSets(installed, available, toinstall);

    PkgDep engine( installed, available ); // TODO alternative_default
//    engine.set_unresolvable_callback(unresolvable_callback); //TODO

    bool success = engine.upgrade( toinstall, good, bad, to_remove, true);

    setAutoState(good, to_remove);

    return success;
}

void PMManager::setMaxRemoveThreshold(unsigned nr)
{
    PkgDep::set_default_max_remove(nr);
}

#if 0
bool PMManager::solveEverythingRight(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad, PkgDep::SolvableList &to_remove)
{
    PkgSet installed; // already installed
    PkgSet available; // available for installation
    PkgSet toinstall; // user selected for installion
    PkgSet toinstall2; // backup for reference

    unsigned ni = 0, na = 0, nt = 0;

    for ( PMManager::PMSelectableVec::iterator it = this->begin(); it != this->end(); ++it ) {
	// remove auto state
	if((*it)->by_auto() )
	{
	    (*it)->auto_unset();
	}

	// installed into installed set

	// installed package with no candidate available that is not to be deleted
	if((*it)->has_installed() && !(*it)->to_delete() && !(*it)->to_install())
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
	else if((*it)->has_candidate())
	{
	    PMSolvablePtr sp = (*it)->candidateObj();
	    if(sp != NULL)
	    {
		available.add(sp);
		na++;
		if((*it)->to_install())
		{
		    installed.add(sp);
		    ni++;
		}
	    }
	    else
		ERR << "oops, got NULL despite Selectable said it has candidateObj obj" << endl;
	}
    }

    MIL << stringutil::form("%d installed, %d available, %d to install", ni, na, nt) << endl;

//    buildSets(installed, available, toinstall);

    PkgDep engine( installed, available ); // TODO alternative_default
    engine.set_unresolvable_callback(unresolvable_callback); //TODO

//    engine.install_installed(true);
//    bool success = engine.upgrade( toinstall, good, bad, to_remove, false, true);
    bool success = engine.solvesystemnoauto( toinstall, good, bad);
//    bool success = engine.install( toinstall, good, bad);

    for (PkgSet::iterator it = toinstall.begin(); it != toinstall.end(); ++it)
    {
	if(toinstall2.includes(it->key))
	{
	    toinstall2.remove(it->key);
	}
	else
	{
//	    INT << it->key << " in toinstall where it doesn't belong" << endl;
	}
    }

    if(!toinstall2.empty())
    {
	DBG << "not installed packages: " << endl;
	for (PkgSet::iterator it = toinstall2.begin(); it != toinstall.end(); ++it)
	{
	    DBG << it->key << endl;
	}
    }

    setAutoState(good, to_remove);

    return success;
}
#endif
