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

  Purpose:

/-*/
#ifndef librpmDb_h
#define librpmDb_h

#include <iosfwd>

#include <y2pm/librpmDbPtr.h>
#include <y2pm/RpmLibHeader.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : librpmDb
/**
 *
 **/
class librpmDb : virtual public Rep {
  REP_BODY(librpmDb);

  private:

    class D;

    D & _d;

  protected:

    /**
     * Private constructor librpmDb objects are to be created via
     * @ref librpmDb::access only.
     **/
    librpmDb();

  public:

    /**
     * Constructor. Open rpmdatabase readonly and return the handle.
     * On error return NULL, and errorcode via argument err_r, if provided.
     *
     * Optional argument root_r may denote an absolute path to be used as root
     * directory for all operations.
     **/
    static constlibrpmDbPtr access( int & err_r, const Pathname & root_r = Pathname() );

    static constlibrpmDbPtr access( const Pathname & root_r = Pathname() ) {
      int dummy;
      return access( dummy, root_r );
    }

    virtual ~librpmDb();

  public:

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : librpmDb::db_const_iterator
    /**
     *
     **/
    class db_const_iterator {
      db_const_iterator & operator=( const db_const_iterator & ); // NO ASSIGNMENT!
      db_const_iterator ( const db_const_iterator & );            // NO COPY!
      friend std::ostream & operator<<( std::ostream & str, const db_const_iterator & obj );
      friend class librpmDb;

      private:

	class D;

	D & _d;

	constlibrpmDbPtr _dbptr;

	constRpmLibHeaderPtr _hptr;

	bool _init( int rpmtag, const void * keyp = NULL, size_t keylen  = 0 );

	void _empty();

	void _clear();

      public:

	/**
	 * Constructor
	 **/
	db_const_iterator( constlibrpmDbPtr dbptr_r );

	/**
	 * Destructor.
	 **/
	~db_const_iterator();

	/**
	 * Advance to next constRpmLibHeaderPtr.
	 **/
	void operator++();

	/**
	 * Returns the current headers index in database,
	 * 0 if no header.
	 **/
	unsigned dbHdrNum() const;

	/**
	 * Returns the current constRpmLibHeaderPtr.
	 **/
	const constRpmLibHeaderPtr & operator*() const {
	  return _hptr;
	}

	/**
	 * Forwards to the current constRpmLibHeaderPtr.
	 **/
	const constRpmLibHeaderPtr & operator->() const {
	  return operator*();
	}

      public:

	/**
	 * Reset to iterate all packages.
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
	bool findPackage( const constPMPackagePtr & which_r ) {
	  if ( !which_r ) {
	    _clear();
	    return false;
	  }
	  return findPackage( which_r->name(), which_r->edition() );
	}
    };

    ///////////////////////////////////////////////////////////////////

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // librpmDb_h

