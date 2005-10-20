/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2004 SUSE AG |
\----------------------------------------------------------------------/

   File:       PkgDep_private.h
   Purpose:    Private solver classes. Do not use!
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#ifndef _PkgDep_private_h
#define _PkgDep_private_h

#include <y2pm/PkgDep.h>
#include <y2pm/IgnoredRelations.h>

#define Y2PMHIDDEN __attribute__ ((visibility("hidden")))
#define Y2PMINTERNAL __attribute__ ((visibility("internal")))

class PkgDep::P
{
    private:
	PkgDep& _dep;

    public:
	IgnoredRequirements ignoredRequirements;

	bool pkg_consistent( PMSolvablePtr pkg, PkgDep::ErrorResult *err ) Y2PMHIDDEN;
	bool is_obsoleted_by_candidate(PkgSet& candidates, PMSolvablePtr instd, ErrorResult* res = NULL) Y2PMHIDDEN;

	/** check consistency of installed packages, mark inconsistent ones for
	 * reinstallation */
	void inconsistent_to_candidates() Y2PMHIDDEN;

	/* alternative handling */
	AltInfoList       alts_to_check;
	std::set<PkgName> alts_handled;
	alternatives_mode alt_mode;

	void handle_alternative( const AltInfo& alt_info ) Y2PMHIDDEN;

	P(PkgDep& dep) : _dep(dep)
	{
	}
};

#endif
