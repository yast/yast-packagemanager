#ifndef _PkgName_h
#define _PkgName_h

#include <y2util/Ustring.h>
#include <y2pm/PkgEdition.h>


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgName
/*
 *
 **/
class PkgName : public Ustring {

  private:

    static UstringHash _nameHash;

  public:

    explicit PkgName( const std::string & n = "" ) : Ustring( _nameHash, n ) {}
};

///////////////////////////////////////////////////////////////////

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
