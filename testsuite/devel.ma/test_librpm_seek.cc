extern "C" {
#include <rpm/rpmlib.h>
#include <fcntl.h>
typedef struct X_FDSTACK_s {
    FDIO_t		io;
/*@dependent@*/ void *	fp;
    int			fdno;
} XFDSTACK_t;
struct X_FD_s {
/*@refs@*/ int	nrefs;
    int		flags;
    int		magic;
#define	XFDMAGIC			0x04463138
    int		nfps;
    XFDSTACK_t	fps[8];
};
}
#include <iomanip>
#include <fstream>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>

using namespace std;

/******************************************************************
**
**
**	FUNCTION NAME : Ftell
**	FUNCTION TYPE : static long
*/
long Ftell( FD_t fd ) {
    long int rc = -1;

    struct X_FD_s * xfd = (struct X_FD_s*)fd;

    if ( !xfd || xfd->magic != XFDMAGIC) {
      INT << "magic(" << XFDMAGIC << ") failed: " << xfd->magic << endl;
      return -2;
    }

    if ( xfd->fps[xfd->nfps].io == fpio ) { // (fdGetIo(fd) == fpio) { // fd->fps[fd->nfps].io;
	FILE * fp;

	fp = (FILE *)xfd->fps[xfd->nfps].fp; // fdGetFILE(fd); // (FILE *)fd->fps[fd->nfps].fp
	rc = ftell(fp);
	return rc;
    }

    INT << "NO Ftell" << endl;
    return -2;
}

/******************************************************************
**
**
**	FUNCTION NAME : test_file
**	FUNCTION TYPE : static bool
*/
static bool test_file( const Pathname & file_r )
{
#define ERRIF(T) if ( T ) { ERR << "^^^^^^FAILED" << endl; if ( fd ) { ::Fclose( fd ); } return false; }

  FD_t fd = 0;
  long res = 0;

  switch ( PathInfo::zipType( file_r ) ) {
  case PathInfo::ZT_NONE:
    {
      fd = ::Fopen( file_r.asString().c_str(), "r.fdio"  );
      DBG << "PLAIN: open 'r.fdio' " << fd
	<< " (" << ::Fstrerror(fd) << ")" << endl;
    }
    break;
  case PathInfo::ZT_GZ:
    {
      fd = ::Fopen( file_r.asString().c_str(), "r.gzdio"  );
      DBG << "GZIP: open 'r.gzdio' " << fd
	<< " (" << ::Fstrerror(fd) << ")" << endl;
    }
    break;
  case PathInfo::ZT_BZ2:
    if ( 0 ) {
      fd = ::Fopen( file_r.asString().c_str(), "r.bzdio"  );
      DBG << "BZIP2: open 'r.bzdio' " << fd
	<< " (" << ::Fstrerror(fd) << ")" << endl;
    }
    break;
  }

  DBG << "Opened " << fd
    << " (" << ::Fstrerror(fd) << ")" << endl;
  ERRIF ( fd == 0 || ::Ferror(fd) );

  res = ::Ftell( fd );
  DBG << "FTell(0) " << res
    << " (" << ::Fstrerror(fd) << ")" << endl;
  ERRIF ( res != 0 );

  char buf[11];
  buf[10] = '\0';
  res = ::Fread( buf, sizeof(char), 10, fd );
  DBG << "FRead(10) " << res
    << " (" << ::Fstrerror(fd) << ")" << endl;
  DBG << "FRead('123456789\\n') '" << buf << "'" << endl;
  ERRIF ( res != 10 || string(buf) != "123456789\n" );

  res = ::Ftell( fd );
  DBG << "FTell(10) " << res
    << " (" << ::Fstrerror(fd) << ")" << endl;
  ERRIF ( res != 10 );

  res = ::Fseek( fd, 5, SEEK_SET );
  DBG << "FSeek(0) " << res
    << " (" << ::Fstrerror(fd) << ")" << endl;
  ERRIF ( res != 0 );

  buf[1] = '\0';
  res = ::Fread( buf, sizeof(char), 1, fd );
  DBG << "FRead(1) " << res
    << " (" << ::Fstrerror(fd) << ")" << endl;
  DBG << "FRead('6') '" << buf << "'" << endl;
  ERRIF ( res != 1 || string(buf) != "6" );

  res = ::Ftell( fd );
  DBG << "FTell(6) " << res
    << " (" << ::Fstrerror(fd) << ")" << endl;
  ERRIF ( res != 6 );

  if ( fd ) {
    ::Fclose( fd );
  }
  return true;
}


/******************************************************************
**
**
**	FUNCTION NAME : do_test
**	FUNCTION TYPE : static bool
*/
static bool do_test( const Pathname & file_r )
{
  MIL << "START " << file_r << " ..."  << endl;
  bool ok = test_file( file_r );
  if ( ok ) {
    MIL << "TESTING " << file_r << " OK" << endl;
  } else {
    ERR << "TESTING " << file_r << " FAILED" << endl;
  }
  return ok;
}

int main()
{
  Y2Logging::setLogfileName("-");
  MIL << "START" << endl;

  int err = 0;
  err += ! do_test( "test.plain" );
  err += ! do_test( "test.gz" );
  err += ! do_test( "test.bz2" );

  if ( err ) {
    INT << "FAILED -> " << err << endl;
  } else {
    MIL << "DONE -> " << err << endl;
  }
  return err;
}
