#ifndef _PkgRevRel_h
#define _PkgRevRel_h

#include <y2pm/PkgRelation.h>
#include <y2pm/PMSolvable.h>

/** \brief Associate package and relation */
class PkgRevRelation
{
    private:
	const PkgRelation* _relation;
	PMSolvablePtr _p;

    public:

	/** r MUST point into p or be NULL */
	PkgRevRelation( const PkgRelation* r, PMSolvablePtr p )
	    : _relation(r), _p(p)
	{}

	/** r MUST point into p or be NULL */
	PkgRevRelation(PMSolvablePtr p, const PkgRelation* r )
	    : _relation(r), _p(p)
	{}

	const PkgRelation relation() const
	{
	    return _relation ?*_relation:_p->self_provides();
	}

	const PMSolvablePtr pkg() const
	{
	    return _p;
	}
};

#endif  /* _PkgRevRel_h */
