#ifndef _PkgEdition_h
#define _PkgEdition_h

#include <string>
#include <iostream>

enum rel_op { NONE, EQ, NE, LT, LE, GT, GE };
extern const char* op_str[];

/**
 * @short PkgEdition represents  [epoch:]version-release
 *
 * PkgEdition represents version, release, and epoch of a package.
 * It provides methods for comparing.
 * It can also represent the special editions MAXIMUM (greater than all real
 * editions) and UNSPEC (uncomparable).
 **/
class PkgEdition {

  public:

    enum type_enum { NORMAL, EPOCH, MAXIMUM, UNSPEC };

  private:

        // Use a 'version-release' form for these strings.
        // (i.e. exactly one '-')
	static const std::string _str_UNSPEC;
	static const std::string _str_MAXIMUM;

	type_enum type;
	int _epoch;
	int _buildtime;
	int _metahash;
	std::string _version;
	std::string _release;

	bool edition_eq( const PkgEdition& e2 ) const;
	bool edition_lt( const PkgEdition& e2 ) const;

	int rpmvercmp( const std::string & a, const std::string & b ) const;

	// helper for copy constructor and operator=
	void xconstruct( type_enum xtype, int buildtime, int metahash,
			 int epoch, const char *v, const char *r ) {
		type = xtype;
		_epoch = epoch;
		_buildtime = buildtime;
		_metahash = metahash;
		_version = v ? v : "";
		_release = r ? r : "";
	}

  public:
	PkgEdition( const char *v = "", const char *r = NULL ) {
		xconstruct(NORMAL,0,0,0,v,r);
	}
	PkgEdition( int e, const char *v, const char *r = NULL ) {
		xconstruct(EPOCH,0,0,e,v,r);
	}
	PkgEdition( int buildtime, int metahash, const char *v, const char *r = NULL ) {
		xconstruct(NORMAL,buildtime,metahash,0,v,r);
	}
	PkgEdition( int buildtime, int metahash, int e, const char *v, const char *r = NULL ) {
		xconstruct(EPOCH,buildtime,metahash,e,v,r);
	}
	PkgEdition( type_enum t ) {
		assert( t == MAXIMUM || t == UNSPEC );
		xconstruct(t,0,0,0,NULL,NULL);
	}
	~PkgEdition() {}

	const char *version() const {
		switch( type ) {
		  case NORMAL:
		  case EPOCH:
			return _version.c_str();
		  case MAXIMUM:
			return _str_MAXIMUM.c_str();
		  case UNSPEC:
			return _str_UNSPEC.c_str();
		}
		return "";
	}
	const char *release() const {
		return (type == NORMAL || type == EPOCH) ? _release.c_str() : "";
	}

	int epoch() const { return type == EPOCH ? _epoch : 0; }

	int buildtime() const { return type == NORMAL ? _buildtime : 0; }

	bool has_epoch() const { return type == EPOCH; }

	bool is_unspecified() const { return type == UNSPEC; }
	bool is_maximum() const { return type == MAXIMUM; }

	bool compare( rel_op op, const PkgEdition& e2 ) const;
	bool operator==( const PkgEdition& e2 ) const { return compare( EQ, e2 ); }
	bool operator!=( const PkgEdition& e2 ) const { return compare( NE, e2 ); }
	bool operator< ( const PkgEdition& e2 ) const { return compare( LT, e2 ); }
	bool operator<=( const PkgEdition& e2 ) const { return compare( LE, e2 ); }
	bool operator> ( const PkgEdition& e2 ) const { return compare( GT, e2 ); }
	bool operator>=( const PkgEdition& e2 ) const { return compare( GE, e2 ); }

	std::string as_string() const;
	//operator std::string() const { return as_string(); }

	friend std::ostream& operator<<( std::ostream&, const PkgEdition& );

  public:

    /**
     * Convert PkgEdition to string (on save to file).
     * <b>Keep it compatible with fromString.</b>
     **/
    static std::string toString( const PkgEdition & t );

    /**
     * Restore PkgEdition from string (on restore from file).
     * <b>Keep it compatible with toString.</b>
     **/
    static PkgEdition fromString( std::string s );

};

#endif  /* _PkgEdition_h */


// Local Variables:
// tab-width: 4
// End:
