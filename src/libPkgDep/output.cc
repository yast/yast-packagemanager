/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       output.cc
   Purpose:    various << operators for PkgDep classes
   Author:     Roman Hodek
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#include <y2util/Y2SLog.h>
#include <y2pm/PkgDep.h>
#include "PkgDep_int.h"
#include <iostream>

using namespace std;

ostream& operator<<( ostream& os, const PkgDep::Result& res )
{
	os << "Name: " << res.name;
	if(res.solvable == NULL) os << "(*)";
	os << endl;
	os << "Edition: " << res.edition << endl;
	if (res.from_input_list)
		os << "From-Input-List: yes\n";
	if (!res.referers.empty())
		os << "Referers: " << res.referers << endl;
	if (!res.is_upgrade_from.is_unspecified())
		os << "Is-Upgrade-From: " << res.is_upgrade_from << endl;
	if (!res.is_downgrade_from.is_unspecified())
		os << "Is-Downgrade-From: " << res.is_downgrade_from << endl;
	if (res.upgrade_to_remove_conflict)
		os << "Upgrade-To-Remove-Conflict: yes\n";
	if (res.install_to_avoid_break)
		os << "Install-To-Avoid-Break: yes\n";

	return os;
}


ostream& operator<<( ostream& os, const PkgDep::ErrorResult& res )
{
	os << (PkgDep::Result&)res;

	if (res.not_available)
		os << "Not-Available: " << res.not_avail_range << endl;
	if (!res.unresolvable.empty())
		os << "Unresolvable: " << res.unresolvable << endl;
	if (!res.alternatives.empty())
		os << "Alternatives: " << res.alternatives << endl;
	if (!res.conflicts_with.empty())
		os << "Conflicts-With: " << res.conflicts_with << endl;
	if (!res.remove_to_solve_conflict.empty())
	{
		os << "Remove-To-Solve-Conflict: ";
		ci_for( PkgDep::SolvableList::,, it, res.remove_to_solve_conflict.,)
		{
		    if(it != res.remove_to_solve_conflict.begin())
		    {
			os << ", ";
		    }
		    os << (*it)->name();
		}
		os << endl;
	}
	if (res.state_change_not_possible)
		os << "State-Change-Not-Possible: yes" << endl;

	return os;
}

ostream& operator<<( ostream& os, const PkgDep::RelInfoList& rl )
{
	for( PkgDep::RelInfoList_const_iterator p = rl.begin();
		 p != rl.end(); ++p ) {
		if (p != rl.begin())
			os << ", ";

		if(p->solvable == NULL)
		{
		    os << p->name << "(*)";
		}
		else
		{
		    if(p->name != p->solvable->name())
		    {
			INT << "names dont match" << endl;
		    }
		    os << p->solvable->nameEd();
		}
		switch(p->kind)
		{
			case PkgDep::RelInfo::REQUIREMENT:
				os << " requires ";
				break;
			case PkgDep::RelInfo::CONFLICT:
				os << " conflicts with ";
				break;
			case PkgDep::RelInfo::OBSOLETION:
				os << " obsoletes ";
				break;
		}
		os << p->rel;
	}
	return os;
}

ostream& operator<<( ostream& os, const list<PkgDep::Alternative>& al )
{
	for( list<PkgDep::Alternative>::const_iterator p = al.begin();
		 p != al.end(); ++p ) {
		if (p != al.begin())
			os << ", ";
		os << p->solvable->name() << "("
		   << (p->kind == PkgDep::SIMPLE ? 'S' :
			   p->kind == PkgDep::REQUIRES_MORE ? 'D' : 'C') << ")";
	}
	return os;
}

ostream& operator<<( ostream& os, const list<PkgName>& nl )
{
	for( list<PkgName>::const_iterator p = nl.begin();
		 p != nl.end(); ++p ) {
		if (p != nl.begin())
			os << "/";
		os << *p;
	}
	return os;
}

ostream& operator<<( ostream& os, const PkgDep::NeededEditionRange& range )
{
	if (range.allows_any())
		os << "any";
	else if (range.less.is_unspecified())
		os << (range.greater_incl ? ">=" : ">") << " " << range.greater;
	else if (range.greater.is_unspecified())
		os << (range.less_incl ? "<=" : "<") << " " << range.less;
	else
		os << (range.greater_incl ? '[' : '(') << range.greater << ','
		   << range.less << (range.less_incl ? "]" : ")")
		   << (range.impossible() ? " (impossible)" : "");
	return os;
}

// Local Variables:
// tab-width: 4
// End:
