#include <iostream>
#include <string>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>
#include <y2util/stringutil.h>

#include <y2pm/RpmHeaderCache.h>
#include <y2pm/RpmHeader.h>

extern "C" {
extern int _rpmdb_debug;
extern int _hdr_debug;
}
using namespace std;

#undef  Y2LOG
#define Y2LOG "genIS_PLAINcache"

#include <Y2PM.h>
const PM::ArchSet & Y2PM::allowedArchs(void) {
  static PM::ArchSet a;
  return a;
}

/******************************************************************
**
**
**	FUNCTION NAME : main
**	FUNCTION TYPE : int
*/
int main( int argc, char *argv[] )
{
  _rpmdb_debug = -1;
  _hdr_debug = 0;
  set_log_filename("-");
  --argc;
  ++argv;

  if ( !argc ) {
    ERR << "testIS_PLAINcache: missing cachefile" << endl;
    ERR << "exit(1)" << endl;
    return 1;
  }

  Pathname cachefile( argv[0] );
  PathInfo pi( cachefile );
  if ( ! pi.isFile() ) {
    ERR << "testIS_PLAINcache: not a file: " << pi << endl;
    ERR << "exit(2)" << endl;
    return 2;
  }

  MIL << "Scan Cache...." << cachefile << endl;
  RpmHeaderCache cache( cachefile );
  DBG << cache << endl;

  if ( cache.open() ) {
    MIL << "open: " << cache << endl;
  } else {
    ERR << "open: " << cache << endl;
    ERR << "exit(3)" << endl;
    return 3;
  }

  unsigned entry = 0;
  unsigned hpos;
  Pathname pkgfile;
  int      isSource;
  for ( constRpmHeaderPtr iter = cache.getFirst( pkgfile, isSource, hpos );
	iter; iter = cache.getNext( pkgfile, isSource, hpos ) ) {
    MIL << "Entry " << entry++ << endl;
    DBG << "  Pos " << hpos << endl;
    DBG << "      " << iter << endl;
    DBG << "      " << pkgfile << "   (" << (isSource?"src)":"bin)") << endl;
  }

  MIL << "Scan Cache found " << entry << " packages in " << cachefile << endl;
  return 0;
}
