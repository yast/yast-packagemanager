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

  File:       binHeaderCache.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/Date.h>
#include <y2util/stringutil.h>

#include <y2pm/binHeaderCache.h>
#include <y2pm/binHeader.h>
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

using namespace std;

#undef Y2LOG
#define Y2LOG "binHeaderCache"

/******************************************************************
**
**
**	FUNCTION NAME : gzipped
**	FUNCTION TYPE : bool
*/
static bool gzipped( const Pathname & file )
{
  bool ret = false;
  int fd = open( file.asString().c_str(), O_RDONLY );
  if ( fd != -1 ) {
    const int magicSize = 2;
    unsigned char magic[magicSize];
    if ( read( fd, magic, magicSize ) == magicSize ) {
      if ( magic[0] == 0037 && magic[1] == 0213 ) {
	ret = true;
      }
    }
    close( fd );
  }
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : binHeaderCache::Cache
/**
 *
 **/
class binHeaderCache::Cache {

  friend std::ostream & operator<<( std::ostream & str, const Cache & obj );

  Cache & operator=( const Cache & );
  Cache            ( const Cache & );

  private:

    FD_t  fd;

  public:

    Cache() : fd( 0 ) {}
    ~Cache() { close(); }

  public:

    bool open( const Pathname & file_r );

    bool isOpen() const { return( fd != 0 ); }

    void close();

  public:

    pos tell() const;

    pos seek( const pos pos_r );

    unsigned readData( void * buf_r, unsigned count_r );

    Header readHeader( bool magicp = true );
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::Cache::open
//	METHOD TYPE : bool
//
bool binHeaderCache::Cache::open( const Pathname & file_r )
{
  close();

  if ( gzipped( file_r ) ) {
    FD_t fd1 = ::Fopen( file_r.asString().c_str(), "r.ufdio"  );
    if ( fd1 == 0 || ::Ferror(fd1) ) {
      ERR << "Can't open cache for reading: " << file_r << " (" << ::Fstrerror(fd1) << ")" << endl;
      if ( fd1 )
	::Fclose( fd1 );
      return false;
    }
    fd = ::Fdopen( fd1, "r.gzdio"  );
  } else {
    fd = ::Fopen( file_r.asString().c_str(), "r"  );
  }

  if ( fd == 0 || ::Ferror(fd) ) {
    ERR << "Can't open cache for reading: " << file_r << " (" << ::Fstrerror(fd) << ")" << endl;
    close();
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::Cache::close
//	METHOD TYPE : void
//
void binHeaderCache::Cache::close()
{
  if ( fd ) {
    ::Fclose( fd );
    fd = 0;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::Cache::tell
//	METHOD TYPE : pos
//
binHeaderCache::pos binHeaderCache::Cache::tell() const
{
  pos rc = npos;

  FILE * fd_fp = (FILE *)fdGetFp( fd );
  if ( fd_fp ) {
    rc = ::ftell( fd_fp );
  } else {
    int fd_fd = ::Fileno( fd );
    if ( fd_fd >= 0 ) {
      rc = ::lseek( fd_fd, 0, SEEK_CUR );
    }
  }

  if ( rc == npos )
    WAR << "Can't tell:" << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;
  return rc;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::Cache::seek
//	METHOD TYPE : pos
//
binHeaderCache::pos binHeaderCache::Cache::seek( const pos pos_r )
{
  pos rc = npos;

  if ( pos_r != npos ) {
    ::Fseek( fd, pos_r, SEEK_SET );
    if ( tell() == pos_r )
      rc = pos_r;
  } else {
    INT << "Attempt to seek to pos -1" << endl;
  }

  if ( rc == npos )
    WAR << "Can't seek to " << pos_r << ":" << ::Ferror(fd) << " (" << ::Fstrerror(fd) << ")" << endl;
  return rc;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::Cache::readData
//	METHOD TYPE : unsigned
//
unsigned binHeaderCache::Cache::readData( void * buf_r, unsigned count_r )
{
  if ( !buf_r ) {
    INT << "Attempt to fill NULL buffer" << endl;
    return 0;
  }
  if ( !count_r ) {
    return 0;
  }

  unsigned got = ::Fread( buf_r, sizeof(char), count_r, fd );
  if ( got != count_r ) {
    if ( got || ::Ferror(fd) ) {
      ERR << "Error reading " << count_r << " byte (" << ::Fstrerror(fd) << ")" << endl;
    } // else EOF?
    return 0;
  }
  return count_r;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::Cache::readHeader
//	METHOD TYPE : Header
//
Header binHeaderCache::Cache::readHeader( bool magicp )
{
  static const int_32 rpm_header_magic = 0x01e8ad8e;

  int_32 block[4];
  int_32 il, dl;
  unsigned totalSize = 0;

  unsigned count = (magicp ? 4 : 2) * sizeof(int_32);
  if ( readData( block, count ) != count ) {
    ERR << "Error reading header info (" << ::Fstrerror(fd) << ")" << endl;
    return 0;
  }

  count = 0;

  if ( magicp ) {
    if ( block[count] != rpm_header_magic ) {
      ERR << "Error bad header magic " << stringutil::hexstring( block[count] )
	<< " (" << stringutil::hexstring( rpm_header_magic ) << ")" << endl;
      return 0;
    }
    count += 2;
  }

  il = ntohl( block[count++] );
  dl = ntohl( block[count++] );

  totalSize = (2*sizeof(int_32)) + (il * sizeof(struct entryInfo)) + dl;
  if (totalSize > (32*1024*1024)) {
    ERR << "Error header ecxeeds 32Mb limit (" << totalSize << ")" << endl;
    return NULL;
  }

  char * data = new char[totalSize];
  int_32 * p = (int_32 *)data;
  Header h = 0;

  *p++ = htonl(il);
  *p++ = htonl(dl);
  totalSize -= (2*sizeof(int_32));

  if ( readData( p, totalSize ) != totalSize ) {
    ERR << "Error reading header data (" << ::Fstrerror(fd) << ")" << endl;
  } else {
    h = ::headerLoad( data );
    if ( !h ) {
      ERR << "Error loading header data" << endl;
    }
  }

  delete [] data;

  return h;
}


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : binHeaderCache
//
///////////////////////////////////////////////////////////////////

const unsigned binHeaderCache::BHC_MAGIC_SZE( 64 );

const binHeaderCache::pos binHeaderCache::npos( binHeaderCache::pos(-1) );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::binHeaderCache
//	METHOD TYPE : Constructor
//
binHeaderCache::binHeaderCache( const Pathname & cache_r )
    : _c( * new Cache )
    , _cpath( cache_r )
    , _cdate( 0 )
    , _cheaderStart( npos )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::~binHeaderCache
//	METHOD TYPE : Destructor
//
binHeaderCache::~binHeaderCache()
{
  delete &_c;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::_cReadMagic
//	METHOD TYPE : int
//
//      [string\0][string\0][\0padded]
//
int binHeaderCache::_cReadMagic()
{
  char magic[BHC_MAGIC_SZE+1];
  memset( magic, 0, BHC_MAGIC_SZE+1 );

  if ( _c.readData( magic, BHC_MAGIC_SZE ) != BHC_MAGIC_SZE ) {
    ERR << "Error reading magic of cache file " << _cpath  << endl;
    return -1;
  }

  _cmagic = magic;
  if ( _cmagic.size() < BHC_MAGIC_SZE ) {
    _cdate = strtoul( magic+_cmagic.size()+1, 0, 10 );
    if ( _cdate ) {
      _cheaderStart = BHC_MAGIC_SZE;
      return 0;
    }
  }

  ERR << "No magic in cache file " << _cpath  << endl;
  return -2;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::open
//	METHOD TYPE : bool
//
bool binHeaderCache::open()
{
  if ( _c.isOpen() )
    return true;

  if ( !_c.open( _cpath ) ) {
    close();
    return false;
  }

  if ( _cReadMagic() != 0 ) {
    close();
    return false;
  }

  if ( !magicOk() ) {
    ERR << "Bad magic in cache file " << _cpath  << endl;
    close();
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::isOpen
//	METHOD TYPE : bool
//
bool binHeaderCache::isOpen() const
{
  return _c.isOpen();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::close
//	METHOD TYPE : void
//
void binHeaderCache::close()
{
  _cmagic       = "";
  _cdate        = 0;
  _cheaderStart = npos;
  _c.close();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::tell
//	METHOD TYPE : binHeaderCache::pos
//
binHeaderCache::pos binHeaderCache::tell() const
{
  return _c.tell();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::seek
//	METHOD TYPE : binHeaderCache::pos
//
binHeaderCache::pos binHeaderCache::seek( const pos pos_r )
{
  return _c.seek( pos_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::readData
//	METHOD TYPE : unsigned
//
unsigned binHeaderCache::readData( void * buf_r, unsigned count_r )
{
  return _c.readData( buf_r, count_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : binHeaderCache::readHeader
//	METHOD TYPE : binHeaderPtr
//
binHeaderPtr binHeaderCache::readHeader( bool magicp )
{
  Header h = _c.readHeader( magicp );
  if ( !h )
    return 0;
  return new binHeader( h );
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const binHeaderCache & obj )
{
  str << "binHeaderCache@" << (void *)&obj;
  if ( obj.isOpen() ) {
    str << '(' << obj._cmagic << '|' << Date(obj._cdate) << "|at " << obj._cheaderStart << ')';
  } else {
    str << "(closed)";
  }
  return str;
}

