#include <y2pm/PkgDep.h>
#include <y2pm/PkgDep_int.h>

using namespace std;


bool PkgDep::consistent( ErrorResultList& failures )
{
	failures = ErrorResultList();

	// for all installed packages...
	ci_for( PkgSet::,, _pkg, installed., ) {
		PMSolvablePtr pkg = _pkg->second;
		ErrorResult err(*this,pkg);
		if (!pkg_consistent( pkg, &err ))
			failures.push_back( err );
	}

	return failures.empty();
}


// checks one package for consistency
bool PkgDep::pkg_consistent( PMSolvablePtr pkg, ErrorResult *err )
{
	bool error = false;

	// for all requirements of the current package
	ci_for( PMSolvable::,PkgRelList_, req, pkg->,requires_ ) {
		bool match_found = false;

/*
		if (!strncmp(req->name(),"rpmlib(",strlen("rpmlib(")))
			continue;
*/
/*
		// ignore rpmlib requirements
		if(req->name()->find("rpmlib(") != string::npos)
		    continue;
*/

		RevRel_for( PkgSet::getRevRelforPkg(installed.provided(),req->name()), prov ) {
			if (req->matches( prov->relation() )) {
				match_found = true;
				break;
			}
		}
		if(!match_found)
		{
		    PMSolvablePtr ptr = NULL;
		    WhatToDoWithUnresolvable what = _unresolvable_callback(this, *req, ptr);
		    switch(what)
		    {
			case UNRES_IGNORE:
			case UNRES_TAKETHIS:
			    continue;
			case UNRES_FAIL:
			    break;
		    }
		}
		if (!match_found) {
			if (err)
				err->add_unresolvable( pkg, *req );
			error = true;
		}
	} // /requirements

	// for all conflicts of the current package
	ci_for( PMSolvable::,PkgRelList_, confl, pkg->,conflicts_ ) {
		RevRel_for( PkgSet::getRevRelforPkg(installed.provided(),confl->name()), prov ) {
			if (confl->matches( prov->relation() ) && prov->pkg() != pkg) {
				if (err)
					err->add_conflict( pkg, *confl, *this,  prov->pkg(), NULL );
				error = true;
			}
		}
	}

	return !error;
}

// Local Variables:
// tab-width: 4
// End:
