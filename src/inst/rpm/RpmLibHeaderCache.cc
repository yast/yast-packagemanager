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

  File:       RpmLibHeaderCache.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PkgName.h>
#include <y2pm/RpmLibHeaderCache.h>
#include <y2pm/RpmLibHeader.h>

using namespace std;

#undef Y2LOG
#define Y2LOG "binHeaderCache"

///////////////////////////////////////////////////////////////////

const PkgNameEd & RpmLibHeaderCache::def_magic()
{
  static PkgNameEd _def_magic( PkgName("YaST-PHC"), PkgEdition("1.0-0") );
  return _def_magic;
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeaderCache::RpmLibHeaderCache
//	METHOD TYPE : Constructor
//
RpmLibHeaderCache::RpmLibHeaderCache( const Pathname & cache_r )
    : binHeaderCache( cache_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeaderCache::~RpmLibHeaderCache
//	METHOD TYPE : Destructor
//
RpmLibHeaderCache::~RpmLibHeaderCache()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeaderCache::magicOk
//	METHOD TYPE : bool
//
bool RpmLibHeaderCache::magicOk()
{
  PkgNameEd magic( PkgNameEd::fromString( _cmagic ) );
  if ( magic != def_magic() ) {
    ERR << "Found magic " << magic << ", expected " << def_magic() << endl;
    return false;
  }
  DBG << "Found magic " << magic << endl;
  return true;
}

///////////////////////////////////////////////////////////////////
#define RETURN_IF_CLOSED(R) if ( !isOpen() ) { ERR << "Cache not open: " << _cpath << endl; return(R); }
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeaderCache::getFirst
//	METHOD TYPE : constRpmLibHeaderPtr
//
constRpmLibHeaderPtr RpmLibHeaderCache::getFirst( Pathname & citem_r, int & isSource_r, pos & at_r )
{
  RETURN_IF_CLOSED( 0 );

  if ( seek( _cheaderStart ) == npos ) {
    ERR << "Can't seek to first header at " << _cheaderStart << endl;
    return 0;
  }

  return getNext( citem_r, isSource_r, at_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeaderCache::getNext
//	METHOD TYPE : constRpmLibHeaderPtr
//
constRpmLibHeaderPtr RpmLibHeaderCache::getNext( Pathname & citem_r, int & isSource_r, pos & at_r )
{
  RETURN_IF_CLOSED( 0 );

  static const unsigned sigsize = 8;

  char sig[sigsize+1];
  sig[sigsize] = '\0';

  unsigned count = readData( sig, sigsize );
  if ( count != sigsize ) {
    if ( count ) {
      ERR << "Error reading entry." << endl;
    } // else EOF?
    return 0;
  }

  if ( sig[0] != '@' || sig[sigsize-1] != '@' ) {
    ERR << "Invalid entry." << endl;
    return 0;
  }

  sig[sigsize-1] = '\0';
  count = atoi( &sig[1] );

  char citem[count+1];
  citem[count] = '\0';

  if ( readData( citem, count ) != count ) {
    ERR << "Error reading entry data." << endl;
    return 0;
  }

  isSource_r = ( citem[0] == 's' );
  citem_r    = &citem[1];
  at_r       = tell();

  return getAt( at_r );
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : RpmLibHeaderCache::getAt
//	METHOD TYPE : constRpmLibHeaderPtr
//
constRpmLibHeaderPtr RpmLibHeaderCache::getAt( pos at_r )
{
  RETURN_IF_CLOSED( 0 );

  if ( seek( at_r ) == npos ) {
    ERR << "Can't seek to header at " << at_r << endl;
    return 0;
  }

  binHeaderPtr bp = readHeader();
  if ( !bp ) {
    ERR << "Can't read header at " << at_r << endl;
    return 0;
  }

  return new RpmLibHeader( bp );
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const RpmLibHeaderCache & obj )
{
  return str << "RpmLibHeaderCache@" << static_cast<const binHeaderCache &>(obj);
}

///////////////////////////////////////////////////////////////////
//
#warning buildHeaderCache needs cleanup
//
///////////////////////////////////////////////////////////////////
#include <y2util/PathInfo.h>
#include "RpmLib.h"
extern "C" {
#include <netinet/in.h>
  // from rpm: lib/header.c
  struct entryInfo {
    int_32 tag;
    int_32 type;
    int_32 offset;              /* Offset from beginning of data segment,
				   only defined on disk */
    int_32 count;
  };
}

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
		     const RpmLibHeaderCache::buildOpts & options_r )
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
//	METHOD NAME : RpmLibHeaderCache::buildHeaderCache
//	METHOD TYPE : int
//
int RpmLibHeaderCache::buildHeaderCache( const Pathname & cache_r,
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

