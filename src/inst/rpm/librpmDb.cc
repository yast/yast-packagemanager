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

  File:       librpmDb.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

extern "C" {
#include <rpm/rpmlib.h>
#include <rpm/rpmdb.h>
#include <rpm/rpmmacro.h>
}

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>

#include <y2pm/librpmDb.h>
#include <y2pm/RpmLibHeader.h>

using namespace std;

#undef Y2LOG
#define Y2LOG "RpmLib"

///////////////////////////////////////////////////////////////////
//	CLASS NAME : librpmDbPtr
//	CLASS NAME : constlibrpmDbPtr
///////////////////////////////////////////////////////////////////
IMPL_BASE_POINTER(librpmDb);

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : librpmDb
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : librpmDb::D
/**
 *
 **/
class librpmDb::D {
  D & operator=( const D & ); // NO ASSIGNMENT!
  D ( const D & );            // NO COPY!
  public:

    rpmdb _db;

    D() : _db( 0 ) {
    }

    ~D() {
      if ( _db ) {
	int err = ::rpmdbClose( _db );
	if ( err ) {
	  WAR << "::rpmdbClose() returned error(" << err << ")" << endl;
	}
      }
    }
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::librpmDb
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
librpmDb::librpmDb()
    : _d( * new D )
{
  INT << "DBACCESS " << this << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::~librpmDb
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
librpmDb::~librpmDb()
{
  INT << "DBCLOSE " << this << endl;
  delete &_d;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & librpmDb::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::access
//	METHOD TYPE : constlibrpmDbPtr
//
constlibrpmDbPtr librpmDb::access( int & err_r, const Pathname & root_r )
{
  if ( root_r.relative() ) {
    ERR << "Illegal root prefix '" << root_r << "'" << endl;
    err_r = 999;
    return 0;
  }

  const char * root = ( root_r.empty() ? NULL : root_r.asString().c_str() );
  rpmdb _db = 0;

  err_r = ::rpmdbOpen( root, &_db, O_RDONLY, 0644 );

  if ( err_r || !_db ) {
    ERR << "::rpmdbOpen(" << root_r << ") returned " << (void*)_db << " error(" << err_r << ")" << endl;
    if ( _db ) {
      ::rpmdbClose( _db );
    } else if ( ! err_r ) {
      err_r = 998;
    }
    return 0;
  }

  librpmDb * ndb = new librpmDb;
  ndb->_d._db = _db;

  err_r = 0;
  return ndb;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : librpmDbPtr::db_const_iterator
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : librpmDb::db_const_iterator::D
/**
 *
 **/
class librpmDb::db_const_iterator::D {
  D & operator=( const D & ); // NO ASSIGNMENT!
  D ( const D & );            // NO COPY!
  public:

    rpmdbMatchIterator _mi;

    D() : _mi( 0 ) {
    }

    ~D() {
      if ( _mi ) {
	::rpmdbFreeIterator( _mi );
      }
    }
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::db_iterator
//	METHOD TYPE : Constructor
//
librpmDb::db_const_iterator::db_const_iterator( constlibrpmDbPtr dbptr_r )
    : _d( * new D )
    , _dbptr( dbptr_r )
{
  if ( ! _dbptr )
    return;

  if ( ! _init( RPMDBI_PACKAGES ) ) {
    DBG << "Empty database" << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::~db_const_iterator
//	METHOD TYPE : Destructor
//
librpmDb::db_const_iterator::~db_const_iterator()
{
  delete &_d;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::_init
//	METHOD TYPE : bool
//
bool librpmDb::db_const_iterator::_init( int rpmtag, const void * keyp, size_t keylen )
{
  _clear();

  if ( ! _dbptr )
    return false;

  _d._mi = ::rpmdbInitIterator( _dbptr->_d._db, rpmTag(rpmtag), keyp, keylen );
  if ( ! _d._mi )
    return false;

  operator++();

  return _hptr;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::_empty
//	METHOD TYPE : void
//
void librpmDb::db_const_iterator::_empty()
{
  _clear();

  if ( ! _dbptr )
    return;

  _d._mi = ::rpmdbInitIterator( _dbptr->_d._db, rpmTag(RPMDBI_PACKAGES), NULL, 0 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::_clear
//	METHOD TYPE : void
//
void librpmDb::db_const_iterator::_clear()
{
  if ( ! _d._mi )
    return;

  _d._mi = ::rpmdbFreeIterator( _d._mi );
  _hptr = 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::operator++
//	METHOD TYPE : void
//
void librpmDb::db_const_iterator::operator++()
{
  if ( ! _d._mi )
    return;

  Header h = ::rpmdbNextIterator( _d._mi );
  if ( ! h ) {
    _clear();
    return;
  }

  _hptr = new RpmLibHeader( new binHeader( h ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::dbHdrNum
//	METHOD TYPE : unsigned
//
unsigned librpmDb::db_const_iterator::dbHdrNum() const
{
  return ::rpmdbGetIteratorOffset( _d._mi );
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const librpmDb::db_const_iterator & obj )
{
  str << "db_const_iterator(" << obj._dbptr;
  if ( obj._d._mi ) {
    str << " Count:" << ::rpmdbGetIteratorCount( obj._d._mi )
      << " HdrNum:" << ::rpmdbGetIteratorOffset( obj._d._mi );
  } else {
    str << " Empty";
  }
  return str << ")";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::findAll
//	METHOD TYPE : bool
//
bool librpmDb::db_const_iterator::findAll()
{
  return _init( RPMDBI_PACKAGES );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::findByFile
//	METHOD TYPE : bool
//
bool librpmDb::db_const_iterator::findByFile( const std::string & file_r )
{
  return _init( RPMTAG_BASENAMES, file_r.c_str() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::findByProvides
//	METHOD TYPE : bool
//
bool librpmDb::db_const_iterator::findByProvides( const std::string & tag_r )
{
  return _init( RPMTAG_PROVIDENAME, tag_r.c_str() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::findByRequiredBy
//	METHOD TYPE : bool
//
bool librpmDb::db_const_iterator::findByRequiredBy( const std::string & tag_r )
{
  return _init( RPMTAG_REQUIRENAME, tag_r.c_str() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::findByConflicts
//	METHOD TYPE : bool
//
bool librpmDb::db_const_iterator::findByConflicts( const std::string & tag_r )
{
  return _init( RPMTAG_CONFLICTNAME, tag_r.c_str() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::findPackage
//	METHOD TYPE : bool
//
bool librpmDb::db_const_iterator::findPackage( const PkgName & name_r )
{
  if ( ! _init( RPMTAG_NAME, name_r->c_str() ) )
    return false;
  if ( ::rpmdbGetIteratorCount( _d._mi ) == 1 )
    return true;

  int match = 0;
  time_t itime = 0;
  for ( ; operator*(); operator++() ) {
    if ( operator*()->tag_installtime() > itime ) {
      match = dbHdrNum();
      itime = operator*()->tag_installtime();
    }
  }

  _empty();
  ::rpmdbAppendIterator( _d._mi, &match, 1 );
  operator++();
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : librpmDb::db_const_iterator::findPackage
//	METHOD TYPE : bool
//
bool librpmDb::db_const_iterator::findPackage( const PkgName & name_r, const PkgEdition & ed_r )
{
  if ( ! _init( RPMTAG_NAME, name_r->c_str() ) )
    return false;

  for ( ; operator*(); operator++() ) {
    if ( ed_r == operator*()->tag_edition() ) {
      int match = dbHdrNum();
      _empty();
      ::rpmdbAppendIterator( _d._mi, &match, 1 );
      operator++();
      return true;
    }
  }
  _clear();
  return false;
}

