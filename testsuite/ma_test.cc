

#include <iomanip>
#include <list>
#include <string>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>

#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/MediaAccess.h>

using namespace std;
#undef  Y2LOG
#define Y2LOG "PM_ma_test"

inline string dec( unsigned i ) {
  static char b[5];
  sprintf( b, "%u", i );
  return b;
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
  Y2SLog::setLogfileName("-");
  MIL << "START" << endl;

  InstSrcPtr nsrc;

  Url      url     ( "dir:///8.0" );  // media
  Pathname proddir ( "" );            // product dir
  Pathname cache   ( "/tmp/tcache" ); // cachedir (must not exist)

  PMError err = InstSrc::vconstruct( nsrc, cache, url, proddir, InstSrc::T_UnitedLinux );

  SEC << err << endl;
  SEC << nsrc << endl;

  MIL << "END" << endl;
  return 0;


  SEC << "=================================" << endl;

  INT << Y2PM::packageManager().size() << endl;
  PMSelectablePtr p = Y2PM::packageManager()["aaa_base"];
  INT << p << endl;
  INT << p->installedObj()->getAttributeValue(p->installedObj()->ATTR_DESCRIPTION) << endl;
  PMPackagePtr P = p->installedObj();
  INT << P->getAttributeValue(P->ATTR_DESCRIPTION) << endl;
  INT << P->getAttributeValue(P->ATTR_INSTALLTIME) << endl;
  INT << P->getAttributeValue(P->ATTR_NAME) << endl;
  INT << p << endl;

  SEC << "=================================" << endl;
  //Y2PM::packageManager().poolRemoveCandidates( plist );

  MIL << "END" << endl;
  Y2PM::packageManager().REINIT();
  //rpmdb = 0;
  return 0;
}
