#include <cstdio>
#include <cstring>
#include <y2pm/Solvable.h>
#include <y2pm/PkgRelation.h>

using namespace std;

bool PkgRelation::matches( const Solvable* pkg ) const
{
	if (pkg->name() != _name)
		return false;
	if (_op == NONE)
		return true;
	return pkg->edition().compare( _op, _edition );
}


//
// This function checks if two version ranges overlap. It is needed to
// check if a relation holds. Easiest case: The whole range is
// accepted/provided if no relation+edition is present. The most
// general case is: p1 needs p2 rel1 ed1 and p2 is provided by p3 rel2
// ed2. So p1 requires some subset of the possible ranges defined by
// rel1 ed1, and some other subset defined by rel2 ed2 is available.
// PkgRelation::matches compares the two ranges if there is any
// intersection and returns true if there is.
//
bool PkgRelation::matches( const PkgRelation& rel2 ) const
{
	if (_name != rel2._name)
		return false;
	
	rel_op o1 = _op, o2 = rel2._op;
	
	// there is always an intersection if one of the version ranges is
	// the full range
	if (o1 == NONE || o2 == NONE)
		return true;
	// after that, there must be a relation on both packages!

	// there is always at least one point of intersection if both
	// relations point into the same "direction"
	if (((o1 == LT || o1 == LE) && (o2 == LT || o2 == LE)) ||
		((o1 == GT || o1 == GE) && (o2 == GT || o2 == GE)))
		return true;

	const PkgEdition& e1 = _edition;
	const PkgEdition& e2 = rel2._edition;
	// if one of the relations is =, we can use the other one directly
	if (o1 == EQ)
		return e1.compare( o2, e2 );
	if (o2 == EQ)
		return e2.compare( o1, e1 );

	// Otherwise, the relations point into different directions, and the
	// editions must be in the reverse one. Equality is allowed
	// only if both relations allow it, too.
	bool both_allow_eq = (o1 == LE || o1 == GE) && (o2 == LE || o2 == GE);
	rel_op op =
		 (o1 == LT || o1 == LE)
			   ? (both_allow_eq ? LE : LT)
			   : (both_allow_eq ? GE : GT);
	return e2.compare( op, e1 );
}

ostream& operator<<( ostream& os, const PkgRelation& rel ) {
	os << (const char *)rel._name;
	switch( rel._op ) {
	  case LT: os << " < "; break;
	  case LE: os << " <= "; break;
	  case EQ: os << " = "; break;
	  case GE: os << " >= "; break;
	  case GT: os << " > "; break;
	  case NE: os << " != "; break;
	  case NONE: return os;
	}
	os << rel._edition.as_string();
	return os;
}

// Local Variables:
// tab-width: 4
// End:
