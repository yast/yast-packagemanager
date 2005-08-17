#include <y2util/Y2SLog.h>

#include <y2pm/PkgDep.h>
#include <y2pm/PkgDep_private.h>
#include "PkgDep_int.h"
#include <functional>

using namespace std;

/* return true if a candidate obsoletes or replaces pkg */
bool PkgDep::P::is_obsoleted_by_candidate(PkgSet& candidates, PMSolvablePtr pkg, ErrorResult* res)
{
    if(!pkg)
	return false;

    bool ret = false;

    ci_for( PMSolvable::,Provides_, prov, pkg->,all_provides_ )
    {
	PMSolvablePtr repl = candidates.lookup((*prov).name());

	if(repl && repl != pkg)
	{
	    D__ << pkg->nameEd() << " replaced by candidate " << repl->nameEd()
		<< "because of provides " << *prov << endl;
	    if(res)
	    {
		res->add_conflict(repl,*prov,_dep, repl, pkg, RelInfo::OBSOLETION);
	    }
	    ret = true;
	}
	RevRel_for( PkgSet::getRevRelforPkg(candidates.obsoleted(),(*prov).name()), obs )
	{
	    if(obs->pkg() == pkg)
	    {
		continue; // we don't obsolete ourself
	    }
	
	    if (obs->relation().matches( *prov ))
	    {
		D__ << pkg->nameEd() << " obsoleted by candidate " << obs->pkg()->name() << endl;
		if(res)
		{
		    res->add_conflict(*obs, _dep, obs->pkg(), pkg, RelInfo::OBSOLETION);
		}
		ret = true;
	    }
	}
    }
    return ret;
}

/** Functor to remove packages from good list that were moved from installed to
 * candidates because of broken dependencies
 * */
class InconsistentSameAsInstalled
{
    private:
	PkgSet* _installed;
    public:
	InconsistentSameAsInstalled(PkgSet* installed) : _installed(installed) {};
	bool operator()(PkgDep::Result& result)
	{
	    if(_installed && !result.was_inconsistent.empty() && result.solvable != NULL)
	    {
		PMSolvablePtr p = _installed->lookup(result.name);
		if(p->edition() == result.solvable->edition())
		{
		    DBG << "don't reinstall inconsistent " << result.solvable->nameEd()
			<< " as same edition is already installed" << endl;
		    return true;
		}
	    }
	    return false;
	}
};

bool PkgDep::install( PkgSet& in_candidates,
			  ResultList& out_good,
			  ErrorResultList& out_bad,
			  ErrorResultList& out_obsoleted,
			  bool commit_to_installed,
			  bool check_inconsistent)
{
	out_good = ResultList();
	good = &out_good;
	out_bad = ErrorResultList();
	bad = &out_bad;
	candidates = &in_candidates;
	vinstalled = installed;
	to_check = deque<PMSolvablePtr >();
	notes = Notes_type();
	out_obsoleted = ErrorResultList();
	i_obsoleted = &out_obsoleted;
	unsigned numtocheck = 0;

	if(check_inconsistent)
		_dp->inconsistent_to_candidates();

	// sort out candidates that are already installed, mark others as
	// coming from input
	for(PkgSet::const_iterator c = candidates->begin(); c != candidates->end(); ++c)
	{
		PkgName candname = c->first;
		PMSolvablePtr cand = c->second;

		PMSolvablePtr instd = installed[candname];
		if (!_install_installed && instd && instd->edition() == cand->edition())
		{
			// is already installed in this version -- drop it
			D__ << candname << " is already installed in same version "
				 << cand->edition() << " -- dropping it\n";
		}
		else {
			++numtocheck;
#if 1
			bool pushfront=false;
			// ensure that packages that obsolete something
			// installed are checked first
			for( PMSolvable::PkgRelList_const_iterator obs = cand->obsoletes_begin();
			    obs != cand->obsoletes_end(); ++obs )
			{
				PMSolvablePtr p = vinstalled.lookup(obs->name());
				if (p && obs->matches( p->self_provides() )) 
				{
				    pushfront=true;
				    break;
				}
			}
			if(pushfront)
			{
			    D__ << cand->name() << " obsoletes installed package, moving to front" << endl;
			    to_check.push_front( cand );
			}   
			else
#endif
			{
			    to_check.push_back( cand );
			}
			if(notes[cand->name()].was_inconsistent.empty())
			{
			    notes[cand->name()].from_input = true;
			}
		}

	}

	DBG << numtocheck << " Packages to check" << endl;

	do {
		while( !to_check.empty() ) {
			PMSolvablePtr pkg = to_check.front();
			add_package( pkg ) ;
			to_check.pop_front();
		}

		while( !_dp->alts_to_check.empty() ) {
			const AltInfo& alt = _dp->alts_to_check.front();
			_dp->handle_alternative( alt );
			_dp->alts_to_check.pop_front();
		}
	} while( !to_check.empty() );

	// post processing: apply notes; on pkgs with conflicts test what would
	// have to be removed
	i_for( ResultList::,, p, good->, ) {
		if (notes.find(p->name) != notes.end())
			p->add_notes( notes[p->name] );
	}
	i_for( ErrorResultList::,, p, bad->, ) {
		if (notes.find(p->name) != notes.end())
			p->add_notes( notes[p->name] );
	}

	if(check_inconsistent)
	    good->remove_if(InconsistentSameAsInstalled(&installed));

	if (bad->empty() && commit_to_installed) {
		// if everything was ok, commit the candidates to the installed set
		ci_for( ErrorResultList::,, n, out_obsoleted., )
		{
			WAR << "obsolete package " << n->name << endl;
			installed.remove( n->name );
		}
		ci_for( PkgSet::,, c, candidates->, )
			installed.add( c->second, true );
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
    if(!cand)
    {
	INT << "got NULL" << endl;
	return;
    }
    
	PkgName candname = cand->name();
	if (!candidates->includes( candname ))
		return;

	bool delay = false;
	bool short_errors = false; // activate early error exit

	bool inconsistent = !notes[cand->name()].was_inconsistent.empty();

	ErrorResult res(*this,cand);
	D__ << "Checking candidate " << cand->nameEd() << endl;

	if(inconsistent)
	    goto skip_obsoletes_and_conflicts_check;

#if 0 // original phi code for reference
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
				i_obsoleted->push_back( oname );
				if (candidates->includes(oname)) {
					good->remove_if( ResultEqName(oname) );
					candidates->remove(oname);
				}
			}
		}
	}
#endif

	// check if the candidates obsoletes something already installed
	// if yes, check if requirements would be broken by the replacement
	// (conflict-by-obsoletion); otherwise, remove the obsoleted package from
	// vinstalled
	
#if 0
	// first check if we obsolete a fellow candidate. Since we don't
	// know the installation order we better disallow this in general.
	// TODO: other way around
	RevRel_for( PkgSet::getRevRelforPkg(candidates->provided(), obs->name()), prov2 )
	{
	    PMSolvablePtr p = prov2->pkg();
	    if(!p)
		{ INT << "got NULL" << endl; continue; }

	    if(cand->name() == p->name()) // that's ourself
		continue;

	    if (!obs->matches( prov2->relation() ))
		continue;

	    DBG << "candidate" << p->nameEd()
		<< " obsoleted by " << cand->nameEd()
		<< " via Obsoletes '" << *obs << "' and Provides '"
		<< prov2->relation() << "'"
		<< endl;

	    error = true;
	    res.add_conflict(cand,*obs,*this, p,NULL,RelInfo::OBSOLETION);
	}
#endif
	if(_dp->is_obsoleted_by_candidate(*candidates, cand, &res))
	    res._error = true;

	{
	deque<PMSolvablePtr> obsoletes;
	for( PMSolvable::PkgRelList_const_iterator obs = cand->obsoletes_begin();
	    obs != cand->obsoletes_end(); ++obs )
	{
	    // find a installed package that is target of the obsoletes
	    // rpmv4: provides do also matter
	    RevRel_for( PkgSet::getRevRelforPkg(vinstalled.provided(), obs->name()), prov )
	    {
		PMSolvablePtr p = prov->pkg();
		if(!p)
		    { INT << "got NULL" << endl; continue; }

		if(cand->name() == p->name()) // upgrade case handled later
		    continue;

		if(candidates->includes(p->name())) // already checked above
		    continue;

		if (!obs->matches( prov->relation() ))
		    continue;

		DBG << "installed/accepted " << p->nameEd()
		    << " obsoleted by " << cand->nameEd()
		    << " via Obsoletes '" << *obs << "' and Provides '"
		    << prov->relation() << "'"
		    << endl;

		// calls search_for_provider which adds packages!
		if (!check_for_broken_reqs( p, cand, res ))
		{
			res._error = true;
		}
		else
		{
		    D__ << "no broken reqs, removing " << p->nameEd() << " from vinstalled" << endl;

		    // we have to defer removing it from vinstalled
		    // otherwise the iterator would invalidate
		    // (i.e. segfault)
		    obsoletes.push_back(p);

		    ErrorResultList::iterator pos =
			find_if(
			    i_obsoleted->begin(),
			    i_obsoleted->end(), ResultEqName(p->name()));

		    if(pos == i_obsoleted->end())
		    {
			ErrorResult res(*this, p );
			res.add_conflict(cand,*obs,*this,NULL,NULL,RelInfo::OBSOLETION);
			i_obsoleted->push_back(res);
		    }
		    else
		    {
			pos->add_conflict(cand,*obs,*this,NULL,NULL,RelInfo::OBSOLETION);
		    }
		}
	    }
	}

	// if some installed package foo provides bar and we are going to
	// install a package called bar, rpmv4 will uninstall foo just as if it
	// was obsoleted
	RevRel_for( PkgSet::getRevRelforPkg(vinstalled.provided(),candname), prov )
	{
	    PMSolvablePtr p = prov->pkg();
	    if(!p)
		{ INT << "got NULL" << endl; continue; }

	    if(cand->name() == p->name()) // upgrade case handled later
		continue;

//	    if (!cand->self_provides().matches( prov->relation() ))
//	    no range check done by rpm
	    if(cand->name() != prov->relation().name())
		continue;

	    DBG << "installed/accepted " << p->nameEd()
		<< " replaced by " << cand->nameEd()
		<< " via provides '" << prov->relation()
		<< endl;

	    // calls search_for_provider which adds packages!
	    if (!check_for_broken_reqs( p, cand, res ))
	    {
		    res._error = true;
	    }
	    else
	    {
		D__ << "no broken reqs, removing " << p->nameEd() << " from vinstalled" << endl;

		// we have to defer removing it from vinstalled
		// otherwise the iterator would invalidate
		// (i.e. segfault)
		obsoletes.push_back(p);

		ErrorResultList::iterator pos =
		    find_if(
			i_obsoleted->begin(),
			i_obsoleted->end(), ResultEqName(p->name()));

		if(pos == i_obsoleted->end())
		{
		    ErrorResult res(*this, p );
		    res.add_conflict(cand,cand->self_provides(),*this,NULL,NULL,RelInfo::OBSOLETION);
		    i_obsoleted->push_back(res);
		}
		else
		{
		    pos->add_conflict(cand,cand->self_provides(),*this,NULL,NULL,RelInfo::OBSOLETION);
		}
	    }

	}


	while(!obsoletes.empty())
	{
	    PMSolvablePtr p = obsoletes.front();
	    vinstalled.remove( p ); // we may have duplicates but that doesn't matter
	    obsoletes.pop_front();
	}
	}

#if 0 // no longer needed as we check against obsoletes of fellow candidates now

	// check if the candidate is obsoleted by an installed package. rpm
	// does not check this, but we better do it (#31001). This code must be
	// after the check if the candidate obsoletes installed packages to not
	// raise useless conflicts.
	// if this is triggered during update, the reason is  a packaging bug.
	// Well, at least in theory. Practically, it depends on the order in
	// which packages are beeing checked. e.g. kdemultimedia3-sound is
	// installed and obsoletes kdemultimedia3. Now updating
	// kdemultimedia3-sound to a version that doesn't have the obsoletes
	// and installing a new kdemultimedia3 raises a conflict by obsoletion
	// if kdemultimedia3 is checked first.
	RevRel_for( PkgSet::getRevRelforPkg(vinstalled.obsoleted(),candname), obs ) {
		if (obs->relation().matches( cand->self_provides() )) {
			WAR << "candidate " << candname << " is obsoleted by installed "
				 << obs->pkg()->name() << endl;
			WAR << "check #31001, better rename the package" << endl;
			if(candname == obs->pkg()->name())
			{
			    ERR << obs->pkg()->nameEd() << " obsoletes itself!" << endl;
			}
			error = true;

			// confirm assume_installed == cand instead of NULL?
			res.add_conflict(obs->pkg(),obs->relation(),*this,obs->pkg(),NULL,RelInfo::OBSOLETION);
		}
	}
#endif


	if (res._error && short_errors)
	{
	    goto add_package_error_out;
	}

	// 17.09.2003 ln -- moved conflicts checks after obsoletes check to not
	// complain about packages that are conflicted and obsoleted.
	// (bind9-utils vs bind-utils case #31031)
	//
	// check if something already installed conflicts with the new package
	ci_for( PMSolvable::,Provides_, prov, cand->,all_provides_ ) {
		RevRel_for( PkgSet::getRevRelforPkg(vinstalled.conflicted(),(*prov).name()), confl ) {
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
					res._error = true;
				}
			}
		}
	}

	if (res._error && short_errors)
	{
	    // ln -- added more error outputs, should avoid e.g. prompting the
	    // user to choose alternatives while it would not be necessary when
	    // he decides to not install the problematic package at all
	    goto add_package_error_out;
	}

	// check if the new package conflicts with something installed
	ci_for( PMSolvable::,PkgRelList_, confl, cand->,conflicts_ ) {
		// confl is now a PkgRelation

		RevRel_for( PkgSet::getRevRelforPkg(vinstalled.provided(),confl->name()), prov ) {
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
					res._error = true;
				}
			}
		}
		// or does it conflict with a fellow candidate? Make a list of all
		// candidates that provide and conflict with that name, they're
		// treated like alternatives
		RevRelList alternatives;
		bool self_conflict = false;
		// for all packages that provide what we conflict with
		RevRel_for( PkgSet::getRevRelforPkg(candidates->provided(),confl->name()), prov1 ) {
			if (confl->matches( prov1->relation() ) &&
				has_conflict_with( prov1->relation(), prov1->pkg() ))
			{
				D__ << "fellow conflict/provider found: " << prov1->pkg()->name() << endl;
				alternatives.push_back( *prov1 );
				if (prov1->pkg() == cand)
					self_conflict = true;
			}
		}
		if (self_conflict && alternatives.size() > 1) {
			D__ << "There are multiple alternatives that Provide and Conflict "
				 "with " << confl->name() << " -- handling that later\n";
			_dp->alts_to_check.push_back( AltInfo(NULL, *confl, alternatives, res));
			ci_for( ,RevRelList_, alt, alternatives., ) {
				candidates->remove( alt->pkg() );
			}
			return;
		}
	}
	if (res._error && short_errors)
	{
	    goto add_package_error_out;
	}

	// check if the requirements of an installed or an already accepted
	// candidate package would be broken by installing the candidate
	// (conflict-by-upgrade)
	if (vinstalled.includes(candname)) {
		D__ << "different version of " << candname << " to be installed -- "
			 "checking for conflict-by-upgrade\n";
		// calls search_for_provider which adds packages!
		if (!check_for_broken_reqs( vinstalled[candname], cand, res ))
			res._error = true;
	}
	if (res._error && short_errors)
	{
	    goto add_package_error_out;
	}

skip_obsoletes_and_conflicts_check: // for inconsistent

	// then check if requirements are present; if not, try to find them in the
	// available set
	ci_for( PMSolvable::,PkgRelList_, req, cand->,requires_ ) {
		PkgName reqname = req->name();

		if(_dp->ignoredRequirements.isIgnored(*req, cand))
		{
		    goto next_requirement;
		}

		// If a matching version is going to be installed (i.e., is a fellow
		// candidate) consider the requirement is satisfied for now.
		// (Candidates must be checked first, because they can replace
		// installed packages.)
		{
		    // don't break. we check all candidates to get all referers (lsb -> lprng|cups)
		    bool satisfied = false;
		    RevRel_for( PkgSet::getRevRelforPkg(candidates->provided(),reqname), prov )
		    {
			if (req->matches( prov->relation() ))
			{
			    satisfied = true;
			    D__ << "Candidate " << prov->pkg()->name() << " provides "
				     << prov->relation() << " which is needed by "
				     << candname << " (Requires: " << *req << ")\n";
			    // add referer only if no installed pkg can
			    // provide this and it's no self-provide
			    if (!also_provided_by_installed( *req ) && prov->pkg() != cand)
				    add_referer( prov->pkg(), cand, *req );
			}
		    }
		    if(satisfied)
			goto next_requirement;
		}

		// also ok if a matching version is provided by an installed package
		{
			RevRel_for( PkgSet::getRevRelforPkg(vinstalled.provided(),reqname), prov ) {
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
			res._error = true;
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
	else if (res._error) {
add_package_error_out:
		D__ << "Candidate " << candname << " failed\n";
		bad->push_back( res );
	}
	else {
		D__ << "Candidate " << candname << " ok\n";
		good->push_back( res );
		vinstalled.add( cand, true );
	}
}

/** search the available set for packages that provide "req" which is needed by
 * "referer"
 * */
PkgDep::search_result
PkgDep::search_for_provider( const PkgRelation& req, PMSolvablePtr referer,
							 ErrorResult *res )
{
	RevRelList providers;
	set<PMSolvablePtr > seen;

	RevRel_for( PkgSet::getRevRelforPkg(available.provided(),req.name()), prov ) {
		if (req.matches( prov->relation() )) {
			D__ << "Available " << prov->pkg()->name() << " provides "
				 << prov->relation() << " which is needed by "
				 << referer->name() << " (Requires: " << req << ")" << endl;

			if (seen.find(prov->pkg()) != seen.end())
				continue;
			seen.insert(prov->pkg());

			// search_for_provider is called when no
			// candidate or installed provides the
			// relation, so when there is 
			PMSolvablePtr instd = vinstalled[prov->pkg()->name()];
			if(instd && instd->doesProvide(prov->relation()))
			{
				W__ << "Installed " << instd->nameEd()
					<< " provides it too, obsolete? -- skipping" << endl;
			}
			else
			{
				providers.push_back( *prov );
			}
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
		// ln -- force used to be true, I don't think it makes sense
		candidates->add( provider );
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
		_dp->alts_to_check.push_back( AltInfo( referer, req, providers,
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

/** resolve broken requirements when replacing oldpkg by newpkg */
bool PkgDep::check_for_broken_reqs( PMSolvablePtr oldpkg, PMSolvablePtr newpkg, ErrorResult &res )
{
	D__ << "check if replacing " << oldpkg->nameEd() << " by " << newpkg->nameEd() << " doesn't break anything" << endl;

	// for all provides of the old package
	ci_for( PMSolvable::,Provides_, prov, oldpkg->,all_provides_)
	{
		D__ << "  checking provided name " << (*prov).name() << endl;
		// for all packages that require this provide
		RevRel_for( PkgSet::getRevRelforPkg(vinstalled.required(),(*prov).name()), req )
		{
			D__ << "    requirement: " << req->relation()
				 << " by installed/accepted " << req->pkg()->name() << endl;


			if(_dp->ignoredRequirements.isIgnored(req->relation(), req->pkg()))
			{
			    D__ << "    ignored" << endl;
			    continue;
			}

			if(oldpkg == req->pkg())
			{
			    D__ << "    satisfied, requires it's own provides" << endl;
			    continue;
			}

#if 0
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
#endif

			if(_dp->is_obsoleted_by_candidate(*candidates,req->pkg()))
			{
			    continue;
			}

			
			bool obsdoesntmatter = false;
			// check if newpkg replaces found package via name
			for(PMSolvable::PkgRelList_const_iterator repl = req->pkg()->provides_begin();
				repl != req->pkg()->provides_end(); ++repl)
			{
			    if (repl->matches( newpkg->self_provides() ))
			    {
				W__ << "    satisfied as " << newpkg->name() <<
					 " obsoletes " << *repl << endl;
				obsdoesntmatter = true;
				break;
			    }
			}

			if(obsdoesntmatter)
			    continue;

			if (!req_ok_after_upgrade( req->relation(), oldpkg, newpkg ))
			{
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
					res.add_conflict( *req, *this, req->pkg(), newpkg, RelInfo::REQUIREMENT );
					res._error = true;
				}
			}
		}
	}
	return !res._error;
}

/** check if the requirement rel is satisfied when replacing oldpkg by newpkg.
 * This is the case when newpkg, a candidate or an otherwise installed package
 * provides it
 * */
bool PkgDep::req_ok_after_upgrade( const PkgRelation& rel, PMSolvablePtr oldpkg, PMSolvablePtr newpkg )
{
	// check if newpkg satisfies the requirement
	ci_for( PMSolvable::,Provides_, prov, newpkg->,all_provides_) {
		if ((*prov).name() == rel.name() && (*prov).matches( rel )) {
			D__ << "    satisfied by upgrade " << newpkg->name() <<
				 " with Provides: " << *prov << endl;
			return true;
		}
	}

	// check if a candidate satisfies it
	RevRel_for( PkgSet::getRevRelforPkg(candidates->provided(),rel.name()), prov1 ) {
		if (prov1->relation().matches( rel )) {
			D__ << "    satisfied by candidate " << prov1->pkg()->name()
				 << " with Provides: " << prov1->relation() << endl;
			return true;
		}
	}

	// check if an installed Solvable satifies it
	// FIXED: vinstalled instead of installed
	RevRel_for( PkgSet::getRevRelforPkg(vinstalled.provided(),rel.name()), prov2 ) {
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

// vim:sw=4
