#include <PkgDep.h>
#include <PkgDep_int.h>


bool PkgDep::consistent( ErrorResultList& failures )
{
	failures = ErrorResultList();

	// for all installed packages...
	ci_for( PkgSet::, _pkg, installed. ) {
		const Solvable *pkg = _pkg->value;
		ErrorResult err(*this,pkg);
		if (!pkg_consistent( pkg, &err ))
			failures.push_back( err );
	}

	return failures.empty();
}


// checks one package for consistency
bool PkgDep::pkg_consistent( const Solvable *pkg, ErrorResult *err )
{
	bool error = false;

	// for all requirements of the current package
	ci_for( Solvable::PkgRelList_, req, pkg->requires_ ) {
		bool match_found = false;

		if (!strncmp(req->name(),"rpmlib(",strlen("rpmlib(")))
			continue;

		RevRel_for( installed.provided()[req->name()], prov ) {
			if (req->matches( prov->relation() )) {
				match_found = true;
				break;
			}
		}
		if (!match_found) {
			if (err)
				err->add_unresolvable( pkg->name(), *req );
			error = true;
		}
	}
	// for all conflicts of the current package
	ci_for( Solvable::PkgRelList_, confl, pkg->conflicts_ ) {
		RevRel_for( installed.provided()[confl->name()], prov ) {
			if (confl->matches( prov->relation() ) && prov->pkg() != pkg) {
				if (err)
					err->add_conflict( pkg->name(), *confl,
									   *this,  prov->pkg(), NULL );
				error = true;
			}
		}
	}

	return !error;
}

// Local Variables:
// tab-width: 4
// End:
