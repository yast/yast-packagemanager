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

  Purpose:

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
 *
 **/
class RpmLibDb {

  friend std::ostream & operator<<( std::ostream & str, const RpmLibDb & obj );

  RpmLibDb & operator=( const RpmLibDb & );
  RpmLibDb            ( const RpmLibDb & );

  public:

    /**
     * default error class
     **/
    typedef InstTargetError Error;

  private:

    static bool _globalInitialized;

    static PMError globalInit();

  private:

    Pathname _dbPath;

    rpmdb    _db;

  public:

    RpmLibDb( const Pathname & dbPath_r );
    ~RpmLibDb();

  public:

    PMError dbOpen();

    PMError dbClose();

    bool isOpen() const { return _db; }

    const Pathname & dbPath() const { return _dbPath; }

  public:

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
     *
     **/
    class const_header_set {

      friend class RpmLibDb;

      private:

	rpmdb                 _dbptr;
	std::vector<unsigned> _idxSet;

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

	const_header_set()
	  : _dbptr( 0 )
	{}

	unsigned size() const { return _idxSet.size(); }

	constRpmLibHeaderPtr operator[]( const unsigned idx_r ) const;
    };

    ///////////////////////////////////////////////////////////////////

    const_header_set findByFile( const std::string & which_r ) const;

    constRpmLibHeaderPtr findPackage( const PkgName & name_r ) const;

    constRpmLibHeaderPtr findPackage( const PkgName & name_r, const PkgEdition & ed_r ) const;
    constRpmLibHeaderPtr findPackage( const PkgNameEd & which_r ) const;
    constRpmLibHeaderPtr findPackage( const constPMPackagePtr & which_r ) const;

  public:

    ///////////////////////////////////////////////////////////////////
    //
    //	CLASS NAME : RpmLibDb::const_iterator
    /**
     *
     **/
    class const_iterator {

      friend class RpmLibDb;

      private:

	mutable constRpmLibHeaderPtr _hptr;

	rpmdb    _dbptr;
	unsigned _recnum;

	void setrec( int recnum_r );

	const_iterator( rpmdb dbptr_r )
	  : _hptr( 0 ), _dbptr( dbptr_r ), _recnum( 0 )
	{
	  if ( _dbptr )
	    setrec( ::rpmdbFirstRecNum( _dbptr ) );
	}

      public:

	const_iterator()
	  : _hptr( 0 ), _dbptr( 0 ), _recnum( 0 )
	{}

	void operator++() {
	  if ( _dbptr )
	    setrec( ::rpmdbNextRecNum( _dbptr, _recnum ) );
	}

	bool operator==( const const_iterator & rhs ) const {
	  return( _dbptr == rhs._dbptr && _recnum == rhs._recnum );
	}

	bool operator!=( const const_iterator & rhs ) const {
	  return !operator==( rhs );
	}

	constRpmLibHeaderPtr & operator*() const {
	  return _hptr;
	}

	constRpmLibHeaderPtr & operator->() const {
	  return operator*();
	}

    };
    ///////////////////////////////////////////////////////////////////

    const_iterator begin() const { return const_iterator( _db ); }
    const_iterator end()   const { return const_iterator(); }

};

///////////////////////////////////////////////////////////////////

#endif // RpmLibDb_h
