#ifndef _PkgRelation_h
#define _PkgRelation_h

#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PMSolvablePtr.h>

class PkgRelation {
	PkgName _name;
	rel_op _op;
	PkgEdition _edition;
  public:
	PkgRelation( const PkgName& n, rel_op o, const PkgEdition& e ) :
		_name(n), _op(o), _edition(e) {};
	// default copy constructor and assigment are ok

	const PkgName& name() const { return _name; }
	PkgName& name() { return _name; }
	rel_op op() const { return _op; }
	rel_op& op() { return _op; }
	const PkgEdition& edition() const { return _edition; }
	PkgEdition& edition() { return _edition; }

	// return true if this relation holds given a package (providing itself)
	// or another relation (which is a provides relation)
	bool matches( const PMSolvablePtr pkg ) const;
	bool matches( const PkgRelation& rel ) const;

	bool operator==( const PkgRelation& r2 ) {
		return( _name == r2._name && _op == r2._op &&
				(_op == NONE || _edition == r2._edition) );
	}
	bool operator!=( const PkgRelation& r2 ) { return !operator==(r2); }

	/**
	 * Convert this into a human readable string.
	 **/
	std::string asString() const;

	/**
	 * Write out asString()
	 **/
	friend std::ostream& operator<<(std::ostream&, const PkgRelation&);
};

#endif  /* _PkgRelation_h */


// Local Variables:
// tab-width: 4
// End:
