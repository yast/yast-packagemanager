#include <y2pm/PkgDep.h>
#include <iostream>

using namespace std;

ostream& operator<<( ostream& os, const PkgDep::Result& res )
{
	os << "Name: " << res.name << endl;
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
		os << "Remove-To-Solve-Conflict: " << res.remove_to_solve_conflict
		   << endl;

	return os;
}

ostream& operator<<( ostream& os, const PkgDep::RelInfoList& rl )
{
	for( PkgDep::RelInfoList_const_iterator p = rl.begin();
		 p != rl.end(); ++p ) {
		if (p != rl.begin())
			os << ", ";
		os << p->name << (p->is_conflict ? " conflicts " : " requires ")
		   << p->rel;
	}
	return os;
}

ostream& operator<<( ostream& os, const list<PkgDep::Alternative>& al )
{
	for( list<PkgDep::Alternative>::const_iterator p = al.begin();
		 p != al.end(); ++p ) {
		if (p != al.begin())
			os << ", ";
		os << p->name << "("
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
			os << ", ";
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
