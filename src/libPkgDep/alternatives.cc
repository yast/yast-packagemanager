#include <y2pm/PkgDep.h>
#include <y2pm/PkgDep_int.h>

using namespace std;

void PkgDep::handle_alternative( const AltInfo& alt_info )
{
	PMSolvablePtr cand = alt_info.pkg;
	PkgName reqname = alt_info.req.name();
	
	// Has this alternative already been handled? This can happen because
	// add_package stores the alternatives that need later processing on a
	// list, so double entries can happen. We need those double entries so
	// that we can add the 2nd and following packages as referers.
	if (cand && alts_handled.exists(reqname)) {
		if (vinstalled.provided()[reqname].size() != 0) {
			PMSolvablePtr first_provider
				= vinstalled.provided()[reqname].front().pkg();
			DBG( "Alternative for " << reqname << " already handled -- "
				 "adding reference from " << cand->name() << " on "
				 << alt_info.req << " provided by "
				 << first_provider->name() << endl );
			add_referer( first_provider, cand, alt_info.req );
		}
		return;
	}

	DBG( "Handling alternative for " << reqname << " " );
	if (cand)
		DBG( "needed by " << cand->name() << " as \"Requires: "
			 << alt_info.req << "\"\nProviders are:\n" );
	else
		DBG( " (all providers are self-conflicting candidates\n" );
	
	ci_for( RevRelList_, alt, alt_info.providers. ) {
		DBG( "  " << alt->pkg()->name() << "-" << alt->pkg()->edition()
			 << " provides: " << alt->relation() << endl );
	}

	// check status of alternatives: an alt could 1) conflict with something,
	// 2) require more packages, or 3) nothing of the above
	hash<PkgName,alternative_kind> altkind;
	ci_for( RevRelList_, alt, alt_info.providers. ) {
		PkgName altname = alt->pkg()->name();
		DBG( "Checking consistency of alternative " << altname << ": " );
		ErrorResult err( *this, alt->pkg() );
		pkg_consistent( alt->pkg(), &err );
		if (err.conflicts_with.size()) {
			DBG( "conflict\n" );
			altkind[altname] = CONFLICT;
		}
		else if (err.unresolvable.size()) {
			DBG( "requires-more\n" );
			altkind[altname] = REQUIRES_MORE;
		}
		else {
			DBG( "simple\n" );
			altkind[altname] = SIMPLE;
		}
	}

	PMSolvablePtr use_pkg = NULL;
	switch( alt_mode ) {

	  case ASK_ALWAYS:
		// always let the caller handle alternatives
		// leave use_pkg == NULL
		break;

	  case ASK_IF_NO_DEFAULT:
	  case AUTO_IF_NO_DEFAULT:
	  {
		const Alternatives::AltDefaultList& defaults
			= _alternatives_callback(reqname);
		if (defaults.size() == 0) {
			if (alt_mode == ASK_IF_NO_DEFAULT)
				// no defaults -> let caller decide
				break;
			else // AUTO_IF_NO_DEFAULT
				goto auto_select;
		}

		Alternatives::AltDefaultList useable_defaults;
		if (cand) {
			// filter out defaults that do not exist or not provide what we
			// need
			DBG( "Filtering defaults:\n" );
			ci_for( Alternatives::AltDefaultList::, def, defaults. ) {
				DBG( "  " << *def << ": " );
				if (!available.includes(*def)) {
					DBG( "not available -- skipping\n" );
					continue;
				}
				PMSolvablePtr pkg = available[*def];
				bool found_match = false;
				ci_for( PMSolvable::Provides_, prov, pkg->all_provides_ ) {
					if (alt_info.req.matches( *prov )) {
						found_match = true;
						break;
					}
				}
				if (found_match) {
					DBG( "ok\n" );
					useable_defaults.push_back( *def );
				}
				else
					DBG( "does not provide what we need -- skipping\n" );
			}
			if (useable_defaults.size() == 0) {
				// If no defaults are left, i.e. none provides what we need,
				// this is probably an error in the defaults database. Warn
				// about it and use "never" behaviour as a fallback.
				cerr << "libPkgDep warning: none of the defaults for "
					 << cand->name() << " provides a suitable version of "
					 << reqname << "\n"
					 << "This could be an error in the defaults list.\n";
				if (alt_mode == ASK_IF_NO_DEFAULT)
					// let caller decide
					break;
				else // AUTO_IF_NO_DEFAULT
					goto auto_select;
			}
		}
		else {
			// filter out defaults that aren't on the providers list
			DBG( "Filtering defaults:\n" );
			ci_for( Alternatives::AltDefaultList::, def, defaults. ) {
				DBG( "  " << *def << ": " );
				bool found = false;
				ci_for( RevRelList_, alt, alt_info.providers. ) {
					if (alt->pkg()->name() == *def) {
						found = true;
						break;
					}
				}
				if (found) {
					DBG( "ok, is an alternative\n" );
					useable_defaults.push_back( *def );
				}
				else {
					DBG( "not ok, isn't one of the alternatives\n" );
				}
			}
		}

		// Select the first default without a conflict (additional
		// requirements ignored here -- if it's a default, accept that we
		// maybe need more packages for it). If no conflict-free default
		// found, use the first one.
		ci_for( Alternatives::AltDefaultList::, def, useable_defaults. ) {
			if (altkind[*def] != CONFLICT) {
				use_pkg = available[*def];
				break;
			}
		}
		if (!use_pkg)
			use_pkg = available[useable_defaults.front()];
		break;
	  }

	  case AUTO_ALWAYS:
	  auto_select:
		// Pick a random provider from the first non-empty class of
		// (no-requirements, requirements, conflicts)
		ci_for( RevRelList_, alt, alt_info.providers. ) {
			PkgName altname = alt->pkg()->name();
			if (altkind[altname] == SIMPLE) {
				use_pkg = alt->pkg();
				break;
			}
		}
		if (use_pkg)
			break;

		ci_for( RevRelList_, alt, alt_info.providers. ) {
			PkgName altname = alt->pkg()->name();
			if (altkind[altname] == REQUIRES_MORE) {
				use_pkg = alt->pkg();
				break;
			}
		}
		if (!use_pkg)
			use_pkg = alt_info.providers.front().pkg();
		break;
	}

	if (use_pkg) {
		// ok, an alternative has been selected, add it as candidate
		DBG( "Selected " << use_pkg->name() << " as alternative.\n" );
		candidates->add( use_pkg, true );
		to_check.push_back( use_pkg );
		
		if (cand) {
			add_referer( use_pkg, cand, alt_info.req );
			DBG( "Candidate " << cand->name() << " ok\n" );
			good->push_back( alt_info.result );
			vinstalled.add( cand, true );
		}
	}
	else {
		// no alternative auto-selected, let the caller decide, i.e. generate
		// an ErrorResult
		// the alternatives list is sorted by kind (SIMPLE, REQUIRES, CONFL)
		DBG( "No alternative selected -- generating ErrorResult\n" );
		ErrorResult err(*this,reqname);
		ci_for( RevRelList_, alt, alt_info.providers. ) {
			PkgName altname = alt->pkg()->name();
			if (altkind[altname] == SIMPLE)
				err.add_alternative( altname, altkind[altname] );
		}
		ci_for( RevRelList_, alt, alt_info.providers. ) {
			PkgName altname = alt->pkg()->name();
			if (altkind[altname] == REQUIRES_MORE)
				err.add_alternative( altname, altkind[altname] );
		}
		ci_for( RevRelList_, alt, alt_info.providers. ) {
			PkgName altname = alt->pkg()->name();
			if (altkind[altname] == CONFLICT)
				err.add_alternative( altname, altkind[altname] );
		}
		if (cand)
			add_referer( reqname, cand, alt_info.req );
		bad->push_back( err );

		if (cand) {
			DBG( "Candidate " << cand->name() << " failed\n" );
			ErrorResult res = alt_info.result;
			res.add_unresolvable( cand->name(), alt_info.req );
			bad->push_back( res );
		}
		else {
			ci_for( RevRelList_, alt, alt_info.providers. ) {
				ErrorResult e(*this, alt->pkg());
				ci_for( RevRelList_, alt2, alt_info.providers. ) {
					if (alt2->pkg() != alt->pkg())
						e.add_conflict( alt2->pkg()->name(), alt_info.req,
										*this, NULL, NULL );
				}
				bad->push_back( e );
			}
		}
	}
	
	alts_handled.insert( reqname );
}



// Local Variables:
// c-basic-offset: 4
// tab-width: 4
// End:
