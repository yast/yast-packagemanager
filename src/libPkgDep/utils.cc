#include <cassert>
#include <PkgDep.h>
#include <PkgDep_int.h>

// initialization of static class vars
const PkgSet* PkgDep::default_avail = new PkgSet();
PkgDep::alternatives_mode PkgDep::default_alternatives_mode =ASK_IF_NO_DEFAULT;
unsigned PkgDep::default_max_remove = 10;


bool PkgDep::also_provided_by_installed( const PkgRelation& req )
{
	bool also_provided_by_installed = false;
	RevRel_for( installed.provided()[req.name()], prov ) {
		if (req.matches( prov->relation() )) {
			also_provided_by_installed = true;
			break;
		}
	}
	return also_provided_by_installed;
}

unsigned PkgDep::count_providers_for(
	const PkgSet* set, const PkgRelation& req
) const {
	unsigned providers = 0;
	
	RevRel_for( set->provided()[req.name()], prov ) {
		if (prov->relation().matches( req )) {
			DBG( "    satisfied by " << prov->pkg()->name()
				 << " with Provides: " << prov->relation() << std::endl );
			++providers;
		}
	}
	DBG( "    total " << providers << " providers\n" );
	return providers;
}

const Package *PkgDep::try_upgrade_conflictor( const Package *pkg,
											   const PkgRelation& provides )
{
	PkgName name = pkg->name();

	DBG( "Trying to upgrade conflictor " << name << "-" << pkg->edition()
		 << " to solve confl source provides " << provides << std::endl );
	// if no different version is available, we can't upgrade
	const Package *upgrade = available_upgrade(pkg);
	if (!upgrade)
		return NULL;

	bool still_conflicts = false;
	ci_for( Package::PkgRelList_, confl, upgrade->conflicts_ ) {
		if (confl->matches( provides )) {
			still_conflicts = true;
			break;
		}
	}
	return still_conflicts ? NULL : upgrade;
}

const Package *PkgDep::try_upgrade_conflicted( const Package *pkg,
											   const PkgRelation& confl )
{
	PkgName name = pkg->name();

	DBG( "Trying to upgrade provider " << name << "-" << pkg->edition()
		 << " to solve conflict " << confl << "\n" );
	// if no different version is available, we can't upgrade
	const Package *upgrade = available_upgrade(pkg);
	if (!upgrade)
		return NULL;

	// There are two cases: confl.name() isn't provided anymore, or the new
	// provided version doesn't match the conflict anymore. Check all names
	// the upgrade provides if they still match the conflict.
	bool still_conflicts = false;
	ci_for( Package::Provides_, prov, upgrade->all_provides_ ) {
		if (confl.matches( *prov )) {
			still_conflicts = true;
			break;
		}
	}
	return still_conflicts ? NULL : upgrade;
}

const Package *PkgDep::try_upgrade_requirerer(
	const Package *pkg, const Package *oldpkg, const Package *newpkg
) {
	PkgName name = pkg->name();

	DBG( "Trying to upgrade requirerer " << name << "-" << pkg->edition()
		 << " to solve broken requirement\n" );
	// if no different version is available, we can't upgrade
	const Package *upgrade = available_upgrade(pkg);
	if (!upgrade) {
		DBG( "no upgrade available for " << pkg->name() << "\n");
		return NULL;
	}

	// check if all requirements of upgrade that have oldpkg or newpkg as
	// target are now satisfied
	bool requirements_ok = true;
	ci_for( Package::PkgRelList_, req, upgrade->requires_ ) {
		if ((req->name() == oldpkg->name() || req->name() == newpkg->name()) &&
			!req_ok_after_upgrade( *req, oldpkg, newpkg )) {
			requirements_ok = false;
			break;
		}
	}
	return requirements_ok ? upgrade : NULL;
}

const Package *PkgDep::available_upgrade( const Package *pkg )
{
	const Package *upgrade;

	// if no different version is available, we can't upgrade
	if (!available.includes(pkg->name()) ||
		(upgrade = available[pkg->name()])->edition() == pkg->edition()) {
		DBG( "  not possible, no different edition available\n" );
		return NULL;
	}
	return upgrade;
}

void PkgDep::do_upgrade_for_conflict( const Package *upgrade )
{
	assert( upgrade != NULL );
	PkgName name = upgrade->name();
	
	if (candidates->includes(name) && ((*candidates)[name] == upgrade)) {
		DBG( "Would upgrade " << name << " for solving conflict, but it's "
			 "already a candidate\n" );
	}
	else {
		DBG( "Upgrading " << name << " to " << upgrade->edition()
			 << " to solve conflict\n" );
		candidates->add( upgrade );
		to_check.push_back( upgrade );
		notes[name].upgrade_to_solve_conflict = true;
	}
}

bool PkgDep::has_conflict_with( const PkgRelation& prov, const Package *pkg )
{
	ci_for( Package::PkgRelList_, confl, pkg->conflicts_ ) {
		if (confl->matches( prov ))
			return true;
	}
	return false;
}

void PkgDep::add_referer( const PkgName& name, const Package *referer,
						  const PkgRelation& rel )
{
	notes[name].referers.push_back( IRelInfo(referer,rel) );
}

void PkgDep::add_not_available(const Package *referer, const PkgRelation& rel )
{
	PkgName name = rel.name();
	
	// add an error result if it doesn't exist yet
	bool found = false;
	ci_for( ErrorResultList::, p, bad-> ) {
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

// Local Variables:
// tab-width: 4
// End:
