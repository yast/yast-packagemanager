#ifndef _PkgName_h
#define _PkgName_h

#include <y2util/Ustring.h>
#include <y2pm/PkgEdition.h>


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgName
/**
 * A package name
 **/
class PkgName : public Ustring {

  private:

    static UstringHash _nameHash;

  public:

    explicit PkgName( const std::string & n = "" ) : Ustring( _nameHash, n ) {}
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgNameEd
/**
 * Convenience struct containing a PkgName and PkgEdition
 * @see PkgName
 * @see PkgEdition
 **/
struct PkgNameEd {

  public:

	PkgName    name;
	PkgEdition edition;

	/**
	 * Constructor
	 **/
	PkgNameEd( const PkgName & n, const PkgEdition & e )
		: name(n), edition(e) {}
	/**
	 * Constructor
	 *
	 * Versions string <code>v</code> may code an epoch. It may
	 * code a release, if no explicit release string <code>r</code>
	 * is given.
	 * @see PkgEdition
	 **/
	PkgNameEd( int bd, int mh,
		   const std::string & n,
		   const std::string & v,
		   const std::string & r = "" )
		: name(n), edition(bd,mh,v,r) {}
	// default copy constructor and assigment are ok

	bool operator==( const PkgNameEd & ne2 ) const {
		return name == ne2.name && edition == ne2.edition;
	}
	bool operator!=( const PkgNameEd & ne2 ) const {
		return !operator==( ne2 );
	}

	friend std::ostream& operator<<( std::ostream&, const PkgNameEd& );

      public:
	/**
	 * Convert PkgNameEd to string (on save to file).
	 * <b>Keep it compatible with fromString.</b>
	 **/
	static std::string toString( const PkgNameEd & t );

	/**
	 * Restore PkgNameEd from string (on restore from file).
	 * <b>Keep it compatible with toString.</b>
	 **/
	static PkgNameEd fromString( std::string s );
};


#endif  /* _PkgName_h */


// Local Variables:
// tab-width: 4
// End:
