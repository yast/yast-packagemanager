/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:       RpmLibDb.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Wraps acces to rpmdb via librpm.

/-*/
#ifndef RpmLibDb_h
#define RpmLibDb_h

extern "C" {
#include <rpm/rpmlib.h>
}

#include <iosfwd>
#include <vector>
#include <list>

#include <y2util/Pathname.h>

#include <y2pm/InstTargetError.h>
#include <y2pm/RpmLibHeader.h>
#include <y2pm/PMPackagePtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : RpmLibDb
/**
 * @short Wraps acces to RPM database via librpm.
 *
 * Besides the task of opening and closing the RPM database,
 * <CODE>RpmLibDb</CODE> provides methods to retrieve individual
 * packages data.
 *
 * A packages data are stored in a @ref RpmLibHeader. The <CODE>RpmLibHeader</CODE>
 * contains the complete rpm header struct. Thus it needs some memory, but data stored
 * within are accessible after the RPM database was closed.
 *
 * This is not true for provided iterator classes like @ref const_header_set
 * or @ref const_iterator. In order to iterate or retrieve a certain
 * <CODE>RpmLibHeaderPtr</CODE> the RPM database must be open, as they store
 * nothing but database indices.
 **/
class RpmLibDb {

  friend std::ostream & operator<<( std::ostream & str, const RpmLibDb & obj );

  /**
   * NO ASSIGNMENT!
   **/
  RpmLibDb & operator=( const RpmLibDb & );
  /**
   * NO COPY!
   **/
  RpmLibDb ( const RpmLibDb & );

  public:

    /**
     * default error class
     **/
    typedef InstTargetError Error;

  private:

    /**
     * Static flag whether globalInit() was called
     **/
    static bool _globalInitialized;

    /**
     * Reads rpmReadConfigFiles
     **/
    static PMError globalInit();

  private:

    /**
     * Path to RPM database
     **/
    Pathname _dbPath;

    /**
     * Handle to RPM database. NULL if closed.
     **/
    rpmdb _db;

  public:

    /**
     * Constructor. Path to RPM database is passed as argument.
     * globalInit() is called if necessary. RPM database is
     * <b>not</b> opened.
     **/
    RpmLibDb( const Pathname & dbPath_r );

    /**
     * Closes RPM database if open.
     **/
    ~RpmLibDb();

  public:

    /**
     * Open RPM database.
     **/
    PMError dbOpen();

    /**
     * Close RPM database.
     **/
    PMError dbClose();

    /**
     * True if RPM database is open.
     **/
    bool isOpen() const { return _db; }

    /**
     * Returns path to RPM database.
     **/
    const Pathname & dbPath() const { return _dbPath; }

  public:

    /**
     * DEVELOPER STUFF. DONT'T USE IT.
     **/
    class openIf {
      private:
	RpmLibDb & _RpmLibDb;
	bool       _mustclose;
	PMError    _openerr;
      public:
	openIf( RpmLibDb & rpmlibdb_r )
	  : _RpmLibDb( rpmlibdb_r )
	{
	  _mustclose = ! _RpmLibDb.isOpen();
	  _openerr = _mustclose ? _RpmLibDb.dbOpen() : Error::E_ok;
	}
	~openIf() {
	  if ( _mustclose )
	    _RpmLibDb.dbClose();
	}
	PMError openerr() const { return _openerr; }
    };

  public:

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : RpmLibDb::const_header_set
    /**
     * @short Iterator returned by queries which may lead to more than one package.
     *
     * Iterator returned by queries which may lead to more than one package.
     * <PRE>
     *   RpmLibDb rpmdb( "/var/lib/rpm" );
     *   if ( rpmdb.dbOpen() ) {
     *     return;
     *   }
     *
     *   constRpmLibHeaderPtr first;
     *   <b>RpmLibDb::const_header_set result</b>( rpmdb.findByFile( "/somwhere/somefile" ) );
     *
     *   if ( <b>result.size()</b> ) {
     *     first = <b>result[0]</b>; // access to rpmdb!
     *   }
     *
     *   rpmdb.dbClose();
     *
     *   if ( first ) // data stored in first remain vald after dbClose!
     *     cout << "Found " << first->tag_name() << "-" << first->tag_edition() << endl;
     *   else
     *     cout << "No package found." << endl;
     * </PRE>
     *
     * @see #const_header_set
     **/
    class const_header_set {

      friend class RpmLibDb;

      private:

	rpmdb                 _dbptr;
	std::vector<unsigned> _idxSet;

	/**
	 * Constructor used by RpmLibDb.
	 **/
	const_header_set( rpmdb dbptr_r, dbiIndexSet & idxSet_r )
	  : _dbptr( dbptr_r )
	{
	  if ( _dbptr && idxSet_r.count ) {
	    _idxSet.resize( idxSet_r.count );
	    for ( int i = 0; i < idxSet_r.count; ++i ) {
	      _idxSet[i] = idxSet_r.recs[i].recOffset;
	    }
	  }
	  ::dbiFreeIndexRecord( idxSet_r );
	}

      public:

	/**
	 * Default constructor.
	 **/
	const_header_set()
	  : _dbptr( 0 )
	{}

	/**
	 * Number of packages available.
	 **/
	unsigned size() const { return _idxSet.size(); }

	/**
	 * Retrieve individual packages data. Needs access to rpmdb.
	 **/
	constRpmLibHeaderPtr operator[]( const unsigned idx_r ) const;
    };

    ///////////////////////////////////////////////////////////////////

    /**
     * Return all packages that own a certain file.
     **/
    const_header_set findByFile( const std::string & which_r ) const;

    /**
     * Find package by name. Returns NULL, if package is not installed.
     *
     * Multiple entries for one package installed in different versions
     * are possible but not desired. If so, the last package installed
     * is returned.
     **/
    constRpmLibHeaderPtr findPackage( const PkgName & name_r ) const;

    /**
     * Find package by name and edition. Returns NULL, if package is not installed.
     * Commonly used by PMRpmPackageDataProvider.
     **/
    constRpmLibHeaderPtr findPackage( const PkgName & name_r, const PkgEdition & ed_r ) const;
    /**
     * Abbr. for <code>findPackage( which_r.name, which_r.edition );</code>
     **/
    constRpmLibHeaderPtr findPackage( const PkgNameEd & which_r ) const;
    /**
     * Abbr. for <code>findPackage( which_r->name(), which_r->edition() );</code>
     **/
    constRpmLibHeaderPtr findPackage( const constPMPackagePtr & which_r ) const;

  public:

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : RpmLibDb::const_iterator
    /**
     * @short Iterate all packages in RPM database.
     *
     * Iterate all packages in RPM database.
     * <PRE>
     *   RpmLibDb rpmdb( "/var/lib/rpm" );
     *   if ( rpmdb.dbOpen() ) {
     *     return;
     *   }
     *
     *   for ( RpmLibDb::const_iterator iter = rpmdb.begin(); iter != rpmdb.end(); ++iter ) {
     *     if ( !*iter ) {
     *       cerr << "RPM database corrupt. Try rebuild." << endl;
     *       break;
     *     }
     *     PkgName name        = iter->tag_name();
     *     Date    installtime = iter->tag_installtime();
     *     ...
     *   }
     *   rpmdb.dbClose();
     * </PRE>
     *
     * @see #const_iterator
     **/
    class const_iterator {

      friend class RpmLibDb;

      private:

	mutable constRpmLibHeaderPtr _hptr;

	rpmdb    _dbptr;
	unsigned _recnum;

	void setrec( int recnum_r );

	/**
	 * Constructor used by RpmLibDb.
	 **/
	const_iterator( rpmdb dbptr_r )
	  : _hptr( 0 ), _dbptr( dbptr_r ), _recnum( 0 )
	{
	  if ( _dbptr )
	    setrec( ::rpmdbFirstRecNum( _dbptr ) );
	}

      public:

	/**
	 * Default constructor.
	 **/
	const_iterator()
	  : _hptr( 0 ), _dbptr( 0 ), _recnum( 0 )
	{}

	/**
	 * Advance to next package. Needs access to rpmdb.
	 **/
	void operator++() {
	  if ( _dbptr )
	    setrec( ::rpmdbNextRecNum( _dbptr, _recnum ) );
	}

	/**
	 * Test for equal iterator position.
	 **/
	bool operator==( const const_iterator & rhs ) const {
	  return( _dbptr == rhs._dbptr && _recnum == rhs._recnum );
	}

	/**
	 * Test for different iterator position.
	 **/
	bool operator!=( const const_iterator & rhs ) const {
	  return !operator==( rhs );
	}

	/**
	 * Returns the current constRpmLibHeaderPtr.
	 **/
	constRpmLibHeaderPtr & operator*() const {
	  return _hptr;
	}

	/**
	 * Forwards to the current constRpmLibHeaderPtr.
	 **/
	constRpmLibHeaderPtr & operator->() const {
	  return operator*();
	}

    };
    ///////////////////////////////////////////////////////////////////

    /**
     * Returns const_iterator pointing to the 1st entry in RPM database.
     **/
    const_iterator begin() const { return const_iterator( _db ); }

    /**
     * Returns const_iterator pointing behind the last entry in RPM database.
     **/
    const_iterator end() const { return const_iterator(); }

};

///////////////////////////////////////////////////////////////////

#endif // RpmLibDb_h
