#include <set>
#include <deque>

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
// FIXME: to_remove should be by name as it could affect toinstall and installed packages?
void PkgDep::remove_package( PkgSet *set, PMSolvablePtr startpkg, SolvableList& to_remove )
{
    std::deque<PMSolvablePtr> tocheck;
    std::set<PMSolvablePtr> removeset;
    unsigned numremove = 0;

    // copy list to set for faster access
    ci_for( SolvableList::,, it, to_remove., )
    {
	removeset.insert(*it);
    }

    // nothing to do if already in to_remove
    if(removeset.find(startpkg) != removeset.end())
    {
	D__ << startpkg->name() << "already in to_remove, nothing to do" << endl;
	return;
    }

    // ok, start with startpkg
    tocheck.push_front(startpkg);

    while(!tocheck.empty())
    {
	PMSolvablePtr pkg = tocheck.front();
	tocheck.pop_front();
	removeset.insert(pkg);

	D__ << "removing package " << pkg->name() << endl;
	set->remove( pkg );

	PMSolvable::PkgRelList_type list = pkg->provides();

	D__ << " need to check " << list.size() << " provides" << endl;

	unsigned numprov =0;
	ci_for( PMSolvable::,PkgRelList_, prov, list.,) {
		++numprov;
		D__ << "  checking provided name " << (*prov).name() << " (" << numprov << ')' << endl;
		RevRel_for( PkgSet::getRevRelforPkg(set->required(),(*prov).name()), req1 ) {
			D__ << "    requirement: " << req1->relation()
				 << " by " << req1->pkg()->name() << endl;
			if (count_providers_for( set, req1->relation() ) < 1) {
				D__ << "    no providers anymore, schedule remove of "
					 << req1->pkg()->name() << endl;
				++numremove;
				if(removeset.find(req1->pkg()) == removeset.end())
				{
				    tocheck.push_back(req1->pkg());
				}
				else
				{
				    D__ << "    not really, already in toremove :-)" << endl;
				}
			}
		}
	}

	D__ << "done removing " << pkg->name() << endl;
    }


    to_remove.clear();
    for( std::set<PMSolvablePtr>::iterator it = removeset.begin();
	    it !=removeset.end(); ++it)
    {
	to_remove.push_back( *it );
    }

    D__ << "removing " << startpkg->name() << " causes removal of " << numremove << " other packages" << endl;
}

// Local Variables:
// tab-width: 4
// End:
