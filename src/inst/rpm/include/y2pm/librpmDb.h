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

  File:       librpmDb.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Manage access to librpm database.

/-*/
#ifndef librpmDb_h
#define librpmDb_h

#include <iosfwd>

#include <y2util/PathInfo.h>
#include <y2util/ProgressCounter.h>

#include <y2pm/InstTargetError.h>
#include <y2pm/librpmDbPtr.h>
#include <y2pm/RpmHeader.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : librpmDb
/**
 * @short Manage access to librpm database.
 **/
class librpmDb : virtual public Rep {
  REP_BODY(librpmDb);
  /**
   * <B>INTENTIONALLY UNDEFINED<\B> because of bug in Ptr classes
   * which allows implicit conversion from librpmDbPtr to
   * constlibrpmDbPtr. Currently we don't want to provide non const
   * handles, as the database is opened READONLY.
   **/
  static PMError dbAccess( librpmDbPtr & ptr_r );

  public:

    /**
     * Default error class
     **/
    typedef InstTargetError Error;

    ///////////////////////////////////////////////////////////////////
    //
    //	static interface
    //
    ///////////////////////////////////////////////////////////////////
  private:

    /**
     * Current root directory for all operations.
     * (initialy /)
     **/
    static Pathname _defaultRoot;

    /**
     * Current directory (below root) that contains the rpmdb.
     * (initialy /var/lib/rpm)
     **/
    static Pathname _defaultDbPath;

    /**
     * Current rpmdb handle.
     **/
    static constlibrpmDbPtr _defaultDb;

    /**
     * Wheter access is blocked (no _defaultDb will be available).
     **/
    static bool _dbBlocked;

    /**
     * For internal use. Pointer returned should immediately be
     * wrapped into librpmDbPtr.
     **/
    static librpmDb * newLibrpmDb( Pathname root_r, Pathname dbPath_r, bool readonly_r, PMError & err_r );

  public:

    /**
     * Initialize lib librpm (read configfiles etc.). It's called
     * on demand but you may call it anytime.
     *
     * @return Whether librpm is initialized.
     **/
    static bool globalInit();

    /**
     * @return librpm macro expansion.
     **/
    static std::string expand( const std::string & macro_r );

    /**
     * @return String '(root_r)sub_r' used in debug output.
     **/
    static std::string stringPath( const Pathname & root_r, const Pathname & sub_r ) {
      return std::string( "'(" ) + root_r.asString() + ")" + sub_r.asString() + "'";
    }

  public:

    /**
     * @return Current root directory for all operations.
     **/
    static const Pathname & defaultRoot() { return _defaultRoot; }

    /**
     * @return Current directory (below root) that contains the rpmdb.
     **/
    static const Pathname & defaultDbPath() { return _defaultDbPath; }

    /**
     * Adjust access to the given database location, making it the new
     * default location on success. No relative Pathnames are allowed.
     *
     * It's not possible to access a database while access is blocked
     * (see @ref blockAccess), but valid Pathnames provided will be stored
     * as new default location.
     *
     * It's not allowed to switch to another location while a database
     * is accessed. Use @ref dbRelease to force releasing the database first.
     *
     * @return @ref PMError
     **/
    static PMError dbAccess( const Pathname & root_r, const Pathname & dbPath_r );

    /**
     * Access the database at the current default location. If necessary
     * (eg. after @ref dbRelease), the database is opened. This usually
     * happens on demand, eg.by creating a @ref db_const_iterator;
     *
     * @return @ref PMError
     **/
    static PMError dbAccess();

    /**
     * Same as &ref dbAccess(). Additionally returns the database handle if
     * avaialble, otherwise NULL. This creates an external reference, thus
     * it should not be used longer than necessary. Be prepared that the
     * handle might become invalid (see @ref dbRelease) later.
     *
     * @return @ref PMError
     **/
    static PMError dbAccess( constlibrpmDbPtr & ptr_r );

    /**
     * If there are no outstanding references to the database (e.g. by @ref db_const_iterator),
     * the database is closed. Subsequent calls to @ref dbAccess may however
     * open the database again.
     *
     * If forced, the internal reference is dropped and it will look like
     * the database was closed. But physically the database will be closed
     * after all outstanding references are gone.
     *
     * @return The number of outstandig references to the database, 0 if
     * if database was physically closed.
     **/
    static unsigned dbRelease( bool force_r = false );

    /**
     * Blocks further access to rpmdb. Basically the same as @ref dbRelease( true ),
     * but subsequent calls to @ref dbAccess will fail returning E_RpmDB_access_blocked.
     *
     * @return The number of outstandig references to the database, 0 if
     * if database was physically closed.
     **/
    static unsigned blockAccess();

    /**
     * Allow access to rpmdb e.g. after @ref blockAccess. Subsequent calls to
     * @ref dbAccess will perform.
     *
     * <B>NOTE:</B> Initially we're in blocked mode. So you must call @ref unblockAccess
     * unblockAccess at least once. Othwise nothing will happen.
     *
     * @return The number of outstandig references to the database, 0 if
     * if database was physically closed.
     **/
    static void unblockAccess();

    /**
     * @return Whether database access is blocked.
     **/
    static bool isBlocked() { return _dbBlocked; }

    /**
     * Dump debug info.
     **/
    static std::ostream & dumpState( std::ostream & str );

  public:

    /**
     * Collect info about what kind of rpmdb seems to be present by
     * looking at paths and filenames.
     **/
    class DbDirInfo;

    /**
     * Subclass to retrieve database content.
     **/
    class db_const_iterator;

  private:
    ///////////////////////////////////////////////////////////////////
    //
    //	internal database handle interface (nonstatic)
    //
    ///////////////////////////////////////////////////////////////////

    /**
     * Hides librpm specific data
     **/
    class D;
    D & _d;

  protected:

    /**
     * Private constructor! librpmDb objects are to be created via
     * static interface only.
     **/
    librpmDb( const Pathname & root_r, const Pathname & dbPath_r, bool readonly_r );

  public:

    /**
     * Destructor. Closes rpmdb.
     **/
    virtual ~librpmDb();

    /**
     * @return This handles root directory for all operations.
     **/
    const Pathname & root() const;

    /**
     * @return This handles directory that contains the rpmdb.
     **/
    const Pathname & dbPath() const;

    /**
     * Return any database error. Usg. if the database was
     * blocked by calling @ref dbRelease(true) or @ref blockAccess.
     **/
    PMError error() const;

    /**
     * @return Whether
     **/
    bool valid() const { return( ! error() ); }

    /**
     * @return True if handle is valid and database is empty.
     **/
    bool empty() const;

  public:

    /**
     * Dont call it ;) It's for development and testing only.
     **/
    void * dont_call_it() const;

    /**
     * Dump debug info.
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : librpmDb::DbDirInfo
/**
 * Collect info about what kind of rpmdb seems to be present by
 * looking at paths and filenames.
 **/
class librpmDb::DbDirInfo {
  friend std::ostream & operator<<( std::ostream & str, const DbDirInfo & obj );

  private:

    /**
     * Root directory for all operations.
     **/
    Pathname _root;

    /**
     * Directory that contains the rpmdb.
     **/
    Pathname _dbPath;

    /**
     * database directory (unset on illegal constructor arguments)
     **/
    PathInfo _dbDir;

    /**
     * rpmV4 database (_dbDir/Packages)
     **/
    PathInfo _dbV4;

    /**
     * rpmV3 database (_dbDir/packages.rpm)
     **/
    PathInfo _dbV3;

    /**
     * rpmV3 database backup created on conversion to rpmV4 (_dbDir/packages.rpm3)
     **/
    PathInfo _dbV3ToV4;

  public:

    /**
     * For Constructor arguments see @ref accessPath. On illegal
     * arguments @ref _dbDir is unset.
     **/
    DbDirInfo( const Pathname & root_r, const Pathname & dbPath_r );

  public:

    /**
     * Root directory for all operations.
     **/
    const Pathname & root() const { return _root; }

    /**
     * Directory that contains the rpmdb.
     **/
    const Pathname & dbPath() const { return _dbPath; }

    /**
     * database directory (unset on illegal constructor arguments)
     **/
    const PathInfo & dbDir() const { return _dbDir; }

    /**
     * rpmV4 database (_dbDir/Packages)
     **/
    const PathInfo & dbV4() const { return _dbV4; }

    /**
     * rpmV3 database (_dbDir/packages.rpm)
     **/
    const PathInfo & dbV3() const { return _dbV3; }

    /**
     * rpmV3 database backup created on conversion to rpmV4 (_dbDir/packages.rpm3)
     **/
    const PathInfo & dbV3ToV4() const { return _dbV3ToV4; }

  public:

    /**
     * Restat all paths
     **/
    void restat();

  public:

    /**
     * Whether constructor arguments were illegal.
     **/
    bool illegalArgs() const { return _dbDir.path().empty(); }

    /**
     * Whether constructor arguments were llegal and dbDir either
     * is a directory or may be created (path does not exist).
     **/
    bool usableArgs() const { return _dbDir.isDir() || ! ( _dbDir.path().empty() || _dbDir.isExist() ); }

    /**
     * Whether dbDir directory exists.
     **/
    bool hasDbDir() const { return _dbDir.isDir(); }

    /**
     * Whether dbV4 file exists.
     **/
    bool hasDbV4() const { return _dbV4.isFile(); }

    /**
     * Whether dbV3 file exists.
     **/
    bool hasDbV3() const { return _dbV3.isFile(); }

    /**
     * Whether dbV3ToV4 file exists.
     **/
    bool hasDbV3ToV4() const { return _dbV3ToV4.isFile(); }
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : librpmDb::db_const_iterator
/**
 * @short Subclass to retrieve database content.
 *
 *
 **/
class librpmDb::db_const_iterator {
  db_const_iterator & operator=( const db_const_iterator & ); // NO ASSIGNMENT!
  db_const_iterator ( const db_const_iterator & );            // NO COPY!
  friend std::ostream & operator<<( std::ostream & str, const db_const_iterator & obj );
  friend class librpmDb;

  private:

    /**
     * Hides librpm specific data
     **/
    class D;
    D & _d;

  public:

    /**
     * Constructor. Iterator is initialized to @ref findAll.
     * The default form accesses librpmDb's default database.
     * Explicitly providing a database handle should not be
     * neccesary, except for testing.
     **/
    db_const_iterator( constlibrpmDbPtr dbptr_r = 0 );

    /**
     * Destructor.
     **/
    ~db_const_iterator();

    /**
     * Return any database error.
     *
     * <B>NOTE:</B> If the database gets blocked (see @ref dbRelease)
     * dbError will immediately report this, but an already running
     * iteration will proceed to its end. Then the database is dropped.
     **/
    PMError dbError() const;

    /**
     * Advance to next constRpmHeaderPtr.
     **/
    void operator++();

    /**
     * Returns the current headers index in database,
     * 0 if no header.
     **/
    unsigned dbHdrNum() const;

    /**
     * Returns the current constRpmHeaderPtr or
     * NULL, if no more entries available.
     **/
    const constRpmHeaderPtr & operator*() const;

    /**
     * Forwards to the current constRpmHeaderPtr.
     **/
    const constRpmHeaderPtr & operator->() const {
      return operator*();
    }

  public:

    /**
     * Reset to iterate all packages. Returns true if iterator
     * contains at least one entry.
     *
     * <B>NOTE:</B> No entry (false) migt be returned due to a
     * meanwhile blocked database (see @ref dbRelease). Use
     * @ref dbError to check this.
     **/
    bool findAll();

    /**
     * Reset to iterate all packages that own a certain file.
     **/
    bool findByFile( const std::string & file_r );

    /**
     * Reset to iterate all packages that provide a certain tag.
     **/
    bool findByProvides( const std::string & tag_r );

    /**
     * Reset to iterate all packages that require a certain tag.
     **/
    bool findByRequiredBy( const std::string & tag_r );

    /**
     * Reset to iterate all packages that conflict with a certain tag.
     **/
    bool findByConflicts( const std::string & tag_r );

    /**
     * Reset to iterate all packages with a certain name.
     *
     * <B>NOTE:</B> Multiple entries for one package installed
     * in different versions are possible but not desired. Usually
     * you'll want to use @ref findPackage instead.
     *
     * findByName is needed to retrieve pseudo packages like
     * 'gpg-pubkey', which in fact exist in multiple instances.
     **/
    bool findByName( const PkgName & name_r );

  public:

    /**
     * Find package by name.
     *
     * Multiple entries for one package installed in different versions
     * are possible but not desired. If so, the last package installed
     * is returned.
     **/
    bool findPackage( const PkgName & name_r );

    /**
     * Find package by name and edition.
     * Commonly used by PMRpmPackageDataProvider.
     **/
    bool findPackage( const PkgName & name_r, const PkgEdition & ed_r );
    /**
     * Abbr. for <code>findPackage( which_r.name, which_r.edition );</code>
     **/
    bool findPackage( const PkgNameEd & which_r ) {
      return findPackage( which_r.name, which_r.edition );
    }
    /**
     * Abbr. for <code>findPackage( which_r->name(), which_r->edition() );</code>
     **/
    bool findPackage( const constPMPackagePtr & which_r );
};

///////////////////////////////////////////////////////////////////

#endif // librpmDb_h

