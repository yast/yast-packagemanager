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

  File:       RpmLibDb.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Wraps acces to rpmdb via librpm.

/-*/
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
}

#include "RpmLib.h"

#include <iostream>
#include <fstream>
#include <map>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>
#include <y2util/PathInfo.h>

#include <y2pm/RpmLibDb.h>
#include <y2pm/RpmLibHeader.h>
#include <y2pm/PMPackage.h>

using namespace std;

#undef Y2LOG
#define Y2LOG "RpmLib"

///////////////////////////////////////////////////////////////////

bool RpmLibDb::_globalInitialized = false;

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : RpmLibDb::const_header_set
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : RpmLibDb::const_header_set::index_set
/**
 * Wrapper for librpm struct dbiIndexSet to handle necessary dbiFreeIndexRecord calls.
 **/
class RpmLibDb::const_header_set::index_set {
  index_set            ( const index_set & ); // no copy
  index_set & operator=( const index_set & ); // no assign
  public:
    std::vector<unsigned> _idxSet;
  public:
    index_set( dbiIndexSet & idxSet_r ) {
      if ( idxSet_r.count ) {
	_idxSet.resize( idxSet_r.count );
	for ( int i = 0; i < idxSet_r.count; ++i ) {
	  _idxSet[i] = idxSet_r.recs[i].recOffset;
	}
      }
      ::dbiFreeIndexRecord( idxSet_r );
      idxSet_r.count = 0;
      idxSet_r.recs = 0;
    }
};


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::const_header_set::const_header_set
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
RpmLibDb::const_header_set::const_header_set( rpmdb dbptr_r, const index_set & idxSet_r )
    : _dbptr( dbptr_r )
{
  if ( _dbptr ) {
    _idxSet = idxSet_r._idxSet;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::const_header_set::operator[]
//	METHOD TYPE : constRpmLibHeaderPtr
//
//	DESCRIPTION :
//
constRpmLibHeaderPtr RpmLibDb::const_header_set::operator[]( const unsigned idx_r ) const
{
  if ( _dbptr && idx_r < size() ) {
    Header nh = ::rpmdbGetRecord( _dbptr, _idxSet[idx_r] );
    if ( nh )
      return new RpmLibHeader( nh );
    else
      ERR << "Bad record number " << _idxSet[idx_r] << " in rpmdb" << endl;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : RpmLibDb::const_iterator
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::const_iterator::const_iterator
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
RpmLibDb::const_iterator::const_iterator( rpmdb dbptr_r )
    : _hptr( 0 )
    , _dbptr( dbptr_r )
    , _recnum( 0 )
{
  if ( _dbptr )
    setrec( ::rpmdbFirstRecNum( _dbptr ) );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::const_iterator::setrec
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void RpmLibDb::const_iterator::setrec( int recnum_r )
{
  if ( recnum_r > 0 ) {
    _recnum  = recnum_r;
    Header nh = ::rpmdbGetRecord( _dbptr, _recnum );
    if ( nh )
      _hptr = new RpmLibHeader( nh );
    else {
      _hptr = 0;
      ERR << "Bad record number " << _recnum << " in rpmdb" << endl;
    }
  } else {
    _recnum = 0;
    _dbptr = 0;
    _hptr = 0;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::const_iterator::operator++
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void RpmLibDb::const_iterator::operator++()
{
  if ( _dbptr )
    setrec( ::rpmdbNextRecNum( _dbptr, _recnum ) );
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : RpmLibDb
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::RpmLibDb
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
RpmLibDb::RpmLibDb( const Pathname & dbPath_r, const bool no_open_r )
    : _dbPath( dbPath_r )
    , _db( 0 )
    , _dbOpenError( Error::E_RpmLib_db_not_open )
{
  if ( !_globalInitialized ) {
    globalInit();
  }
  if ( ! no_open_r ) {
    dbOpen();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::~RpmLibDb
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
RpmLibDb::~RpmLibDb()
{
  dbClose();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::globalInit
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError RpmLibDb::globalInit()
{
  if ( _globalInitialized ) {
    return Error::E_ok;
  }

  int rc = ::rpmReadConfigFiles( 0, 0 );
  if ( rc ) {
    ERR << "rpmReadConfigFiles returned: " << rc << endl;
    return Error::E_RpmLib_read_config_failed;
  }

  _globalInitialized = true;

  MIL << "globalInit " << Error::E_ok << endl;
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::dbOpen
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError RpmLibDb::dbOpen()
{
  if ( _db ) {
    return _dbOpenError; // is Error::E_ok
  }

  _dbOpenError = globalInit();
  if ( _dbOpenError ) {
    ERR << *this << " globalInit returned: " << _dbOpenError << endl;
    return _dbOpenError;
  }

  ::addMacro(NULL, "_dbpath", NULL, _dbPath.asString().c_str(), RMIL_CMDLINE);

  int rc = ::rpmdbOpen( 0, &_db, O_RDONLY, 0644 );
  if ( rc ) {
    ERR << *this << " rpmdbOpen returned: " << rc << endl;
    _dbOpenError = Error::E_RpmLib_dbopen_failed; // set it before calling dbClose
    dbClose();
    return _dbOpenError;
  }

  return _dbOpenError; // is Error::E_ok
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::dbClose
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError RpmLibDb::dbClose()
{
  if ( _db ) {
    ::rpmdbClose( _db );
    _db = 0;
    if ( !_dbOpenError )
      _dbOpenError = Error::E_RpmLib_db_not_open;
  }
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findByFile
//	METHOD TYPE : RpmLibDb::const_header_set
//
//	DESCRIPTION :
//
RpmLibDb::const_header_set RpmLibDb::findByFile( const std::string & file_r ) const
{
  if ( ! file_r[0] == '/' )
    return const_header_set();

  dbiIndexSet idxSet;
  switch ( ::rpmdbFindByFile( _db, file_r.c_str(), &idxSet ) ) {
  case 0:
    return const_header_set( _db, idxSet );

  case -1:
    WAR << "Error reading a database record" << endl;
  }
  return const_header_set();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findByProvides
//	METHOD TYPE : RpmLibDb::const_header_set
//
//	DESCRIPTION :
//
RpmLibDb::const_header_set RpmLibDb::findByProvides( const std::string & tag_r ) const
{
  dbiIndexSet idxSet;
  switch ( ::rpmdbFindByProvides( _db, tag_r.c_str(), &idxSet ) ) {
  case 0:
    return const_header_set( _db, idxSet );

  case -1:
    WAR << "Error reading a database record" << endl;
  }
  return const_header_set();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findByRequiredBy
//	METHOD TYPE : RpmLibDb::const_header_set
//
//	DESCRIPTION :
//
RpmLibDb::const_header_set RpmLibDb::findByRequiredBy( const std::string & tag_r ) const
{
  dbiIndexSet idxSet;
  switch ( ::rpmdbFindByRequiredBy( _db, tag_r.c_str(), &idxSet ) ) {
  case 0:
    return const_header_set( _db, idxSet );

  case -1:
    WAR << "Error reading a database record" << endl;
  }
  return const_header_set();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findByConflicts
//	METHOD TYPE : RpmLibDb::const_header_set
//
//	DESCRIPTION :
//
RpmLibDb::const_header_set RpmLibDb::findByConflicts( const std::string & tag_r ) const
{
  dbiIndexSet idxSet;
  switch ( ::rpmdbFindByConflicts( _db, tag_r.c_str(), &idxSet ) ) {
  case 0:
    return const_header_set( _db, idxSet );

  case -1:
    WAR << "Error reading a database record" << endl;
  }
  return const_header_set();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findAllPackages
//	METHOD TYPE : RpmLibDb::const_header_set
//
//	DESCRIPTION :
//
RpmLibDb::const_header_set RpmLibDb::findAllPackages( const PkgName & name_r ) const
{
  dbiIndexSet idxSet;
  switch ( ::rpmdbFindPackage( _db, name_r->c_str(), &idxSet ) ) {
  case 0:
    return const_header_set( _db, idxSet );

  case -1:
    WAR << "Error reading a database record" << endl;
  }
  return const_header_set();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
//	DESCRIPTION :
//
constRpmLibHeaderPtr RpmLibDb::findPackage( const PkgName & name_r ) const
{
  constRpmLibHeaderPtr h;
  const_header_set result( findAllPackages( name_r ) );
  for ( unsigned i = 0; i < result.size(); ++i ) {
    if ( !h || h->tag_installtime() < result[i]->tag_installtime() ) {
      h = result[i];
    }
  }
  return h;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
//	DESCRIPTION :
//
constRpmLibHeaderPtr RpmLibDb::findPackage( const PkgName & name_r, const PkgEdition & ed_r ) const
{
  const_header_set result( findAllPackages( name_r ) );
  for ( unsigned i = 0; i < result.size(); ++i ) {
    constRpmLibHeaderPtr h = result[i];
    if ( ed_r == h->tag_edition() )
      return h;
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
//	DESCRIPTION :
//
constRpmLibHeaderPtr RpmLibDb::findPackage( const PkgNameEd & which_r ) const
{
  return findPackage( which_r.name, which_r.edition );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::findPackage
//	METHOD TYPE : constRpmLibHeaderPtr
//
//	DESCRIPTION :
//
constRpmLibHeaderPtr RpmLibDb::findPackage( const constPMPackagePtr & which_r ) const
{
  if ( !which_r )
    return 0;
  return findPackage( which_r->name(), which_r->edition() );
}


/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const RpmLibDb & obj )
{
  str << "RpmDb(" << obj._dbPath << '|';
  if ( obj._db )
    str << "open";
  else
    str << (obj._globalInitialized ? "closed" : "noinit" );

  return str << ')';
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibDb::getData
//	METHOD TYPE : constRpmLibHeaderPtr
//
constRpmLibHeaderPtr RpmLibDb::getData( const Pathname & path )
{
  PathInfo file( path );
  if ( ! file.isFile() ) {
    ERR << "Not a file: " << file << endl;
    return 0;
  }

  FD_t fd = ::Fopen( file.asString().c_str(), "r" );
  if ( fd == 0 || ::Ferror(fd) ) {
    ERR << "Can't open file for reading: " << file << " (" << ::Fstrerror(fd) << ")" << endl;
    if ( fd )
      ::Fclose( fd );
    return 0;
  }

  Header nh    = 0;
  int isSource = 0;
  int major    = 0;
  int minor    = 0;

  int res = ::rpmReadPackageHeader( fd, &nh, &isSource, &major, &minor );
  ::Fclose( fd );
  if ( res || !nh ) {
    ERR << "Error reading: " << file << (res==1?" (bad magic)":"") << endl;
    return 0;
  }

  constRpmLibHeaderPtr h( new RpmLibHeader( nh, isSource ) );
  MIL << major << "." << minor << "-" << (isSource?"src ":"bin ") << h << " from " << path << endl;

  return h;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgHeaderCache
//
///////////////////////////////////////////////////////////////////
#undef Y2LOG
#define Y2LOG "PkgHeaderCache"

static const unsigned PHC_MAGIC_SZE = 64; // dont change!
static const string   PHC_MAGIC( "YaST-PHC-1.0-0" );

/******************************************************************
**
**
**	FUNCTION NAME : phcAddMagic
**	FUNCTION TYPE : void
*/
void phcAddMagic( FD_t fd )
{
  char magic[PHC_MAGIC_SZE];
  memset( magic, 0, PHC_MAGIC_SZE );
  strcpy( magic, PHC_MAGIC.c_str() );
  strcpy( magic+PHC_MAGIC.size()+1, stringutil::numstring( Date::now() ).c_str() );

  ::Fwrite( magic, sizeof(char), PHC_MAGIC_SZE, fd );
}

/******************************************************************
**
**
**	FUNCTION NAME : phcReadMagic
**	FUNCTION TYPE : void
*/
int phcReadMagic( FD_t fd, time_t & date_r )
{
  char magic[PHC_MAGIC_SZE+1];
  memset( magic, 0, PHC_MAGIC_SZE+1 );
  size_t got = ::Fread( magic, sizeof(char), PHC_MAGIC_SZE, fd );
  if ( got != PHC_MAGIC_SZE ) {
    return -1;
  }
  if ( strcmp( magic, PHC_MAGIC.c_str() ) ) {
    return -2;
  }
  date_r = strtoul( magic+PHC_MAGIC.size()+1, 0, 10 );
  return 0;
}

/******************************************************************
**
**
**	FUNCTION NAME : phcAddHeader
**	FUNCTION TYPE : unsigned
*/
unsigned phcAddHeader( FD_t fd, Header h, const Pathname & citem_r, int isSource )
{
  string entry = stringutil::form( "%c%s", (isSource?'s':'b'), citem_r.asString().c_str() );
  entry = stringutil::form( "@%6d@%s", entry.size(), entry.c_str() );

  ::Fwrite( entry.c_str(), sizeof(char), entry.size(), fd );
  ::headerWrite( fd, h, HEADER_MAGIC_YES );

  return 1;
}

/******************************************************************
**
**
**	FUNCTION NAME : phcReadHeader
**	FUNCTION TYPE : Header
*/
Header phcReadHeader( FD_t fd, unsigned & at_r )
{
  at_r = ::Fseek( fd, 0, SEEK_CUR );
  Header h = ::headerRead( fd, HEADER_MAGIC_YES );
  if ( !h ) {
    ERR << "Error reading header (" << ::Fstrerror(fd) << ")" << endl;
  }
  return h;
}

/******************************************************************
**
**
**	FUNCTION NAME : phcGetHeader
**	FUNCTION TYPE : Header
*/
Header phcGetHeader( FD_t fd, Pathname & citem_r, int & isSource_r, unsigned & at_r )
{
  char sig[] = "xxxxxxxx";

  size_t got = ::Fread( sig, sizeof(char), 8, fd );
  if ( got != 8 ) {
    if ( got || ::Ferror(fd) ) {
      ERR << "Error reading entry (" << ::Fstrerror(fd) << ")" << endl;
    }
    return 0;
  }

  if ( sig[0] != '@' || sig[7] != '@' ) {
    ERR << "Invalid entry." << endl;
    return 0;
  }

  sig[7] = '\0';
  unsigned count = atoi( &sig[1] );

  char citem[count+1];
  if ( ::Fread( citem, sizeof(char), count, fd ) != count ) {
    ERR << "Error reading entry data (" << ::Fstrerror(fd) << ")" << endl;
    return 0;
  }
  citem[count] = '\0';

  isSource_r = ( citem[0] == 's' );
  citem_r    = &citem[1];

  return phcReadHeader( fd, at_r );
}

/******************************************************************
**
**
**	FUNCTION NAME : phcAddFile
**	FUNCTION TYPE : unsigned
*/
unsigned phcAddFile( FD_t fd, const PathInfo & cpath_r, const Pathname & citem_r )
{
  FD_t pkg = ::Fopen( cpath_r.asString().c_str(), "r" );
  if ( pkg == 0 || ::Ferror(pkg) ) {
    ERR << "Can't open file for reading: " << cpath_r << " (" << ::Fstrerror(pkg) << ")" << endl;
    if ( pkg )
      ::Fclose( pkg );
    return 0;
  }

  Header h     = 0;
  int isSource = 0;
  int major    = 0;
  int minor    = 0;

  int res = ::rpmReadPackageHeader( pkg, &h, &isSource, &major, &minor );
  ::Fclose( pkg );

  if ( res || !h ) {
    WAR << "Error reading: " << cpath_r << (res==1?" (bad magic)":"") << endl;
    return 0;
  }

  constRpmLibHeaderPtr dummy( new RpmLibHeader( h, isSource ) ); // to handle header free
  MIL << major << "." << minor << "-" << (isSource?"src ":"bin ") << dummy << " for " << citem_r << endl;

  return phcAddHeader( fd, h, citem_r, isSource );
}

/******************************************************************
**
**
**	FUNCTION NAME : phcScanDir
**	FUNCTION TYPE : unsigned
*/
unsigned phcScanDir( FD_t fd, const PathInfo & cpath_r, const Pathname & prfx_r,
		     const PkgHeaderCache::buildOpts & options_r )
{
  DBG << "SCAN " << cpath_r << " (" << prfx_r << ")" << endl;

  list<string> retlist;
  int res = PathInfo::readdir( retlist, cpath_r.path(), false );
  if ( res ) {
    ERR << "Error reading content of " << cpath_r << " (readdir " << res << ")" << endl;
    return 0;
  }

  unsigned count = 0;
  list<string> downlist;

  for ( list<string>::const_iterator it = retlist.begin(); it != retlist.end(); ++it ) {
    PathInfo cpath( cpath_r.path() + *it, PathInfo::LSTAT );
    if ( cpath.isFile() ) {
      count += phcAddFile( fd, cpath, prfx_r + *it );
    } else if ( options_r.recurse && cpath.isDir() ) {
      downlist.push_back( *it );
    }
  }
  retlist.clear();

  for ( list<string>::const_iterator it = downlist.begin(); it != downlist.end(); ++it ) {
    count += phcScanDir( fd, cpath_r.path() + *it, prfx_r + *it, options_r );
  }

  return count;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgHeaderCache::buildPkgHeaderCache
//	METHOD TYPE : int
//
int PkgHeaderCache::buildPkgHeaderCache( const Pathname & cache_r,
					 const Pathname & pkgroot_r,
					 const buildOpts & options_r )
{
  ///////////////////////////////////////////////////////////////////
  // Check pkgroot
  ///////////////////////////////////////////////////////////////////

  PathInfo pkgroot( pkgroot_r );
  if ( !pkgroot.isDir() ) {
    ERR << "Not a directory: Pkgroot " << pkgroot << endl;
    return -1;
  }

  ///////////////////////////////////////////////////////////////////
  // Prepare cache file
  ///////////////////////////////////////////////////////////////////
  FD_t fd = ::Fopen( cache_r.asString().c_str(), "w"  );
  if ( fd == 0 || ::Ferror(fd) ) {
    ERR << "Can't open cache for writing: " << cache_r << " (" << ::Fstrerror(fd) << ")" << endl;
    if ( fd )
      ::Fclose( fd );
    return -2;
  }

  phcAddMagic( fd );

  ///////////////////////////////////////////////////////////////////
  // Scan pkgroot_r
  ///////////////////////////////////////////////////////////////////
  MIL << "Start scan below " << pkgroot_r
    << " (recurse=" << (options_r.recurse?"yes":"no")
    << ")" << endl;
  unsigned count = phcScanDir( fd, pkgroot_r, "/", options_r );

  if ( ::Ferror(fd) ) {
    ERR << "Error writing cache: " << cache_r << " (" << ::Fstrerror(fd) << ")" << endl;
    ::Fclose( fd );
    return -3;
  }

  MIL << "Found " << count << " package(s) below " << pkgroot_r << endl;
  ::Fclose( fd );
  return count;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgHeaderCache::Cache
//
struct PkgHeaderCache::Cache {

  FD_t   fd;
  time_t _cdate;

  void close() {
    if ( fd )
      ::Fclose( fd );
    fd = 0;
    _cdate = 0;
  }

  bool open( const Pathname & file_r ) {
    close();
    fd = ::Fopen( file_r.asString().c_str(), "r"  );
    if ( fd == 0 || ::Ferror(fd) ) {
      ERR << "Can't open cache for reading: " << file_r << " (" << ::Fstrerror(fd) << ")" << endl;
      close();
      return false;
    }
    if ( phcReadMagic( fd, _cdate ) < 0 ) {
      ERR << "Not a cache file. (" << ::Fstrerror(fd) << ")" << endl;
      close();
      return false;
    }
    return true;
  }

  bool isOpen() const { return( fd != 0 ); }

  Cache() : fd( 0 ), _cdate( 0 ) {}

  ~Cache() { close(); }
};
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgHeaderCache::PkgHeaderCache
//	METHOD TYPE : Constructor
//
PkgHeaderCache::PkgHeaderCache( const Pathname & cache_r )
    : _cpath( cache_r )
    , _c( new Cache )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgHeaderCache::~PkgHeaderCache
//	METHOD TYPE : Destructor
//
PkgHeaderCache::~PkgHeaderCache()
{
  delete _c;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgHeaderCache::openCache
//	METHOD TYPE : bool
//
bool PkgHeaderCache::openCache()
{
  if ( _c->isOpen() )
    return true;
  return _c->open( _cpath );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgHeaderCache::closeCache
//	METHOD TYPE : void
//
void PkgHeaderCache::closeCache()
{
  _c->close();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgHeaderCache::cacheOk
//	METHOD TYPE : bool
//
bool PkgHeaderCache::cacheOk() const
{
  if ( !_c->isOpen() ) {
    DBG << "Cache not open: " << _cpath << endl;
    return false;
  }
  if ( ::Ferror(_c->fd) ) {
    INT << "Ferror: " << ::Fstrerror(_c->fd) << endl;
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgHeaderCache::cacheCdate
//	METHOD TYPE : time_t
//
time_t PkgHeaderCache::cacheCdate() const
{
  return _c->_cdate;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgHeaderCache::getFirst
//	METHOD TYPE : constRpmLibHeaderPtr
//
constRpmLibHeaderPtr PkgHeaderCache::getFirst( Pathname & citem_r, int & isSource_r, unsigned & at_r )
{
  if ( !_c->isOpen() ) {
    ERR << "Cache not open: " << _cpath << endl;
    return 0;
  }

  ::Fseek( _c->fd, PHC_MAGIC_SZE, SEEK_SET );
  return getNext( citem_r, isSource_r, at_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgHeaderCache::getNext
//	METHOD TYPE : constRpmLibHeaderPtr
//
constRpmLibHeaderPtr PkgHeaderCache::getNext( Pathname & citem_r, int & isSource_r, unsigned & at_r )
{
  if ( !_c->isOpen() ) {
    ERR << "Cache not open: " << _cpath << endl;
    return 0;
  }

  Header h = phcGetHeader( _c->fd, citem_r, isSource_r, at_r );
  if ( !h ) {
    return 0;
  }
  return new RpmLibHeader( h );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgHeaderCache::getAt
//	METHOD TYPE : constRpmLibHeaderPtr
//
constRpmLibHeaderPtr PkgHeaderCache::getAt( unsigned at_r )
{
  if ( !_c->isOpen() ) {
    ERR << "Cache not open: " << _cpath << endl;
    return 0;
  }

  ::Fseek( _c->fd, at_r, SEEK_SET );
  Header h = phcReadHeader( _c->fd, at_r );
  if ( !h ) {
    return 0;
  }
  return new RpmLibHeader( h );
}
