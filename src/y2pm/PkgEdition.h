#ifndef _PkgEdition_h
#define _PkgEdition_h

#include <ctime>
#include <string>
#include <iosfwd>

enum rel_op { NONE, EQ, NE, LT, LE, GT, GE };
extern const char* op_str[];

/**
 * @short PkgEdition represents <code>[epoch:]version[-release]</code>
 *
 * Syntax for specifying a PkgEdition:
 * <PRE>
 * [epoch:]version[-release]
 *
 *   epoch   (optional) number, with assumed default of 0 if not supplied
 *   version (required) can contain any character except '-'
 *   release (optional) can contain any character except '-'
 * </PRE>
 *
 * PkgEdition can also represent the special editions <code>MAXIMUM</code>
 * (greater than all real editions) and <code>UNSPEC</code> (uncomparable).
 *
 * PkgEdition may also contain a buildtime (optional, default of 0). On comparing
 * PkgEditions the buildtime is taken into account, iff PkgEditions are equal and
 * both contain a nonzero buildtime.
 **/
class PkgEdition {

  public:

    enum type_enum { NORMAL, MAXIMUM, UNSPEC };

  private:

    // Use a 'version-release' form for these strings.
    // (i.e. exactly one '-')
    static const std::string _str_UNSPEC;
    static const std::string _str_MAXIMUM;

    type_enum   _type;
    unsigned    _epoch;
    std::string _version;
    std::string _release;
    time_t      _buildtime;

    bool edition_eq( const PkgEdition& e2 ) const;
    bool edition_lt( const PkgEdition& e2 ) const;

    /**
     * Return -1,0,1 if versions are <,==,>
     **/
    int rpmvercmp( const std::string & a, const std::string & b ) const;

    /**
     * Helper for Constructor
     *
     * String form for an edition is <code>[epoch:]version-release</code>,
     * where (optional) epoch is a number, and neither version nor release
     * may contain a '-'. This form may be passed to the version string
     * <code>v</code>.
     *
     * In constructors providing an 'int e' epoch value, an epoch part within
     * the version string <code>v</code> is not allowed (ignored).
     *
     * In constructors providing an nonempty release string <code>r</code>,
     * a release part within the version string <code>v</code> is
     *  not allowed (ignored).
     * <pre>
     * Construct PkgEdition: epoch 3, version 1.0 release 1
     *     PkgEdition( "3:1.0-1" )
     *     PkgEdition( "3:1.0", "1" )
     *     PkgEdition( 3, "1.0-1" )
     *     PkgEdition( 3, "1.0", "1" )
     * </pre>
     *
     * Assert _version/_release are empty srings on MAXIMUM and UNSPEC type.
     **/
    void xconstruct( type_enum xtype,
		     unsigned epoch, const std::string & v, const std::string & r,
		     time_t buildtime );

  public:

    /**
     * Constructor
     * @see #xconstruct
     **/
    PkgEdition( type_enum t = NORMAL ) {
      xconstruct(t,0,"","",0);
    }
    /**
     * Constructor
     * @see #xconstruct
     **/
    PkgEdition( const std::string & v, const std::string & r = "", time_t bt = 0 ) {
      xconstruct(NORMAL,0,v,r,bt);
    }
    /**
     * Constructor
     * @see #xconstruct
     **/
    PkgEdition( const std::string & v, time_t bt ) {
      xconstruct(NORMAL,0,v,"",bt);
    }
    /**
     * Constructor
     * @see #xconstruct
     **/
    PkgEdition( unsigned e, const std::string & v, const std::string & r = "", time_t bt = 0 ) {
      xconstruct(NORMAL,e,v,r,bt);
    }
    /**
     * Constructor
     * @see #xconstruct
     **/
    PkgEdition( unsigned e, const std::string & v, time_t bt ) {
      xconstruct(NORMAL,e,v,"",bt);
    }

    ~PkgEdition() {}

    const std::string & version() const {
      switch( _type ) {
      case MAXIMUM:
	return _str_MAXIMUM;
      case UNSPEC:
	return _str_UNSPEC;
      case NORMAL:
	break;
      }
      return _version;
    }

    const std::string & release() const {
      return _release;
    }

    int epoch() const { return _epoch; }

    bool has_epoch() const { return _epoch; }

    time_t buildtime() const { return _buildtime; }

    bool is_unspecified() const { return _type == UNSPEC; }

    bool is_maximum() const { return _type == MAXIMUM; }

    bool compare( rel_op op, const PkgEdition& e2 ) const;
    bool operator==( const PkgEdition& e2 ) const { return compare( EQ, e2 ); }
    bool operator!=( const PkgEdition& e2 ) const { return compare( NE, e2 ); }
    bool operator< ( const PkgEdition& e2 ) const { return compare( LT, e2 ); }
    bool operator<=( const PkgEdition& e2 ) const { return compare( LE, e2 ); }
    bool operator> ( const PkgEdition& e2 ) const { return compare( GT, e2 ); }
    bool operator>=( const PkgEdition& e2 ) const { return compare( GE, e2 ); }

    std::string asString() const;

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

#endif  // _PkgEdition_h
