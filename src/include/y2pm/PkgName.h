#ifndef _PkgName_h
#define _PkgName_h

#include <y2util/UniqStr.h>
#include <y2pm/PkgEdition.h>

class PkgName {
	static UniqStr PkgNameHash;
	const char *name;
  public:
	PkgName( const char *n = "" ) { name = PkgNameHash.add(n); }
	PkgName( const std::string& s ) { name = PkgNameHash.add(s.c_str()); }
	// default copy constructor and assigment are ok

	bool operator==( const PkgName& n2 ) const { return name == n2.name; }
	bool operator!=( const PkgName& n2 ) const { return name != n2.name; }
	operator const char* () const { return name; }
	friend std::ostream& operator<<( std::ostream&, const PkgName& );
};

inline size_t hashfun( const PkgName& n ) {
	return (size_t)(const char *)n;
}

struct PkgNameEd {
	PkgName name;
	PkgEdition edition;

	PkgNameEd( const PkgName& n, const PkgEdition& e )
		: name(n), edition(e) {}
	PkgNameEd( int bd, int mh, const char *n, const char *v, const char *r )
		: name(n), edition(bd,mh,v,r) {}
	// default copy constructor and assigment are ok

	bool operator==( const PkgNameEd& ne2 ) const {
		return name == ne2.name && edition == ne2.edition;
	}
	bool operator!=( const PkgNameEd& ne2 ) const {
		return name != ne2.name || edition != ne2.edition;
	}

	friend std::ostream& operator<<( std::ostream&, const PkgNameEd& );
};


#endif  /* _PkgName_h */


// Local Variables:
// tab-width: 4
// End:
