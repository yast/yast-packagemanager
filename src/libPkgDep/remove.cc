#include <y2pm/PkgDep.h>
#include <y2pm/PkgDep_int.h>

using namespace std;

void PkgDep::remove( NameList& pkgs )
{
	typedef list<PMSolvablePtr > PkgPList;
	PkgPList pkgs_p;
	
	ci_for( NameList::, name, pkgs. ) {
		if (installed.includes(*name))
			pkgs_p.push_back( installed[*name] );
	}

	pkgs = NameList();
	ci_for( PkgPList::, pkg, pkgs_p. ) {
		remove_package( &installed, *pkg, pkgs );
	}
}

void PkgDep::virtual_remove_package( PMSolvablePtr pkg, NameList& to_remove,
				 PMSolvablePtr assume_instd ) const
{
	PkgSet set = vinstalled;
	if (assume_instd)
		set.add( assume_instd );
	remove_package( &set, pkg, to_remove );
}
	

void PkgDep::remove_package( PkgSet *set, PMSolvablePtr pkg,
				 NameList& to_remove ) const
{
	DBG( "removing package " << pkg->name() << endl );
	set->remove( pkg );

	bool already_present = false;
	ci_for( NameList::, n, to_remove. ) {
		if (*n == pkg->name()) {
			already_present = true;
			break;
		}
	}
	if (!already_present)
		to_remove.push_back( pkg->name() );
	
	ci_for( PMSolvable::Provides_, prov, pkg->all_provides_) {
		DBG( "  checking provided name " << (*prov).name() << endl );
		RevRel_for( set->required()[(*prov).name()], req1 ) {
			DBG( "    requirement: " << req1->relation()
				 << " by " << req1->pkg()->name() << endl );
			if (count_providers_for( set, req1->relation() ) < 1) {
				DBG( "    no providers anymore, removing "
					 << req1->pkg()->name() << endl );
				remove_package( set, req1->pkg(), to_remove );
			}
		}
	}
}

// Local Variables:
// tab-width: 4
// End:
