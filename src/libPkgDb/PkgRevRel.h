#ifndef _PkgRevRel_h
#define _PkgRevRel_h

#include <Package.h>
#include <PkgRelation.h>

class PkgRevRelation {
	// _relation can be a NULL pointer; that means that this is a
	// self-providing entry, i.e. the package provides its own name. If
	// somebody asks for the PkgRelation, a faked one is returned.
	
	const PkgRelation *_relation;
	const Package *_pkg;

  public:
	PkgRevRelation( const PkgRelation *r, const Package *p )
		: _relation(r), _pkg(p) {}
	// default copy constructor and assigment are ok

	const PkgRelation relation() const {
		return _relation ? *_relation : _pkg->self_provides();
	}
	const Package *pkg() const { return _pkg; }
};

#endif  /* _PkgRevRel_h */


// Local Variables:
// tab-width: 4
// End:
