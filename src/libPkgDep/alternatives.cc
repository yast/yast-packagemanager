#include <y2util/Y2SLog.h>
#include <y2pm/PkgDep.h>
#include "PkgDep_int.h"

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
		if (PkgSet::getRevRelforPkg(vinstalled.provided(),reqname).size() != 0) {
			PMSolvablePtr first_provider
				= PkgSet::getRevRelforPkg(vinstalled.provided(),reqname).front().pkg();
#ifdef phicode
			D__ << "Alternative for " << reqname << " already handled -- "
				 "adding reference from installed/accepted " << cand->name() << " on "
				 << alt_info.req << " provided by "
				 << first_provider->name() << endl;
			add_referer( first_provider, cand, alt_info.req );
#endif
			D__ << "Alternative for " << reqname << " needed by " << cand->name()
				<< " already handled, put back to to_check" << endl;
			// well, now we know that the alternative was already
			// handled but we still don't know whether this
			// particular package is ok. I'll add it back to
			// to_check in the hope that this won't generate
			// loops. this has yet to be proofed -- ln
			to_check.push_back(cand);
		}
		// this branch was not here before. Why we need it: XFree86-GLX
		// and mesasoft provide libGL.so.1, arts and kdelibs3 need it.
		// arts alternative is handled first, XFree86-GLX is selected
		// and added to candidates. Now at the time kdelibs3 is checked
		// XFree86-GLX is still in candidates, not vinstalled! -- ln
		else if (PkgSet::getRevRelforPkg(candidates->provided(),reqname).size() != 0) {
			PMSolvablePtr first_provider
				= PkgSet::getRevRelforPkg(candidates->provided(),reqname).front().pkg();
#ifdef phicode
			D__ << "Alternative for " << reqname << " already handled -- "
				 "adding reference from candiate " << cand->name() << " on "
				 << alt_info.req << " provided by "
				 << first_provider->name() << endl;
			add_referer( first_provider, cand, alt_info.req );
#endif
			D__ << "Alternative for " << reqname << " needed by " << cand->name()
				<< " already handled, put back to to_check" << endl;
			// well, now we know that the alternative was already
			// handled but we still don't know whether this
			// particular package is ok. I'll add it back to
			// to_check in the hope that this won't generate
			// loops. this has yet to be proofed -- ln
			to_check.push_back(cand);
		}
		else
		{
			D__ << "no package in vinstalled or candidates provides "
				<< reqname << " but the alternatives are already handled" << endl;
			// This branch should only be reached when no automatic
			// alternative selection is active (normal case). Since
			// the alternative was already handled must have
			// produced an error result. So we search for the
			// result and add us as referer. -- ln

			bool found = false;
			i_for( ErrorResultList::,, p, bad->, )
			{
				if( p->name == reqname )
				{
					found = true;
					add_referer(p->name, cand, alt_info.req );
				}
			}
			if(!found) INT << "already handled alternative " <<
				reqname << " not found in badlist" << endl;

			/* we don't generate this result to not flood the list
			 * with redundant information. the alternative error
			 * should already suffice
			ErrorResult res = alt_info.result;
			res.add_unresolvable( cand, alt_info.req );
			bad->push_back( res );
			*/

		}
		return;
	}

	D__ << "Handling alternative for " << reqname << " ";
	if (cand)
		D__ << "needed by " << cand->name() << " as \"Requires: "
			 << alt_info.req << "\"\nProviders are:\n";
	else
		D__ << " (all providers are self-conflicting candidates\n";

	ci_for( ,RevRelList_, alt, alt_info.providers., ) {
		D__ << "  " << alt->pkg()->name() << "-" << alt->pkg()->edition()
			 << " provides: " << alt->relation() << endl;
	}

	// check status of alternatives: an alt could 1) conflict with something,
	// 2) require more packages, or 3) nothing of the above
	hash<PkgName,alternative_kind> altkind;
	ci_for( ,RevRelList_, alt, alt_info.providers., ) {
		PkgName altname = alt->pkg()->name();
		D__ << "Checking consistency of alternative " << altname << ": ";
		ErrorResult err( *this, alt->pkg() );
		pkg_consistent( alt->pkg(), &err );
		if (err.conflicts_with.size()) {
			D__ << "conflict\n";
			altkind[altname] = CONFLICT;
		}
		else if (err.unresolvable.size()) {
			D__ << "requires-more\n";
			altkind[altname] = REQUIRES_MORE;
		}
		else {
			D__ << "simple\n";
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
			D__ << "Filtering defaults:\n";
			ci_for( Alternatives::AltDefaultList::,, def, defaults., ) {
				D__ << "  " << *def << ": ";
				if (!available.includes(*def)) {
					D__ << "not available -- skipping\n";
					continue;
				}
				PMSolvablePtr pkg = available[*def];
				bool found_match = false;
				ci_for( PMSolvable::,Provides_, prov, pkg->,all_provides_ ) {
					if (alt_info.req.matches( *prov )) {
						found_match = true;
						break;
					}
				}
				if (found_match) {
					D__ << "ok\n";
					useable_defaults.push_back( *def );
				}
				else
					D__ << "does not provide what we need -- skipping\n";
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
			D__ << "Filtering defaults:\n";
			ci_for( Alternatives::AltDefaultList::,, def, defaults., ) {
				D__ << "  " << *def << ": ";
				bool found = false;
				ci_for( ,RevRelList_, alt, alt_info.providers., ) {
					if (alt->pkg()->name() == *def) {
						found = true;
						break;
					}
				}
				if (found) {
					D__ << "ok, is an alternative\n";
					useable_defaults.push_back( *def );
				}
				else {
					D__ << "not ok, isn't one of the alternatives\n";
				}
			}
		}

		// Select the first default without a conflict (additional
		// requirements ignored here -- if it's a default, accept that we
		// maybe need more packages for it). If no conflict-free default
		// found, use the first one.
		ci_for( Alternatives::AltDefaultList::,, def, useable_defaults., ) {
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
		ci_for( ,RevRelList_, alt, alt_info.providers., ) {
			PkgName altname = alt->pkg()->name();
			if (altkind[altname] == SIMPLE) {
				use_pkg = alt->pkg();
				break;
			}
		}
		if (use_pkg)
			break;

		ci_for( ,RevRelList_, alt, alt_info.providers., ) {
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
		D__ << "Selected " << use_pkg->name() << " as alternative.\n";
		candidates->add( use_pkg, true );
		to_check.push_back( use_pkg );

		if (cand) {
			add_referer( use_pkg, cand, alt_info.req );
			D__ << "Candidate " << cand->name() << "seems ok, check again\n";
			to_check.push_back(cand);
			// putting this into good is definitively no good idea,
			// could have been an error result
			//good->push_back( alt_info.result );
			//vinstalled.add( cand, true );
		}
	}
	else {
		// no alternative auto-selected, let the caller decide, i.e. generate
		// an ErrorResult
		// the alternatives list is sorted by kind (SIMPLE, REQUIRES, CONFL)
		D__ << "No alternative selected -- generating ErrorResult\n";
		ErrorResult err(*this,reqname);
		ci_for( ,RevRelList_, alt, alt_info.providers., ) {
			PkgName altname = alt->pkg()->name();
			if (altkind[altname] == SIMPLE)
				err.add_alternative( alt->pkg(), altkind[altname] );
		}
		ci_for( ,RevRelList_, alt, alt_info.providers., ) {
			PkgName altname = alt->pkg()->name();
			if (altkind[altname] == REQUIRES_MORE)
				err.add_alternative( alt->pkg(), altkind[altname] );
		}
		ci_for( ,RevRelList_, alt, alt_info.providers., ) {
			PkgName altname = alt->pkg()->name();
			if (altkind[altname] == CONFLICT)
				err.add_alternative( alt->pkg(), altkind[altname] );
		}
		if (cand)
			add_referer( reqname, cand, alt_info.req );
		bad->push_back( err );

		if (cand) {
			D__ << "Candidate " << cand->name() << " failed\n";
			/* we don't generate this result to not flood the list
			 * with redundant information. the alternative error
			 * should already suffice
			ErrorResult res = alt_info.result;
			res.add_unresolvable( cand, alt_info.req );
			bad->push_back( res );
			*/
		}
		else {
			ci_for( ,RevRelList_, alt, alt_info.providers., ) {
				ErrorResult e(*this, alt->pkg());
				ci_for( ,RevRelList_, alt2, alt_info.providers., ) {
					if (alt2->pkg() != alt->pkg())
						e.add_conflict( alt2->pkg(), alt_info.req,
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
