#include <y2util/Y2SLog.h>
#include <y2pm/PkgDep.h>
#include <y2pm/PkgDep_int.h>
#include <y2util/hash.h>

#include <set>

using namespace std;

// predicate class for finding results for a PkgName
struct ResultEqName {
	PkgName name;
	ResultEqName( PkgName n ) : name(n) {}
	bool operator() ( const PkgDep::Result& res ) { return res.name == name; }
};


struct fromBrokeninstalled {
	PkgSet& _b;
	fromBrokeninstalled( PkgSet& b ) : _b(b){}
	bool operator() ( const PkgDep::Result& res )
	{
	    PMSolvablePtr p = _b.lookup(res.name);
	    if(p != NULL)
	    {
		if(p->edition() == res.edition)
		{
		    D__ << "remove " << res.name << " from goodlist" << endl;
		    return true;
		}
	    }
	    return false;
	}
};


static void deselect_referers(
	set<PkgName>& visited,
	PkgName pkgname,
	PkgSet& candidates,
	set<PMSolvablePtr>& noinstcandidates,
	const list<PkgDep::RelInfo>& referers,
	const PkgDep::ResultList& good, const PkgDep::ErrorResultList& bad
) {
	if(visited.find(pkgname) != visited.end()) // endless recursion protection
	    return;
	visited.insert(pkgname);

	ci_for( list<PkgDep::RelInfo>::,, p, referers., ) {
//		D__ << " deselecting " << p->name << "\n";
		if (p->name == pkgname)
			continue;
		PMSolvablePtr cand = candidates.lookup(p->name);
		if ( cand != NULL )
		{
			candidates.remove( cand );
			noinstcandidates.insert( cand );
		}
		ci_for( PkgDep::ResultList::,, q, good., )
			if (q->name == p->name)
			{
				deselect_referers( visited, p->name, candidates, noinstcandidates, q->referers, good, bad );
			}
		ci_for( PkgDep::ErrorResultList::,, q, bad., )
			if (q->name == p->name)
			{
				deselect_referers( visited, p->name, candidates, noinstcandidates, q->referers, good, bad );
			}
	}
}


#if 0
// probably fucked up
bool PkgDep::upgrade(
	PkgSet &candidates, ResultList& out_good, ErrorResultList& out_bad,
	SolvableList& to_remove, bool all, bool none, unsigned max_remove )
	{
	PkgSet installed_backup = installed;
	noval_hash<PkgName> real_from_input_list;
	noval_hash<PkgName> upgrades_solving_conflicts;
	noval_hash<PkgName> avoid_break_installs;
	to_remove = SolvableList();
	unsigned numinconsistent = 0;
	set<PMSolvablePtr> noinstcandidates;
	set<PMSolvablePtr> brokeninstalled;

#warning function unusable

	D__ << "Starting upgrade\n";

//	installed.dumpOn(cout);

	{
	    ci_for( PkgSet::,, it, installed., ) {
		PkgName iname = it->key;
		PMSolvablePtr instd = it->value;
		// ignore if installed is already a candidate
		if(!candidates.empty() && candidates.includes(iname))
		    continue;

		// add installed as candiate if it's inconsistent
		if(!pkg_consistent(instd, NULL))
		{
		    numinconsistent++;
		    brokeninstalled.insert(instd);
    //		notes[instd->name()].inconsistent = true;
		}
	    }
	    D__ << "moving inconsistent from installed to candidates: ";
	    i_for( set<PMSolvablePtr>::,, bit, brokeninstalled., )
	    {
		if(installed.includes((*bit)->name()))
		{
		    SolvableList toremove;
		    remove_package(&installed, *bit, toremove);
		    ci_for(SolvableList::,, it, toremove.,)
		    {
			brokeninstalled.insert(*it);
			candidates.add( *it );
			D__ << (*it)->name() << " ";
		    }
		}
	    }
	    D__ << endl;
	}


	if (!none && (all || candidates.empty())) {
		// for all installed packages...
		ci_for( PkgSet::,, p, installed., ) {
			PkgName iname = p->key;
			PMSolvablePtr ipkg = p->value;
			bool added = false;

			// if there's a newer version available, make it a candidate
			PMSolvablePtr upgrade;
			if (available.includes(iname) &&
				(upgrade = available[iname])->edition() > ipkg->edition()) {
				D__ << iname << ": upgrade from " << ipkg->edition()
					 << " to " << upgrade->edition() << endl;
				candidates.add( upgrade );
				real_from_input_list.insert( upgrade->name() );
				added = true;
			}

			// also, if an available packages obsoletes this installed package
			if (!added) {
				RevRel_for( available.obsoleted()[iname], obs ) {
					if (obs->relation().matches( ipkg->self_provides() )) {
						D__ << iname << ": obsoleted by available "
							 << obs->pkg()->name() << "; installing "
							 << obs->pkg()->name() << endl;
						candidates.add( obs->pkg() );
						real_from_input_list.insert( obs->pkg()->name() );
						added = true;
					}
				}
			}

			if (!added)
				D__ << iname << ": no upgrade\n";
		}
	}

	bool all_ok = true;
	int endless_protect = 100;
	while (--endless_protect > 0) {

		// try installation of the candidates
		DBG << "-------------------- install run --------------------\n";
		install( candidates, out_good, out_bad, false );
		DBG << "-------------------- install end --------------------\n";

		if (out_bad.empty())
			// no problems...
			goto out;

		// fix fixable problems
		ci_for( ErrorResultList::,, p, out_bad., ) {
			if (p->install_to_avoid_break)
				avoid_break_installs.insert( p->name );
			if (p->upgrade_to_remove_conflict)
				upgrades_solving_conflicts.insert( p->name );

			// if some pkg needs something that's not available, don't upgrade
			// it
			if (p->not_available) {
				DBG << p->name <<" is not available, deselecting its referers\n";
				set<PkgName> visited;
				deselect_referers( visited, PkgName("<none>"), candidates, noinstcandidates, p->referers, out_good, out_bad);
			}

			// alternatives possible: simply choose first one
			if (!p->alternatives.empty()) {
				PMSolvablePtr alt = p->alternatives.front().solvable;
				if(alt == NULL)
				{
				    INT << "alt may not be NULL";
				    break;
				}
				D__ << "Choosing " << alt << " as alternative for "
					 << p->name << endl;
				candidates.add( alt );
				if (p->install_to_avoid_break)
					avoid_break_installs.insert( alt->name() );
			}

			// fix conflicts by removing packages, except there is too much to
			// remove
			if (!p->conflicts_with.empty()) {
				if (p->remove_to_solve_conflict.size() > max_remove) {
					D__ << "too many packages ("
						 << p->remove_to_solve_conflict.size()
						 << ") to remove for conflict(s) of " << p->name
						 << " -- aborting upgrade\n";
					all_ok = false;
					goto out;
				}
				ci_for( SolvableList::,, q, p->remove_to_solve_conflict., ) {
					if (candidates.includes((*q)->name())) {
						D__ << "removing candidate " << (*q)->name()
							 << " due to conflict with " << p->name << endl;
						candidates.remove( (*q)->name() );
					}
					else {
						DBG << "removing installed " << (*q)->name()
							 << " due to conflict with " << p->name << endl;
						to_remove.push_back( *q );
					}
				}
			}
		}

		ci_for( ResultList::,, p, out_good., ) {
			if (p->install_to_avoid_break)
				avoid_break_installs.insert( p->name );
			if (p->upgrade_to_remove_conflict)
				upgrades_solving_conflicts.insert( p->name );
		}

		// ok, everything fixed, remove to_remove pkgs from installed
		//DBG << "Ok, removing " << to_remove << endl;
		ci_for( SolvableList::,, p, to_remove., )
			installed.remove( (*p)->name() );
	}
  out:
	if (endless_protect <= 0)
		all_ok = false;

	if (!all_ok)
		// revert installed list
		installed = installed_backup;
	else {
		i_for( PkgDep::ResultList::,, p, out_good., ) {
			p->from_input_list = real_from_input_list.exists( p->name );
			if (avoid_break_installs.exists( p->name ))
				p->install_to_avoid_break = true;
			if (upgrades_solving_conflicts.exists( p->name ))
				p->upgrade_to_remove_conflict = true;
		}
		i_for( PkgDep::ErrorResultList::,, p, out_bad., ) {
			p->from_input_list = real_from_input_list.exists( p->name );
			if (avoid_break_installs.exists( p->name ))
				p->install_to_avoid_break = true;
			if (upgrades_solving_conflicts.exists( p->name ))
				p->upgrade_to_remove_conflict = true;
		}
	}


	// go through not installed candidates, mark those as to_remove that
	// originally were broken installed packages
	ci_for( set<PMSolvablePtr>::,, bit, noinstcandidates., )
	{
	    if(brokeninstalled.find(*bit) != brokeninstalled.end())
	    {
		cout << "remove broken " << (*bit)->name() << endl;
		to_remove.push_back(*bit);
	    }
	    else
		cout << "not installing " << (*bit)->name() << endl;
	}

	return all_ok;
}
#endif

bool PkgDep::solvesystemnoauto(
	PkgSet &candidates,
	ResultList& out_good,
	ErrorResultList& out_bad,
	ErrorResultList& out_obsoleted)
{
	unsigned numinconsistent = 0;
	set<PMSolvablePtr> noinstcandidates;
	PkgSet brokeninstalled;

	D__ << "Starting solver\n";

//	go through all installed, put all inconsistent into candidates and remove them from installed
	{
	    ci_for( PkgSet::,, it, installed., )
	    {
		bool obsolete = false;
		PkgName iname = it->first;
		PMSolvablePtr instd = it->second;
		// ignore if installed is already a candidate
		if(!candidates.empty() && candidates.includes(iname))
		    continue;

		RevRel_for( PkgSet::getRevRelforPkg(candidates.obsoleted(),iname), obs ) {
			if (obs->relation().matches( instd->self_provides() )) {
				WAR << "installed " << iname << " is obsoleted by candidate "
					 << obs->pkg()->name() << " -- not checking consistency"
					 << endl;
				if(iname == obs->pkg()->name())
				{
				    ERR << obs->pkg()->nameEd() << " obsoletes itself!" << endl;
				}
				obsolete = true;
			}
		}

		// add installed as candiate if it's inconsistent
		if(!obsolete && !pkg_consistent(instd, NULL))
		{
		    numinconsistent++;
		    brokeninstalled.add(instd);
    //		notes[instd->name()].inconsistent = true;
		}
	    }

	    D__ << "moving inconsistent from installed to candidates: ";
	    i_for( PkgSet::,, bit, brokeninstalled., )
	    {
		if(installed.includes(bit->second->name()))
		{
		    D__ << (bit->second->name());
		    if(!candidates.includes( bit->second->name()))
		    {
			installed.remove( bit->second );
			candidates.add( bit->second );
			D__ << " ";
		    }
		    else
		    {
			D__ << "(NOT!) ";
		    }
		}
		else
		    INT << bit->second->name() << " not in installed" << endl;
	    }
	    D__ << endl;
	}

	// try installation of the candidates
	DBG << "-------------------- install run --------------------\n";
	install( candidates, out_good, out_bad, out_obsoleted, true );
	DBG << "-------------------- install end --------------------\n";

	out_good.remove_if( fromBrokeninstalled(brokeninstalled) );

	return out_bad.empty();
}

// Local Variables:
// tab-width: 4
// End:
