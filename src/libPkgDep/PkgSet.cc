#include <functional>

#include <y2util/Y2SLog.h>
#include <y2pm/PkgSet.h>
#include <y2pm/PMSolvable.h>

using namespace std;

PkgSet::PkgSet() : _additionalprovides_callback(NULL)
{
}

PkgSet::~PkgSet()
{
}

#define RelList_for(field,var)											\
	for( PMSolvable::PkgRelList_const_iterator var = pkg->field##_begin();	\
	     var != pkg->field##_end(); ++p )

void PkgSet::add( PMSolvablePtr pkg, bool force )
{
	if(!pkg)
	{
	    ERR << "got NULL" << endl;
	    return;
	}
	PMSolvablePtr opkg = lookup(pkg->name());
	if (opkg != NULL) {
		// the same name is already contained in this set
		// keep the already contained package if its edition is greater or
		// equal; otherwise replace it
		if (!force && opkg->edition() >= pkg->edition())
			return;
		remove( opkg );
	}
	
	contents[pkg->name()] = pkg;

	RelList_for( requires, p ) {
		_required[p->name()].push_back( PkgRevRelation( &*p, pkg ) );
	}
	RelList_for( conflicts, p ) {
		_conflicted[p->name()].push_back( PkgRevRelation( &*p, pkg ) );
	}
	RelList_for( provides, p ) {
		_provided[p->name()].push_back( PkgRevRelation( &*p, pkg ) );
	}
	RelList_for( obsoletes, p ) {
		_obsoleted[p->name()].push_back( PkgRevRelation( &*p, pkg ) );
	}

	// every package "provides" its own name
	_provided[pkg->name()].push_back( PkgRevRelation( NULL, pkg ) );

	if(_additionalprovides_callback)
	{
	    PMSolvable::PkgRelList_type addprovides = _additionalprovides_callback(pkg);
	    for(PMSolvable::PkgRelList_const_iterator p = addprovides.begin();
		p != addprovides.end(); ++p)
	    {
		_provided[p->name()].push_back( PkgRevRelation( &*p, pkg ) );
	    }
	}
}

class RevRel_By {
	PMSolvablePtr pkg;
  public:
	RevRel_By( PMSolvablePtr p ) : pkg(p) {}
	bool operator() ( const PkgRevRelation& rrel ) {
		return rrel.pkg() == pkg;
	}
};

void PkgSet::remove( PMSolvablePtr pkg )
{
	if (!contents.erase( pkg->name() ))
		return; // wasn't contained

	RevRel_By pred(pkg);
	RelList_for( requires, p ) {
		_required[p->name()].remove_if( pred );
	}
	RelList_for( conflicts, p ) {
		_conflicted[p->name()].remove_if( pred );
	}
	RelList_for( provides, p ) {
		_provided[p->name()].remove_if( pred );
	}
	RelList_for( obsoletes, p ) {
		_obsoleted[p->name()].remove_if( pred );
	}
	_provided[pkg->name()].remove_if( pred );

	if(_additionalprovides_callback)
	{
	    PMSolvable::PkgRelList_type addprovides = _additionalprovides_callback(pkg);
	    for(PMSolvable::PkgRelList_const_iterator p = addprovides.begin();
		p != addprovides.end(); ++p)
	    {
		_provided[p->name()].remove_if( pred );
	    }
	}
}


void PkgSet::new_provides( PMSolvablePtr pkg, const PkgRelation& prov )
{
	if (!includes(pkg->name()) ||
		lookup(pkg->name())->edition() != pkg->edition())
		// package not contained in this set
		return;
	
	_provided[prov.name()].push_back( PkgRevRelation( &prov, pkg ));
}

bool PkgSet::empty() const
{
	return contents.empty();
}

unsigned PkgSet::size() const
{
    return contents.size();
}

void PkgSet::remove( PkgName name )
{
    PMSolvablePtr pkg = lookup(name);

    if (pkg) remove( pkg );
}

PMSolvablePtr PkgSet::lookup( const PkgName& name ) const
{
    const_iterator it = contents.find(name);
    
    if(it==contents.end())
	return NULL;

    return it->second;
}

bool PkgSet::includes( const PkgName& name ) const
{
    return (contents.count(name)>0);
}

static PkgSet::RevRelList_type _tmprevrellist;

const PkgSet::RevRelList_type& PkgSet::getRevRelforPkg(const PkgSet::InvRel_type& m, const PkgName& n)
{
    PkgSet::InvRel_const_iterator it = m.find(n);
    if(it == m.end())
	return _tmprevrellist;

    return it->second;
}

// Local Variables:
// tab-width: 4
// End:
