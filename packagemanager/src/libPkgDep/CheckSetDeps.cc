/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       CheckSetDeps.cc

   Author:     Ludwig Nussel <lnussel@suse.de
   Maintainer: Ludwig Nussel <lnussel@suse.de

/-*/

#include <y2pm/CheckSetDeps.h>

using namespace std;

CheckSetDeps::CheckSetDeps(PkgSet& c, BrokenMap& b)
    : _c(c), _b(b), _track_packages(true), _track_relations(true)
{
}

void CheckSetDeps::setTrackPackages(bool y)
{
    _track_packages = y;
}

void CheckSetDeps::setTrackRelations(bool y)
{
    _track_relations = y;
}

bool CheckSetDeps::operator()(PMSolvablePtr p)
{
    PMSolvable::PkgRelList_type brokendeps;

    PMSolvable::PkgRelList_type::iterator req_it, req_end;

    for(req_it = p->requires_begin(), req_end = p->requires_end();
	req_it != req_end; ++req_it)
    {
	// ignore rpmlib dep
	if(req_it->name()->find("rpmlib(") != string::npos)
	    continue;

	const PkgSet::RevRelList_type& providers =
	    PkgSet::getRevRelforPkg(_c.provided(), req_it->name());

	bool matchfound = false;
	PkgSet::RevRelList_type::const_iterator rprov_it, rprov_end;
	for( rprov_it = providers.begin(), rprov_end = providers.end();
	    rprov_it != rprov_end; ++rprov_it)
	{
	    if(rprov_it->relation().matches(*req_it))
	    {
		matchfound = true;
		break;
	    }
	}
	if(!matchfound)
	{
	    if(brokendeps.empty() || _track_relations)
		brokendeps.push_back(*req_it);
	    if(!_track_relations)
		break;
	}
    }

    if(!brokendeps.empty())
    {
	if(_track_packages)
	    _b[p] = brokendeps; //XXX copies the list
	return false;
    }

    return true;
}

bool CheckSetDeps::checkAll()
{
    bool ok = true;

    PkgSet::iterator it, end;
    for(it = _c.begin(), end = _c.end(); it != end; ++it)
    {
	ok = (*this)(it->second);
	if(!_track_packages)
	    break;
    }
    return ok;
}
