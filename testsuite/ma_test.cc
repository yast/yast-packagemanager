

#include <iomanip>
#include <list>
#include <string>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>

#include <y2pm/InstSrcManager.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/MediaAccess.h>

#include "show_pm.h"

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
  Y2Logging::setLogfileName("-");
  MIL << "START" << endl;

  Y2PM y2pm;
  InstSrcManager& MGR = y2pm.instSrcManager();

  Url url( "dir:///8.0" );

  InstSrcManager::ISrcIdList nids;
  PMError err = MGR.scanMedia( nids, url );
  SEC << err << endl;
  SEC << nids.size() << endl;

  if ( nids.size() ) {
    err = MGR.enableSource( *nids.begin() );
    SEC << "enable: " <<  err << endl;
  }

  // hack to get InstSrcPtr:
  //
  // InstSrcPtr enabled_souce( const_cast<InstSrc*>((*nids.begin()).operator->()) );


  MIL << "END" << endl;
  return 0;


  SEC << "=================================" << endl;

  INT << Y2PM::packageManager().size() << endl;
  PMSelectablePtr p = Y2PM::packageManager()["aaa_base"];
  INT << p << endl;
  show_pmpackage (p->installedObj());

  SEC << "=================================" << endl;
  //Y2PM::packageManager().poolRemoveCandidates( plist );

  MIL << "END" << endl;
  Y2PM::packageManager().REINIT();
  //rpmdb = 0;
  return 0;
}
