extern "C" {
#include <rpm/rpmlib.h>
#include <rpm/rpmdb.h>
#include <rpm/rpmmacro.h>
extern int _hdr_debug;
}
#include <iomanip>
#include <fstream>
#include <string>
#include <list>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>
#include <y2util/stringutil.h>

#include <y2pm/librpmDb.h>
#include <y2pm/RpmHeader.h>

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

unsigned dumpit( librpmDb::db_const_iterator & it )
{
  unsigned cnt = 0;
  WAR << "+++dumpit " << it << endl;
  for ( ; *it; ++it, ++cnt ) {
    WAR << '[' << cnt << "] " << it.dbHdrNum() << ' ' << *it << endl;
  }
  WAR << "---dumpit " << it << endl;
  return cnt;
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
  set_log_filename("-");
  MIL << "BEGIN" << endl;

  _rpmdb_debug = 0;
  _hdr_debug = 0;

  int err = 0;

  ///////////////////////////////////////////////////////////////////
#if 1

  librpmDb::globalInit();


  constlibrpmDbPtr db( librpmDb::access( err ) );
  CHK << "librpmDb::access() -> " << err << endl;


  ///////////////////////////////////////////////////////////////////
  MIL << "END" << endl;
  return 0;
#endif
  ///////////////////////////////////////////////////////////////////

  err = ::rpmReadConfigFiles(NULL, NULL);
  CHK << "::rpmReadConfigFiles() -> " << err << endl;

  err = ::rpmdbOpen( _root.asString().c_str(), &_db, O_RDONLY, 0644 );
  CHK << "::rpmdbOpen(" << _root << ") -> " << err << endl;

  //err = ::rpmdbOpenAll( _db );
  //CHK << "::rpmdbOpenAll() -> " << err << endl;

  ///////////////////////////////////////////////////////////////////

  int hdn[6] = { 1, 2,2, 3,4,5 };

  //rpmdbMatchIterator mi = ::rpmdbInitIterator( _db, rpmTag(RPMDBI_PACKAGES), NULL, 0 );
  //rpmdbMatchIterator mi = ::rpmdbInitIterator( _db, RPMTAG_GROUP, "System/Packages", 0 );
  rpmdbMatchIterator mi = ::rpmdbInitIterator( _db, RPMTAG_BASENAMES, "/bin/sh", 0 );
  CK(mi==0) << "::rpmdbInitIterator(RPMDBI_PACKAGES) -> " << mi << endl;
  MIL << "Count:" << ::rpmdbGetIteratorCount( mi ) << " Offset:" << ::rpmdbGetIteratorOffset( mi ) << endl;

  //err = ::rpmdbSetIteratorRE( mi, RPMTAG_NAME, RPMMIRE_DEFAULT, "r*" );
  //CHK << "::rpmdbSetIteratorRE() -> " << err << endl;
  //err = ::rpmdbSetIteratorRE( mi, RPMTAG_NAME, RPMMIRE_DEFAULT, "*" );
  //CHK << "::rpmdbSetIteratorRE() -> " << err << endl;

  //err = ::rpmdbAppendIterator( mi, hdn, 6 );
  //CHK << "::rpmdbAppendIterator(3) -> " << err << endl;
  //MIL << "Count:" << ::rpmdbGetIteratorCount( mi ) << " Offset:" << ::rpmdbGetIteratorOffset( mi ) << endl;

  //err = ::rpmdbPruneIterator( mi, hdn, 1, 0 );
  //CHK << "::rpmdbPruneIterator(3) -> " << err << endl;
  //MIL << "Count:" << ::rpmdbGetIteratorCount( mi ) << " Offset:" << ::rpmdbGetIteratorOffset( mi ) << endl;

  unsigned cnt = 0;
  for ( Header h = ::rpmdbNextIterator( mi ); h; h = ::rpmdbNextIterator( mi ) ) {
    DBG << h << " Count:" << ::rpmdbGetIteratorCount( mi ) << " Offset:" << ::rpmdbGetIteratorOffset( mi ) << endl;
    ++cnt;
  }
  MIL << "Packages:" << cnt << endl;

  mi = ::rpmdbFreeIterator( mi );
  CK(mi!=0) << "::rpmdbFreeIterator() -> " << mi << endl;

  ///////////////////////////////////////////////////////////////////

  err = ::rpmdbClose( _db );
  CHK << "::rpmdbClose() -> " << err << endl;

  MIL << "END" << endl;
  return 0;
}


