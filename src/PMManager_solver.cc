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

#define KEEPSETS

#include <malloc.h>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>
#include <y2util/timeclass.h>

#include <y2pm/PMManager.h>
#include <y2pm/PkgSet.h>

using namespace std;

static inline void addpkgtosetifnotalreadyin(PkgSet& set, PMSolvablePtr newp)
{
	PMSolvablePtr oldp;
	if(!newp) return;
	oldp = set.lookup(newp->name());
	if(oldp == NULL) // new is not already in set
	{
		set.add(newp);
	}
	else if ( oldp != newp ) // a package with this name is in the set, but it's not the same
	{
		set.remove(oldp);
		set.add(newp);
	}
	// otherwise package is already in set
}

/**
 * Build a PkgSet that consists of installed packages and packages that are
 * marked for installation
 * */
static void buildinstalledonly(PMManager& manager, PkgSet& installed)
{
    for ( PMManager::PMSelectableVec::iterator it = manager.begin(); it != manager.end(); ++it )
    {
	bool hasi = (*it)->has_installed();
	bool toi  = (*it)->to_install();
	// if (package is installed and not marked for deletion) or marked for installation
	if(( hasi && !(*it)->to_delete()) || toi )
	{
	    PMSolvablePtr sp;
	    if(hasi)
		sp = (*it)->installedObj();
	    else
		sp = (*it)->candidateObj();

	    if(sp != NULL)
	    {
#ifndef KEEPSETS
		installed.add(sp);
#else
		addpkgtosetifnotalreadyin(installed,sp);
#endif
	    }
	    else
	    {
		ERR << "oops, got NULL despite Selectable said it has installed obj" << endl;
#ifdef KEEPSETS
		installed.remove((*it)->name());
#endif
	    }
	}
#ifdef KEEPSETS
	else
	{
	    installed.remove((*it)->name());
	}
#endif
    }
}

void PMManager::buildSets(PkgSet& installed, PkgSet& available, PkgSet& toinstall)
{
    unsigned ni = 0, na = 0, nt = 0;

    for ( PMManager::PMSelectableVec::iterator it = this->begin(); it != this->end(); ++it ) {
	// remove auto state
	if((*it)->by_auto() )
	{
	    (*it)->auto_unset();
	}

	// installed into installed set
	if((*it)->has_installed() && !(*it)->to_delete())
	{
	    PMSolvablePtr sp = (*it)->installedObj();
	    if(sp != NULL)
	    {
#ifndef KEEPSETS
		installed.add(sp);
#else
		addpkgtosetifnotalreadyin(installed,sp);
#endif
		ni++;
	    }
	    else
	    {
		ERR << "oops, got NULL despite Selectable said it has installed obj" << endl;
		installed.remove((*it)->name());
	    }
	}
	else
	{
	    installed.remove((*it)->name());
	}

	// candidates to available, and those with marked for install also to toinstall
	if((*it)->has_candidate())
	{
	    PMSolvablePtr sp = (*it)->candidateObj();
	    if(sp != NULL)
	    {
#ifndef KEEPSETS
		available.add(sp);
#else
		addpkgtosetifnotalreadyin(available,sp);
#endif
		na++;

		if((*it)->to_install())
		{
#ifndef KEEPSETS
		    toinstall.add(sp);
#else
		    addpkgtosetifnotalreadyin(toinstall,sp);
#endif
		    nt++;
		}
		else
		{
		    toinstall.remove((*it)->name());
		}
	    }
	    else
	    {
		ERR << "oops, got NULL despite Selectable said it has candidateObj obj" << endl;
		available.remove((*it)->name());
		toinstall.remove((*it)->name());
	    }
	}
	else
	{
	    available.remove((*it)->name());
	    toinstall.remove((*it)->name());
	}
    }

    MIL << stringutil::form("%d installed, %d available, %d to install", ni, na, nt) << endl;
}

// predicate class for finding results for a PkgName
struct ResultEqName {
	PkgName name;
	ResultEqName( PkgName n ) : name(n) {}
	bool operator() ( const PkgDep::Result& res ) { return res.name == name; }
};

// set packages not from input list to auto, those that can't be set to auto
// are added to bad list
//
// installed is a set of packages that are assumed to be installed,
// used for building remove_to_solve_conflict lists
static void setAutoState(PkgSet& installed, PkgDep::ResultList& good, PkgDep::ErrorResultList&
bad, PkgDep::ErrorResultList& obsolete)
{
    bool deletedone = false;
    for(PkgDep::ResultList::iterator it = good.begin();
	    it != good.end(); (deletedone?0:++it))
    {
	deletedone = false;
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
	    {
		WAR << "could not set " << it->name <<
		    " to status auto install, current state is " << selp->status() << endl;
		PkgDep::ErrorResult err(*it);
		err.state_change_not_possible = true;
		bad.push_back(err);
		deletedone = true;
		PkgDep::ResultList::iterator it2 = it;
		++it;
		good.erase(it2);
		continue;
	    }
	else
	{
	    if(selp->has_installed())
	    {
#warning hack for unknown file dependencies
		PMSolvablePtr i = selp->installedObj();
		PMSolvablePtr c = selp->candidateObj();
		if( i != NULL && c != NULL && i->edition() == c->edition())
		{
		    WAR << "reverting auto state of " << i->name() << " as same version is already installed" << endl;
		    selp->auto_unset();
		}
	    }
	}
    }


    // go through error list and search for packages that could not
    // be set to autoinstall. For those packages, a remove list is
    // generated
    for(PkgDep::ErrorResultList::iterator it = bad.begin();
	    it != bad.end(); ++it)
    {
    	if(!it->referers.empty())
	{
	    PkgSet fakei;
	    D__ << "before assign" << endl;
	    fakei = installed;
	    D__ << "after assign" << endl;
	    for(PkgDep::RelInfoList::iterator rit = it->referers.begin()
		; rit != it->referers.end(); ++rit)
	    {
		PMSolvablePtr p = rit->solvable;
		if(!p)
		    {INT << "p is NULL" << endl; continue; }

		if(p == it->solvable) continue; // do not remove the package that has the conflict

		if(!fakei.includes(p->name())) continue;
		D__ << "remove " <<  p->name() << endl;
		PkgDep::remove_package(&fakei, p, it->remove_referers);
	    }
	}
    }

    for(PkgDep::ErrorResultList::iterator it = obsolete.begin();
	    it != obsolete.end(); ++it)
    {
	PkgDep::ErrorResult* err = &(*it);

	PMObjectPtr op = err->solvable;

	if(!op)
		{ ERR << "result " << err->name << " didn't contain a valid object" << endl; continue; };

	PMSelectablePtr selp = op->getSelectable();

	if(selp == NULL)
	    { ERR << "good result with NULL selectable: " << err->name << endl; continue; }

	if(!selp->auto_set_delete())
	{
		ERR << "could not set " << err->name << " to status auto delete" << endl;
		err->state_change_not_possible = true;
		bad.push_back(*err);
		good.remove_if( ResultEqName(err->name) );
	}
    }
}

static ostream& operator<<( ostream& os, const struct mallinfo& i )
{
    os << "Memory from system: " << (i.arena >> 10) << "k, used: " << (i.uordblks >> 10) << "k";
    return os;
}

bool PMManager::solveInstall(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad,
	bool filter_conflicts_with_installed)
{
    bool repeat = false;
    short count = 5; // repeat maximal five times, shouldn't happen more then once but who knows ...
    bool success = false;
    PkgDep::ErrorResultList obsolete;

    do
    {
#ifndef KEEPSETS
	PkgSet* installed = new PkgSet(); // already installed
	PkgSet* available = new PkgSet(); // available for installation
	PkgSet* toinstall = new PkgSet(); // user selected for installion
	PkgSet* nowinstalled = new PkgSet(); // assumed state after operation
#else
	if(!installed) installed = new PkgSet(); // already installed
	if(!available) available = new PkgSet(); // available for installation
	if(!toinstall) toinstall = new PkgSet(); // user selected for installion
	if(!nowinstalled) nowinstalled = new PkgSet(); // assumed state after operation
#endif
	if(!installed || !available || !toinstall || !nowinstalled )
	{
	    INT << "memory exhausted" << endl;
	    return false;
	}
	repeat = false;

	MIL << mallinfo() << endl;

	buildSets(*installed, *available, *toinstall);
	MIL << mallinfo() << " (after set contruction)" << endl;

	PkgDep* engine = new PkgDep( *installed, *available ); // TODO alternative_default
	if(!engine)
	{
	    INT << "memory exhausted" << endl;
	    return false;
	}
	MIL  << mallinfo() << " (after engine construction)" << endl;
	success = engine->solvesystemnoauto( *toinstall, good, bad, obsolete);
	MIL << mallinfo() << " (after solver run)" << endl;
#ifndef KEEPSETS
	delete installed;
	delete available;
	delete toinstall;
	installed = available = toinstall = NULL;
#endif
	delete engine;
	engine = NULL;
	MIL << mallinfo() << endl;

	buildinstalledonly(*this,*nowinstalled);
	MIL << mallinfo() << endl;

	// revert install request set by appl if it introduces a conflict
	if(!success && filter_conflicts_with_installed)
	{
	    // iterate through error list
	    for(PkgDep::ErrorResultList::iterator it = bad.begin();
		    it != bad.end(); ++it)
	    {
		PMObjectPtr op = it->solvable; // who has conflict, e.g postfix
		// must be an unavailable
		if(!op) continue;
		PMSelectablePtr who = op->getSelectable();
		if(!who) continue;

		// iterate through conflicts
		for(PkgDep::RelInfoList_iterator rlit = it->conflicts_with.begin();
		    rlit != it->conflicts_with.end(); ++rlit)
		{
		    DBG << "checking conflict introduced by " << rlit->name << endl;
		    if(!it->solvable) continue;
		    PMObjectPtr o = rlit->solvable;
		    if(!o) continue;
		    PMSelectablePtr what = o->getSelectable(); // what is causing the conflict, e.g. sendmail
		    // only conflicts
		    if(!what || rlit->kind != PkgDep::RelInfo::CONFLICT) continue;

		    if(what->has_installed() && who->by_appl())
		    {
			WAR << who->name()
			    << " conflicts with installed "
			    << what->name()
			    << ", dropping "
			    << who->name()
			    << endl;
			who->appl_unset();
			repeat = true;
		    }
		}
	    }
	}

	setAutoState(*nowinstalled, good, bad, obsolete);

#ifndef KEEPSETS
	delete nowinstalled;
	nowinstalled = NULL;
#endif

	count--;
    } while(repeat && count > 0);

    return bad.empty();
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

/*
bool PMManager::solveUpgrade(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad, PkgDep::SolvableList& to_remove)
{
    PkgSet installed; // already installed
    PkgSet available; // available for installation
    PkgSet toinstall; // user selected for installion

    buildSets(installed, available, toinstall);

    PkgDep engine( installed, available ); // TODO alternative_default
//    engine.set_unresolvable_callback(unresolvable_callback); //TODO

    bool success = engine.upgrade( toinstall, good, bad, to_remove, true);

    setAutoState(good, bad, to_remove);

    return success;
}

void PMManager::setMaxRemoveThreshold(unsigned nr)
{
    PkgDep::set_default_max_remove(nr);
}
*/
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
