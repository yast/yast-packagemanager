

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
  M__ << "Y2SLOG_DEBUG IS ON" << endl;
  MIL << "START" << endl;

  InstSrcManager & MGR( Y2PM::instSrcManager() );

  Url url( "dir:////Local/tmp/UL_test" );

  InstSrcManager::ISrcIdList nids;
  PMError err = MGR.scanMedia( nids, url );
  SEC << err << endl;
  SEC << nids.size() << endl;

  if ( nids.size() ) {
    err = MGR.enableSource( *nids.begin() );
    SEC << "enable: " <<  err << endl;
  }

  MIL << "END " << endl;
  return 0;
}
