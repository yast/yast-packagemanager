extern "C" {
#include <rpm/rpmlib.h>
#include <rpm/rpmdb.h>
extern int _hdr_debug;
}
#include <iomanip>
#include <fstream>
#include <string>
#include <list>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>
#include <y2util/stringutil.h>

using namespace std;

static Pathname _root;
static rpmdb    _db = 0;

#define CK(C) ((C)?ERR:DBG)
#define CHK CK(err)

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const Header & h )
{
  if ( !h )
    return str << "HDR[nil]";

  const char * n;
  const char * v;
  const char * r;
  ::headerNVR( h, &n, &v, &r );

  str << "HDR["<<(void*)h<< "|" << n << "-" <<  v << "-" << r << "]";
  return str;
}

/******************************************************************
**
**
**	FUNCTION NAME : main
**	FUNCTION TYPE : int
**
**	DESCRIPTION :
*/
int main()
{
  Y2Logging::setLogfileName("-");
  MIL << "BEGIN" << endl;

  _rpmdb_debug = -1;
  _hdr_debug = -1;

  int err = 0;

  ///////////////////////////////////////////////////////////////////

  //err = ::rpmReadConfigFiles(NULL, NULL);
  //CHK << "::rpmReadConfigFiles() -> " << err << endl;

  err = ::rpmdbOpen( _root.asString().c_str(), &_db, O_RDONLY, 0644 );
  CHK << "::rpmdbOpen(" << _root << ") -> " << err << endl;

  ///////////////////////////////////////////////////////////////////

  rpmdbMatchIterator mi = ::rpmdbInitIterator( _db, rpmTag(RPMDBI_PACKAGES), NULL, 0 );
  CK(mi==0) << "::rpmdbInitIterator(RPMDBI_PACKAGES) -> " << mi << endl;

  MIL << "Count:" << ::rpmdbGetIteratorCount( mi ) << " Offset:" << ::rpmdbGetIteratorOffset( mi ) << endl;

  for ( Header h = ::rpmdbNextIterator( mi ); h; h = ::rpmdbNextIterator( mi ) ) {
    DBG << h << " Count:" << ::rpmdbGetIteratorCount( mi ) << " Offset:" << ::rpmdbGetIteratorOffset( mi ) << endl;
  }

  mi = ::rpmdbFreeIterator( mi );
  CK(mi!=0) << "::rpmdbFreeIterator() -> " << mi << endl;

  ///////////////////////////////////////////////////////////////////

  err = ::rpmdbClose( _db );
  CHK << "::rpmdbClose() -> " << err << endl;

  MIL << "END" << endl;
  return 0;
}


