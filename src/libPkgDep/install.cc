#include <y2util/Y2SLog.h>

#include <y2pm/PkgDep.h>
#include <y2pm/PkgDep_int.h>
#include <functional>

using namespace std;

bool PkgDep::install( PkgSet& in_candidates,
					  ResultList& out_good, ErrorResultList& out_bad,
					  bool commit_to_installed )
{
	out_good = ResultList();
	good = &out_good;
	out_bad = ErrorResultList();
	bad = &out_bad;
	candidates = &in_candidates;
	vinstalled = installed;
	to_check = deque<PMSolvablePtr >();
	notes = Notes_type();
	i_obsoleted = NameList();
	unsigned numtocheck = 0;

	// sort out candidates that are already installed, mark others as
	// coming from input
	ci_for( PkgSet::,, c, candidates->, ) {
		bool error = false;
		PkgName candname = c->key;
		PMSolvablePtr cand = c->value;

		PMSolvablePtr instd = installed[candname];
		if (!_install_installed && instd && instd->edition() == cand->edition())
		{
			// is already installed in this version -- drop it
			D__ << candname << " is already installed in same version "
				 << cand->edition() << " -- dropping it\n";
		}
		else {
			// check if the candidate is the target of an obsoletes; if yes, drop it
			RevRel_for( vinstalled.obsoleted()[candname], obs ) {
				if (obs->relation().matches( cand->self_provides() )) {
					WAR << "candidate " << candname << " is obsoleted by "
						 << obs->pkg()->name() << " -- dropping it\n";
					if(candname == obs->pkg()->name())
					{
					    ERR << obs->pkg()->nameEd() << " obsoletes itself!" << endl;
					}
					error = true;
				}
			}
			if (error)
				continue;

			/* we don't do that, we just remove them from vinstalled
			 *
			// check if the candidates obsoletes something already installed
			// if yes, check if requirements would be broken by the replacement
			// (conflict-by-obsoletion); otherwise, remove the obsoleted package from
			// vinstalled
			ci_for( PMSolvable::,PkgRelList_, obs, cand->,obsoletes_ ) {
				PkgName oname = obs->name();
				if (vinstalled.includes(oname)) {
					W__ << "installed/accepted " << oname << " obsoleted by "
						 << candname << " -- checking for conflict-by-obsoletion\n";
					if (!check_for_broken_reqs( vinstalled[oname], cand, res ))
						error = true;
					else {
						vinstalled.remove( oname );
						i_obsoleted.push_back( oname );
						if (candidates->includes(oname)) {
							good->remove_if( ResultEqName(oname) );
							candidates->remove(oname);
						}
					}
				}
			}
			*/

			for( PMSolvable::PkgRelList_const_iterator obs = cand->obsoletes_begin();
			    obs != cand->obsoletes_end(); ++obs )
			{
				PkgName oname = obs->name();
				PMSolvablePtr p = vinstalled.lookup(oname);
				if (p && obs->matches( p->self_provides() )) {
					WAR << "installed " << oname << " obsoleted by " << candname << endl;
					vinstalled.remove( oname );
				}
			}

			numtocheck++;
			to_check.push_back( cand );
			notes[cand->name()].from_input = true;
		}

	}


	DBG << numtocheck << " Packages to check" << endl;

	do {
		while( !to_check.empty() ) {
			PMSolvablePtr pkg = to_check.front();
			add_package( pkg );
			to_check.pop_front();
		}

		while( !alts_to_check.empty() ) {
			const AltInfo& alt = alts_to_check.front();
			handle_alternative( alt );
			alts_to_check.pop_front();
		}
	} while( !to_check.empty() );

	// post processing: apply notes; on pkgs with conflicts test what would
	// have to be removed
	i_for( ResultList::,, p, good->, ) {
		if (notes.exists(p->name))
			p->add_notes( notes[p->name] );
	}
	i_for( ErrorResultList::,, p, bad->, ) {
		if (notes.exists(p->name))
			p->add_notes( notes[p->name] );
	}

	if (bad->empty() && commit_to_installed) {
		// if everything was ok, commit the candidates to the installed set
		ci_for( NameList::,, n, i_obsoleted., )
		{
			WAR << "obsolete package " << *n << endl;
			installed.remove( *n );
		}
		ci_for( PkgSet::,, c, candidates->, )
			installed.add( c->value, true );
	}

	return bad->empty();
}


// predicate class for finding results for a PkgName
struct ResultEqName {
	PkgName name;
	ResultEqName( PkgName n ) : name(n) {}
	bool operator() ( const PkgDep::Result& res ) { return res.name == name; }
};

void PkgDep::add_package( PMSolvablePtr cand )
{
	PkgName candname = cand->name();
	if (!candidates->includes( candname ))
		return;

	bool error = false;
	bool delay = false;
	ErrorResult res(*this,cand);
	D__ << "Checking candidate " << candname << endl;


	// first check if something already installed conflicts with the new
	// package
	ci_for( PMSolvable::,Provides_, prov, cand->,all_provides_ ) {
		RevRel_for( vinstalled.conflicted()[(*prov).name()], confl ) {
			if (confl->relation().matches( *prov )) {
				D__ << "Conflict of candidate " << *prov
					 << " provided by " << candname
					 << " with \"Conflicts: " << confl->relation()
					 << "\" by " << confl->pkg()->name() << endl;
				if (PMSolvablePtr upgrade =
					try_upgrade_conflictor( confl->pkg(), *prov )) {
					do_upgrade_for_conflict( upgrade );
				}
				else {
					res.add_conflict( *confl, *this, confl->pkg(), cand );
					error = true;
				}
			}
		}
	}

	// check if the new package conflicts with something installed
	ci_for( PMSolvable::,PkgRelList_, confl, cand->,conflicts_ ) {
		RevRel_for( vinstalled.provided()[confl->name()], prov ) {
			if (confl->matches( prov->relation() )) {
				D__ << "Conflict of installed/accepted " << prov->relation()
					 << " provided by " << prov->pkg()->name()
					 << " with \"Conflicts: " << *confl
					 << "\" by " << candname << endl;
				if (PMSolvablePtr upgrade =
					try_upgrade_conflicted( prov->pkg(), *confl )) {
					do_upgrade_for_conflict( upgrade );
				}
				else {
					res.add_conflict( cand, *confl,
									  *this, prov->pkg(), cand );
					error = true;
				}
			}
		}
		// or does it conflict with a fellow candidate? Make a list of all
		// candidates that provide and conflict with that name, they're
		// treated like alternatives
		RevRelList alternatives;
		bool self_conflict = false;
		RevRel_for( candidates->provided()[confl->name()], prov1 ) {
			if (confl->matches( prov1->relation() ) &&
				has_conflict_with( prov1->relation(), prov1->pkg() )) {
				alternatives.push_back( *prov1 );
				if (prov1->pkg() == cand)
					self_conflict = true;
			}
		}
		if (self_conflict && alternatives.size() > 1) {
			D__ << "There are multiple alternatives that Provide and Conflict "
				 "with " << confl->name() << " -- handling that later\n";
			alts_to_check.push_back( AltInfo(NULL, *confl, alternatives, res));
			ci_for( ,RevRelList_, alt, alternatives., ) {
				candidates->remove( alt->pkg() );
			}
			return;
		}
	}

	// check if the requirements of an installed or an already accepted
	// candidate package would be broken by installing the candidate
	// (conflict-by-upgrade)
	if (vinstalled.includes(candname)) {
		D__ << "different version of " << candname << " to be installed -- "
			 "checking for conflict-by-upgrade\n";
		if (!check_for_broken_reqs( vinstalled[candname], cand, res ))
			error = true;
	}

	// then check if requirements are present; if not, try to find them in the
	// available set
	ci_for( PMSolvable::,PkgRelList_, req, cand->,requires_ ) {
		PkgName reqname = req->name();

/*
		if (!strncmp(req->name(),"rpmlib(",strlen("rpmlib(")))
			continue; */

		// ignore rpmlib requirements
//		if(req->name()->find("rpmlib(") != string::npos)
//		    continue;


		// If a matching version is going to be installed (i.e., is a fellow
		// candidate) consider the requirement is satisfied for now.
		// (Candidates must be checked first, because they can replace
		// installed packages.)
		RevRel_for( candidates->provided()[reqname], prov ) {
			if (req->matches( prov->relation() )) {
				D__ << "Candidate " << prov->pkg()->name() << " provides "
					 << prov->relation() << " which is needed by "
					 << candname << " (Requires: " << *req << ")\n";
				// add referer only if no installed pkg can
				// provide this and it's no self-provide
				if (!also_provided_by_installed( *req ) && prov->pkg() != cand)
					add_referer( prov->pkg(), cand, *req );
				goto next_requirement;
			}
		}

		// also ok if a matching version is provided by an installed package
		{
			RevRel_for( installed.provided()[reqname], prov ) {
				if(prov->pkg()->name()==candname)
				{
					W__ << "ignoring old version of " << candname << " for provides check" << endl;
					continue;
				}
				if (req->matches( prov->relation() )) {
					D__ << "Installed " << prov->pkg()->name() << " provides "
						 << prov->relation() << " which is needed by "
						 << candname << " (Requires: " << *req << ")\n";
					goto next_requirement;
				}
			}
		}

		// If we come here, no package installed or to be installed provides
		// what we need. This means that some has additionally to be
		// installed. Let's check the available list.
		switch( search_for_provider( *req, cand, &res )) {
		  case NONE:
			error = true;
			break;
		  case ONE:
			break;
		  case MULTI:
			delay = true;
			break;
		}

	  next_requirement:
		;
	}

	if (delay) {
		D__ << "Candidate " << candname << " delayed\n";
	}
	else if (error) {
		D__ << "Candidate " << candname << " failed\n";
		bad->push_back( res );
	}
	else {
		D__ << "Candidate " << candname << " ok\n";
		good->push_back( res );
		vinstalled.add( cand, true );
	}
}

// FIXME
#warning make better hash function

size_t hashfun( PMSolvablePtr & pkg ){
        return size_t((const void*)pkg);
}

size_t hashfun( const PMSolvablePtr & pkg ){
        return size_t((const void*)pkg);
}

PkgDep::search_result
PkgDep::search_for_provider( const PkgRelation& req, PMSolvablePtr referer,
							 ErrorResult *res )
{
	RevRelList providers;
	noval_hash<PMSolvablePtr > seen;

	RevRel_for( available.provided()[req.name()], prov ) {
		if (seen.exists(prov->pkg()))
			continue;
		seen.insert(prov->pkg());
		if (req.matches( prov->relation() )) {
			D__ << "Available " << prov->pkg()->name() << " provides "
				 << prov->relation() << " which is needed by "
				 << referer->name() << " (Requires: " << req << ")\n";
			providers.push_back( *prov );
		}
	}

	if (providers.empty())
	{
	    PMSolvablePtr ptr = NULL;
	    WhatToDoWithUnresolvable what = _unresolvable_callback(this, req, ptr);
	    switch(what)
	    {
		case UNRES_IGNORE:
		    return ONE;
		case UNRES_TAKETHIS:
		    if(ptr != NULL)
		    {
			if( !candidates->includes(ptr->name()) && !vinstalled.includes(ptr->name()))
			{
			    providers.push_back(PkgRevRelation(NULL,ptr));
			}
			else
			{
			    return ONE;
			}
		    }
		    else
			ERR << "solvable is NULL" << endl;
		    break;
		case UNRES_FAIL:
		    break;
	    }
	}


	if (providers.empty()) {
		// nothing provides what we need -- this candidate is
		// unresolvable
		D__ << "No providers found for " << req.name()
			 << " which is needed by " << referer->name()
			 << " (Requires: " << req << ")\n";
		if (res) {
			D__ << "add_unres( " << referer->name() << ", " << req << ")\n";
			res->add_unresolvable( referer, req );
			add_not_available( referer, req );
		}
		return NONE;
	}
	else if (providers.size() == 1) {
		// exactly one package satisfies our requirement -> add it
		PMSolvablePtr provider = providers.front().pkg();
		D__ << "Exactly one provider for " << req
			 << " found, adding " << provider->name()
			 << " as candidate\n";
		candidates->add( provider, true );
		to_check.push_back( provider );
		// add referer only if no installed pkg can provide this
		if (!also_provided_by_installed( req ))
			add_referer( provider, referer, req );
		if (!res)
			notes[provider->name()].install_to_avoid_break = true;
		return ONE;
	}
	else {
		// more than one provider, i.e. we have alternatives to choose
		// from save this for later when all needed packages are
		// known, because then we can prefer alternatives that don't
		// require even more stuff
		D__ << "More than one provider for " << req << " found\n";
		alts_to_check.push_back( AltInfo( referer, req, providers,
										  res ? *res : ErrorResult(*this,referer) ));
		if (!res) {
			notes[req.name()].install_to_avoid_break = true;
			ci_for( ,RevRelList_, alt, providers., ) {
				notes[alt->pkg()->name()].install_to_avoid_break = true;
			}
		}
		return MULTI;
	}
}

bool PkgDep::check_for_broken_reqs( PMSolvablePtr oldpkg,
									PMSolvablePtr newpkg,
									ErrorResult &res )
{
	bool error = false;

	// for all provides of the old package
	ci_for( PMSolvable::,Provides_, prov, oldpkg->,all_provides_) {
		D__ << "  checking provided name " << (*prov).name() << endl;
		// for all packages that require this provide
		RevRel_for( vinstalled.required()[(*prov).name()], req ) {
			bool obsdoesntmatter = false;
			D__ << "    requirement: " << req->relation()
				 << " by installed/accepted " << req->pkg()->name() << endl;

			// check if newpkg obsoletes found package, in this
			// case requirements of the found one do not matter
			ci_for( PMSolvable::,PkgRelList_, obs, newpkg->,obsoletes_)
			{
				if (obs->matches( req->pkg()->self_provides() ))
				{
					W__ << "    satisfied as " << newpkg->name() <<
						 " obsoletes " << *obs << endl;
					obsdoesntmatter = true;
				}
			}

			if (!obsdoesntmatter && !req_ok_after_upgrade( req->relation(), oldpkg, newpkg )) {
				if (PMSolvablePtr upgrade = try_upgrade_requirerer(
						req->pkg(), oldpkg, newpkg )) {
					do_upgrade_for_conflict( upgrade );
				}
				else if (search_for_provider( req->relation(), req->pkg(),
											  NULL ) != NONE) {
					D__ << "Could solve broken requirement " << req->relation()
						 << " by " << req->pkg()->name() << " by installing "
						 << "provider for " << req->relation().name() << endl;
				}
				else {
					D__ << "Requirement " << req->relation()
						 << " of installed/accepted "
						 << req->pkg()->name() << " would be broken by "
						 << " replacing " << oldpkg->name() << "-"
						 << oldpkg->edition() << " by " << newpkg->name()
						 << "-" << newpkg->edition()
						 << " (old provided " << *prov << ")\n";
					res.add_conflict( *req, *this, req->pkg(), newpkg, false );
					error = true;
				}
			}
		}
	}
	return !error;
}

bool PkgDep::req_ok_after_upgrade( const PkgRelation& rel,
								   PMSolvablePtr oldpkg,
								   PMSolvablePtr newpkg )
{
	// check if newpkg satisfies the requirement
	ci_for( PMSolvable::,Provides_, prov, newpkg->,all_provides_) {
		if ((*prov).name() == rel.name() && (*prov).matches( rel )) {
			D__ << "    satisfied by upgrade " << newpkg->name() <<
				 " with Provides: " << *prov << endl;
			return true;
		}
	}

	// check if another candidate satifies it
	RevRel_for( candidates->provided()[rel.name()], prov1 ) {
		if (prov1->relation().matches( rel )) {
			D__ << "    satisfied by candidate " << prov1->pkg()->name()
				 << " with Provides: " << prov1->relation() << endl;
			return true;
		}
	}

	// check if an installed Solvable satifies it
	RevRel_for( installed.provided()[rel.name()], prov2 ) {
		// skip oldpkg (which is to be replaced) and packages that are
		// candidates
		if (prov2->pkg() == oldpkg ||
			candidates->includes(prov2->pkg()->name()))
			continue;
		if (prov2->relation().matches( rel )) {
			D__ << "    satisfied by installed " << prov2->pkg()->name()
				 << " with Provides: " << prov2->relation() << endl;
			return true;
		}
	}

	return false;
}


// Local Variables:
// c-basic-offset: 4
// tab-width: 4
// End:
