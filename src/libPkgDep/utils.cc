#include <cassert>
#include <y2util/Y2SLog.h>
#include <y2pm/PkgDep.h>
#include <y2pm/PkgDep_int.h>

using namespace std;

// initialization of static class vars
//const PkgSet* PkgDep::default_avail = new PkgSet();
PkgDep::alternatives_mode PkgDep::default_alternatives_mode =ASK_IF_NO_DEFAULT;
unsigned PkgDep::default_max_remove = 10;

PkgDep::PkgDep ( PkgSet& instd,
	 const PkgSet& avail,
	 AlternativesCallback alternatives_callback,
	 alternatives_mode m
	)
    : alt_mode(m),
	installed(instd),
	available(avail),
	_alternatives_callback(alternatives_callback),
	_install_installed(true)
{
    _unresolvable_callback = default_unresolvable_callback;
}

bool PkgDep::also_provided_by_installed( const PkgRelation& req )
{
	bool also_provided_by_installed = false;
	// originally installed was used here, this must have been a bug
	// (lprng, cups-client, lsb incident, #21829)
	RevRel_for( PkgSet::getRevRelforPkg(vinstalled.provided(),req.name()), prov ) {
		if (req.matches( prov->relation() )) {
			also_provided_by_installed = true;
			break;
		}
	}
	return also_provided_by_installed;
}

unsigned PkgDep::count_providers_for( const PkgSet* set, const PkgRelation& req)
{
	unsigned providers = 0;

	RevRel_for( PkgSet::getRevRelforPkg(set->provided(),req.name()), prov ) {
		if (prov->relation().matches( req )) {
			D__ << req.name() << " satisfied by " << prov->pkg()->name()
				 << " with Provides: " << prov->relation() << std::endl;
			++providers;
		}
	}
	D__ << req.name() << ": total " << providers << " providers" << endl;
	return providers;
}

/** some to-be-installed package provides "provides", however pkg has a
 * conflict on this relation. Lookup the package name in the available set and
 * see if choosing that one as candidate would resolve the conflict
 * */
PMSolvablePtr PkgDep::try_upgrade_conflictor( PMSolvablePtr pkg, const PkgRelation& provides )
{
	PkgName name = pkg->name();

	D__ << "Trying to upgrade conflictor " << name << "-" << pkg->edition()
		 << " to solve confl source provides " << provides << std::endl;
	// if no different version is available, we can't upgrade
	PMSolvablePtr upgrade = available_upgrade(pkg);
	if (!upgrade)
		return NULL;

	ci_for( PMSolvable::,PkgRelList_, confl, upgrade->,conflicts_ ) {
		if (confl->matches( provides )) {
			upgrade = NULL;
			break;
		}
	}
	return upgrade;
}

PMSolvablePtr PkgDep::try_upgrade_conflicted( PMSolvablePtr pkg, const PkgRelation& confl )
{
	PkgName name = pkg->name();

	D__ << "Trying to upgrade provider " << name << "-" << pkg->edition()
		 << " to solve conflict " << confl << "\n";
	// if no different version is available, we can't upgrade
	PMSolvablePtr upgrade = available_upgrade(pkg);
	if (!upgrade)
		return NULL;

	// There are two cases: confl.name() isn't provided anymore, or the new
	// provided version doesn't match the conflict anymore. Check all names
	// the upgrade provides if they still match the conflict.
	ci_for( PMSolvable::,Provides_, prov, upgrade->,all_provides_ ) {
		if (confl.matches( *prov )) {
			upgrade = NULL;
			break;
		}
	}
	return upgrade;
}

/** pkg requires something that is no longer there when oldpkg is replaced by
 * newpkg. Try to find a replacement for pkg in the hope that the requirement
 * is satisfied then.
 * */
PMSolvablePtr PkgDep::try_upgrade_requirerer(
	PMSolvablePtr pkg, PMSolvablePtr oldpkg, PMSolvablePtr newpkg)
{
	PkgName name = pkg->name();

	D__ << "Trying to upgrade requirerer " << name << "-" << pkg->edition()
		 << " to solve broken requirement\n";
	// if no different version is available, we can't upgrade
	PMSolvablePtr upgrade = available_upgrade(pkg);
	if (!upgrade) {
		D__ << "no upgrade available for " << pkg->name() << "\n";
		return NULL;
	}

	// check if all requirements of upgrade that have oldpkg or newpkg as
	// target are now satisfied
	bool requirements_ok = true;
	ci_for( PMSolvable::,PkgRelList_, req, upgrade->,requires_ ) {
		// ln -- doesn't make sense, could be indirect requirement
		//if ((req->name() == oldpkg->name() || req->name() == newpkg->name()) &&
		
		if(!req_ok_after_upgrade( *req, oldpkg, newpkg )) {
			requirements_ok = false;
			break;
		}
	}
	if( !requirements_ok) D__ << "upgrade still broken" << endl;
	return requirements_ok ? upgrade : (PMSolvablePtr)0;
}

/** try to find a different version of pkg in the available set
 * */
PMSolvablePtr PkgDep::available_upgrade( PMSolvablePtr pkg )
{
	PMSolvablePtr upgrade;

	// if no different version is available, we can't upgrade
	if (!available.includes(pkg->name()) ||
		(upgrade = available[pkg->name()])->edition() == pkg->edition()) {
		D__ << "  not possible, no different edition available\n";
		return NULL;
	}
	return upgrade;
}

void PkgDep::do_upgrade_for_conflict( PMSolvablePtr upgrade )
{
	assert( upgrade != NULL );
	PkgName name = upgrade->name();

	if (candidates->includes(name) && ((*candidates)[name] == upgrade)) {
		D__ << "Would upgrade " << name << " for solving conflict, but it's "
			 "already a candidate\n";
	}
	else {
		D__ << "Upgrading " << name << " to " << upgrade->edition()
			 << " to solve conflict\n";
		candidates->add( upgrade );
		to_check.push_back( upgrade );
		notes[name].upgrade_to_solve_conflict = true;
	}
}

bool PkgDep::has_conflict_with( const PkgRelation& prov, PMSolvablePtr pkg )
{
	ci_for( PMSolvable::,PkgRelList_, confl, pkg->,conflicts_ ) {
		if (confl->matches( prov ))
			return true;
	}
	return false;
}

void PkgDep::add_referer( const PkgName& name, PMSolvablePtr referer,
						  const PkgRelation& rel )
{
	notes[name].referers.push_back( IRelInfo(referer,rel) );
}

void PkgDep::add_not_available(PMSolvablePtr referer, const PkgRelation& rel )
{
	PkgName name = rel.name();

	// add an error result if it doesn't exist yet
	bool found = false;
	ci_for( ErrorResultList::,, p, bad->, ) {
		if (p->name == name) {
			found = true;
			break;
		}
	}
	if (!found)
		bad->push_back( ErrorResult(*this, rel.name()) );

	add_referer( name, referer, rel );
	notes[name].not_available = true;
	notes[name].not_avail_range.merge( rel );
}

PkgDep::WhatToDoWithUnresolvable PkgDep::default_unresolvable_callback(
    PkgDep* solver, const PkgRelation& rel, PMSolvablePtr& p)
{
    if(rel.name()->find("rpmlib(") != std::string::npos)
	return UNRES_IGNORE;

    return UNRES_FAIL;
}

// Local Variables:
// tab-width: 4
// End:
