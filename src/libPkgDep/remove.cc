#include <y2util/Y2SLog.h>
#include <y2pm/PkgDep.h>
#include <y2pm/PkgDep_int.h>

using namespace std;

void PkgDep::remove( SolvableList& pkgs )
{
    SolvableList pkgs_p;

    ci_for( SolvableList::,, it, pkgs., ) {
	if((*it) == NULL)
	    { ERR << "got NULL PMSolvablePtr" << endl; continue; }

	if (installed.includes((*it)->name()))
	    pkgs_p.push_back( (*it) );
	else
	    WAR << (*it)->name() << "is not installed" << endl;
    }

    pkgs = SolvableList();
    ci_for( SolvableList::,, pkg, pkgs_p., ) {
	    remove_package( &installed, *pkg, pkgs );
    }
}

void PkgDep::virtual_remove_package( PMSolvablePtr pkg, SolvableList& to_remove,
				 PMSolvablePtr assume_instd ) const
{
    PkgSet set = vinstalled;
    if (assume_instd)
	set.add( assume_instd );
    remove_package( &set, pkg, to_remove );
}

//recoursive remove package pkg from PkgSet set and extend to_remove
//with all removed packages
void PkgDep::remove_package( PkgSet *set, PMSolvablePtr pkg, SolvableList& to_remove )
{
	D__ << "removing package " << pkg->name() << endl;
	set->remove( pkg );

	bool already_present = false;
	ci_for( SolvableList::,, it, to_remove., ) {
		if (*it == pkg) {
			already_present = true;
			break;
		}
	}
	if (!already_present)
		to_remove.push_back( pkg );

	PMSolvable::PkgRelList_type list = pkg->provides();

	for(unsigned callbackdone = 0;callbackdone < 2; callbackdone++)
	{
	    ci_for( PMSolvable::,PkgRelList_, prov, list.,) {
		    D__ << "  checking provided name " << (*prov).name() << endl;
		    RevRel_for( set->required()[(*prov).name()], req1 ) {
			    D__ << "    requirement: " << req1->relation()
				     << " by " << req1->pkg()->name() << endl;
			    if (count_providers_for( set, req1->relation() ) < 1) {
				    D__ << "    no providers anymore, removing "
					     << req1->pkg()->name() << endl;
				    remove_package( set, req1->pkg(), to_remove );
			    }
		    }
	    }

	    // add additional provides from packageset as well as the package name itself
	    if(!callbackdone)
	    {
		if(set->AdditionalProvidesCallback())
		{
		    PkgSet::getAdditionalProvides_callback callback=set->AdditionalProvidesCallback();
		    list = callback(pkg);
		}
		else
		    list = PMSolvable::PkgRelList_type();

		list.push_front(pkg->self_provides());
	    }
	}
}

// Local Variables:
// tab-width: 4
// End:
