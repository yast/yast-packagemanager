#ifndef _PkgEdition_h
#define _PkgEdition_h

#include <string>
#include <iostream>

enum rel_op { NONE, EQ, NE, LT, LE, GT, GE };
extern const char* op_str[];

// PkgEdition represents version, release, and epoch of a package.
// It provides methods for comparing.
// It can also represent the special editions MAXIMUM (greater than all real
// editions) and UNSPEC (uncomparable).
class PkgEdition {
  public:
	enum type_enum { NORMAL, EPOCH, MAXIMUM, UNSPEC };
  private:
	type_enum type;
	int _epoch;
	int _buildtime;
	int _metahash;
	char *_version;
	char *_release;

	bool edition_eq( const PkgEdition& e2 ) const;
	bool edition_lt( const PkgEdition& e2 ) const;
	int rpmvercmp(const char * a, const char * b) const;

	// helper for copy constructor and operator=
	void xconstruct( type_enum xtype, int buildtime, int metahash,
		int epoch, const char *v, const char *r
	) {
		type = xtype;
		_epoch = epoch;
		_buildtime = buildtime;
		_metahash = metahash;
		_version = v ? strdup(v) : NULL;
		_release = r ? strdup(r) : NULL;
	}
	void yconstruct( const PkgEdition& e ) {
		xconstruct(e.type,e._buildtime,e._metahash,e._epoch,e._version,e._release);
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
	PkgEdition( const PkgEdition& e ) {
		yconstruct(e);
	}
	~PkgEdition() {
		if (type == MAXIMUM || type == UNSPEC)
			return;
		if (_version) {
			free( _version );
			_version = NULL;
		}
		if (_release) {
			free( _release );
			_release = NULL;
		}
	}

	PkgEdition operator= (const PkgEdition& e) {
		if (type == NORMAL || type == EPOCH) {
			if (_version) {
				free( _version );
				_version = NULL;
			}
			if (_release) {
				free( _release );
				_release = NULL;
			}
		}
		yconstruct( e );
		return *this;
	}

	const char *version() const {
		switch( type ) {
		  case NORMAL:
		  case EPOCH:
			return _version;
		  case MAXIMUM:
			return "-*-MAXIMUM-*-";
		  case UNSPEC:
			return "-*-UNSPEC-*-";
		  default:
			return "";
		}
	}
	const char *release() const {
		return (type == NORMAL || type == EPOCH) && _release ? _release : "";
	}
	bool has_release() const { return _release != NULL; }
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
	operator std::string() const { return as_string(); }

	friend std::ostream& operator<<( std::ostream&, const PkgEdition& );
};

#endif  /* _PkgEdition_h */


// Local Variables:
// tab-width: 4
// End:
