#ifndef _PkgRelation_h
#define _PkgRelation_h

#include <list>

#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PMSolvablePtr.h>

class PkgRelation {
	PkgName _name;
	rel_op _op;
	PkgEdition _edition;
	bool _isprereq;  // whether this is a prerequirement
  public:
	PkgRelation( const PkgName& n, rel_op o, const PkgEdition& e ) :
		_name(n), _op(o), _edition(e) {};
	// default copy constructor and assigment are ok

	const PkgName& name() const { return _name; }
	rel_op op() const { return _op; }
	const PkgEdition& edition() const { return _edition; }

	// return true if this relation holds given a package (providing itself)
	// or another relation (which is a provides relation)
	bool matches( constPMSolvablePtr pkg ) const;
	bool matches( const PkgRelation & rel ) const;

	bool operator==( const PkgRelation& r2 ) const {
		return( _name == r2._name && _op == r2._op &&
				(_op == NONE || _edition == r2._edition) );
	}
	bool operator!=( const PkgRelation& r2 ) const { return !operator==(r2); }

	/**
	 * Convert this into a human readable string.
	 **/
	std::string asString() const;

	/**
	 * Mark as pre-requirement
	 * */
	void setPreReq(bool yes) { _isprereq = yes; }

	/**
	 * determine if Relation is a pre-requirement
	 * */
	bool isPreReq() const { return _isprereq; }

	/**
	 * True, if this is a file dependency
	 **/
	bool isFileRel() const { return( _name->size() && _name->at( 0 ) == '/' ); }

	/**
	 * Write out asString()
	 **/
	friend std::ostream& operator<<(std::ostream&, const PkgRelation&);

      public:

	/**
	 * Convert PkgRelation to string (on save to file).
	 * <b>Keep it compatible with fromString.</b>
	 **/
	static std::string toString( const PkgRelation & t );

	/**
	 * Restore PkgRelation from string (on restore from file).
	 * <b>Keep it compatible with toString.</b>
	 **/
	static PkgRelation fromString( std::string s );

        /**
         * Parse relations strings like "pkg1 pkg2 >= 1.2.3".
         **/
        static std::list<PkgRelation> parseRelations( const std::string & data_tr );

      private:

        static rel_op string2DepCompare( const std::string & str_tr );
};

#endif  /* _PkgRelation_h */


// Local Variables:
// tab-width: 4
// End:
