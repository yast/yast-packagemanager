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

#include <iosfwd>
#include <vector>
#include <list>

#include <y2util/Pathname.h>

#include <y2pm/InstTargetError.h>
#include <y2pm/librpmDb.h>
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
    constlibrpmDbPtr _db;

    /**
     * Remember dbOpen error.
     **/
    PMError _dbOpenError;

  public:

    /**
     * Constructor. Path to RPM database is passed as argument.
     * globalInit() is called if necessary. RPM database is
     * opened, if no_open_r is false (default).
     *
     * Call @ref dbOpenError to test whether the database is accessible.
     **/
    RpmLibDb( const Pathname & dbPath_r, const bool no_open_r = false );

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
     * Returns <code>E_ok</code> if RPM database is open. Otherwise
     * the error returned by the last call to @ref dbOpen, or
     * <code>E_RpmLib_db_not_open</code> after call to @ref dbClose.
     **/
    PMError dbOpenError() const { return _dbOpenError; }

    /**
     * Returns path to RPM database.
     **/
    const Pathname & dbPath() const { return _dbPath; }

  public:

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

    /**
     * Return true if at least one package owns a certain file.
     **/
    bool hasFile( const std::string & file_r ) const;

    /**
     * Return true if at least one package provides a certain tag.
     **/
    bool hasProvides( const std::string & tag_r ) const;

    /**
     * Return true if at least one package requires a certain tag.
     **/
    bool hasRequiredBy( const std::string & tag_r ) const;

    /**
     * Return true if at least one package conflicts with a certain tag.
     **/
    bool hasConflicts( const std::string & tag_r ) const;

    /**
     * Return true if package is installed.
     **/
    bool hasPackage( const PkgName & name_r ) const;

  public:

    /**
     * Get an accessible packages data from disk. Data returned via
     * result are NULL on any error.
     **/
    static constRpmLibHeaderPtr getData( const Pathname & path );
};

///////////////////////////////////////////////////////////////////

#endif // RpmLibDb_h
