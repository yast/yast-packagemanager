#include <PkgDep.h>
#include <PkgDep_int.h>

using namespace std;

PkgDep::Result::Result( const PkgDep& pkgdep, const Solvable *pkg )
{
	PkgName n = pkg->name();
	name = n;
	edition = pkg->edition();
	upgrade_to_remove_conflict = false;
	install_to_avoid_break = false;
	from_input_list = false;

	if (pkgdep.installed.includes(n)) {
		const Solvable *ipkg = pkgdep.installed[n];
		is_upgrade_from = (ipkg->edition() < pkg->edition())
						  ? ipkg->edition()
						  : PkgEdition(PkgEdition::UNSPEC);
		is_downgrade_from = (ipkg->edition() > pkg->edition())
							? ipkg->edition()
							: PkgEdition(PkgEdition::UNSPEC);
	}
	else {
		is_upgrade_from = PkgEdition(PkgEdition::UNSPEC);
		is_downgrade_from = PkgEdition(PkgEdition::UNSPEC);
	}
}

PkgDep::Result::Result( const PkgDep& pkgdep, const PkgName& n )
{
	name = n;
	edition = PkgEdition(PkgEdition::UNSPEC);
	upgrade_to_remove_conflict = false;
	from_input_list = false;
	is_upgrade_from = PkgEdition(PkgEdition::UNSPEC);
	is_downgrade_from = PkgEdition(PkgEdition::UNSPEC);
}

void PkgDep::ErrorResult::add_conflict( const PkgRevRelation& rrel,
										const PkgDep& dep,
										const Solvable *to_remove,
										const Solvable *assume_instd,
										bool is_conflict )
{
	conflicts_with.push_back( RelInfo( rrel, is_conflict ));
	if (to_remove)
		dep.virtual_remove_package( to_remove, remove_to_solve_conflict,
									assume_instd );
}

void PkgDep::ErrorResult::add_conflict( PkgName n, const PkgRelation& rel,
										const PkgDep& dep,
										const Solvable *to_remove,
										const Solvable *assume_instd,
										bool is_conflict )
{
	conflicts_with.push_back( RelInfo( n, rel, is_conflict ));
	if (to_remove)
		dep.virtual_remove_package( to_remove, remove_to_solve_conflict,
									assume_instd );
}

void PkgDep::ErrorResult::add_unresolvable( PkgName n, const PkgRelation& rel )
{
	unresolvable.push_back( RelInfo( n, rel, false ));
}

void PkgDep::ErrorResult::add_alternative( PkgName n, alternative_kind k )
{
	alternatives.push_back( Alternative( n, k ));
}

void PkgDep::Result::add_notes( const Notes& notes )
{
	from_input_list = notes.from_input;
	upgrade_to_remove_conflict = notes.upgrade_to_solve_conflict;
	install_to_avoid_break = notes.install_to_avoid_break;
	ci_for( IRelInfoList::, n, notes.referers. ) {
		referers.push_back( RelInfo(n->pkg->name(), n->rel) );
	}
}

void PkgDep::ErrorResult::add_notes( const Notes& notes )
{
	Result::add_notes( notes );
	if (notes.not_available) {
		not_available = true;
		not_avail_range = notes.not_avail_range;
	}
}

void PkgDep::NeededEditionRange::merge( const PkgRelation& rel )
{
	rel_op op = rel.op();
	
	switch( op ) {
	  case NONE:
		break;

	  case GT:
	  case GE:
		if (greater.is_unspecified() ||
			greater < rel.edition() ||
			(greater == rel.edition() && greater_incl && op == GT)) {
			greater = rel.edition();
			greater_incl = (op == GE);
		}
		break;

	  case LT:
	  case LE:
		if (less.is_unspecified() ||
			less > rel.edition() ||
			(less == rel.edition() && less_incl && op == LT)) {
			less = rel.edition();
			less_incl = (op == LE);
		}
		break;

	  case EQ:
		if (greater.is_unspecified() ||
			greater < rel.edition()) {
			greater = rel.edition();
			greater_incl = true;
		}
		if (less.is_unspecified() ||
			less > rel.edition()) {
			less = rel.edition();
			less_incl = true;
		}
		break;

	  case NE:
		cerr << "Oops... relation is != in EditionRange::merge!\n";
		break;
	}
}

// Local Variables:
// tab-width: 4
// End:
