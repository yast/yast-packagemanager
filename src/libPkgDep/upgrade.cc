#include <y2pm/PkgDep.h>
#include <y2pm/PkgDep_int.h>
#include <y2util/hash.h>

using namespace std;

static void deselect_referers(
	PkgName pkgname,
	PkgSet& candidates, const list<PkgDep::RelInfo>& referers,
	const PkgDep::ResultList& good, const PkgDep::ErrorResultList& bad
) {
	ci_for( list<PkgDep::RelInfo>::, p, referers. ) {
		// cerr << " deselecting " << p->name << "\n";
		if (p->name == pkgname)
			continue;
		if (candidates.includes( p->name ))
			candidates.remove( p->name );
		ci_for( PkgDep::ResultList::, q, good. )
			if (q->name == p->name && !q->referers.empty())
				deselect_referers( p->name, candidates, q->referers, good, bad );
		ci_for( PkgDep::ErrorResultList::, q, bad. )
			if (q->name == p->name && !q->referers.empty())
				deselect_referers( p->name, candidates, q->referers, good, bad );
	}
}

bool PkgDep::upgrade(
	PkgSet &candidates, ResultList& out_good, ErrorResultList& out_bad,
	NameList& to_remove, unsigned max_remove
) {
	PkgSet installed_backup = installed;
	noval_hash<PkgName> real_from_input_list;
	noval_hash<PkgName> upgrades_solving_conflicts;
	noval_hash<PkgName> avoid_break_installs;
	to_remove = NameList();
	
	DBG( "Starting upgrade\n" );
	
	if (candidates.empty()) {
		// for all installed packages...
		ci_for( PkgSet::, p, installed. ) {
			PkgName iname = p->key;
			PMSolvablePtr ipkg = p->value;
			bool added = false;

			// if there's a newer version available, make it a candidate
			PMSolvablePtr upgrade;
			if (available.includes(iname) &&
				(upgrade = available[iname])->edition() > ipkg->edition()) {
				DBG( iname << ": upgrade from " << ipkg->edition()
					 << " to " << upgrade->edition() << endl );
				candidates.add( upgrade );
				real_from_input_list.insert( upgrade->name() );
				added = true;
			}

			// also, if an available packages obsoletes this installed package
			if (!added) {
				RevRel_for( available.obsoleted()[iname], obs ) {
					if (obs->relation().matches( ipkg->self_provides() )) {
						DBG( iname << ": obsoleted by available "
							 << obs->pkg()->name() << "; installing "
							 << obs->pkg()->name() << endl );
						candidates.add( obs->pkg() );
						real_from_input_list.insert( obs->pkg()->name() );
						added = true;
					}
				}
			}

			if (!added)
				DBG( iname << ": no upgrade\n" );
		}
	}

	bool all_ok = true;
	int endless_protect = 100;
	while (--endless_protect > 0) {

		// try installation of the candidates
		DBG( "-------------------- install run --------------------\n" );
		install( candidates, out_good, out_bad, false );
		DBG( "-------------------- install end --------------------\n" );

		if (out_bad.empty())
			// no problems...
			goto out;

		// fix fixable problems
		ci_for( ErrorResultList::, p, out_bad. ) {
			if (p->install_to_avoid_break)
				avoid_break_installs.insert( p->name );
			if (p->upgrade_to_remove_conflict)
				upgrades_solving_conflicts.insert( p->name );

			// if some pkg needs something that's not available, don't upgrade
			// it
			if (p->not_available) {
				DBG(p->name <<" is not available, deselecting its referers\n");
				deselect_referers( PkgName("<none>"), candidates, p->referers, out_good, out_bad);
			}

			// alternatives possible: simply choose first one
			if (!p->alternatives.empty()) {
				PkgName alt = p->alternatives.front().name;
				DBG( "Choosing " << alt << " as alternative for "
					 << p->name << endl );
				assert( available.includes(alt) );
				candidates.add( available[alt] );
				if (p->install_to_avoid_break)
					avoid_break_installs.insert( alt );
			}

			// fix conflicts by removing packages, except there is too much to
			// remove
			if (!p->conflicts_with.empty()) {
				if (p->remove_to_solve_conflict.size() > max_remove) {
					DBG( "too many packages ("
						 << p->remove_to_solve_conflict.size()
						 << ") to remove for conflict(s) of " << p->name
						 << " -- aborting upgrade\n" );
					all_ok = false;
					goto out;
				}
				ci_for( NameList::, q, p->remove_to_solve_conflict. ) {
					if (candidates.includes(*q)) {
						DBG( "removing candidate " << *q
							 << " due to conflict with " << p->name << endl );
						candidates.remove( *q );
					}
					else {
						DBG( "removing installed " << *q
							 << " due to conflict with " << p->name << endl );
						to_remove.push_back( *q );
					}
				}
			}
		}

		ci_for( ResultList::, p, out_good. ) {
			if (p->install_to_avoid_break)
				avoid_break_installs.insert( p->name );
			if (p->upgrade_to_remove_conflict)
				upgrades_solving_conflicts.insert( p->name );
		}

		// ok, everything fixed, remove to_remove pkgs from installed
		DBG( "Ok, removing " << to_remove << endl );
		ci_for( NameList::, p, to_remove. )
			installed.remove( *p );
	}
  out:
	if (endless_protect <= 0)
		all_ok = false;
	
	if (!all_ok)
		// revert installed list
		installed = installed_backup;
	else {
		i_for( PkgDep::ResultList::, p, out_good. ) {
			p->from_input_list = real_from_input_list.exists( p->name );
			if (avoid_break_installs.exists( p->name ))
				p->install_to_avoid_break = true;
			if (upgrades_solving_conflicts.exists( p->name ))
				p->upgrade_to_remove_conflict = true;
		}
		i_for( PkgDep::ErrorResultList::, p, out_bad. ) {
			p->from_input_list = real_from_input_list.exists( p->name );
			if (avoid_break_installs.exists( p->name ))
				p->install_to_avoid_break = true;
			if (upgrades_solving_conflicts.exists( p->name ))
				p->upgrade_to_remove_conflict = true;
		}
	}
	return all_ok;
}
	
// Local Variables:
// tab-width: 4
// End:
