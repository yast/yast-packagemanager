#include <getopt.h>

#include <iomanip>
#include <list>
#include <string>

#undef  Y2LOG
#define Y2LOG "err_test"
#include <y2util/Y2SLog.h>

#include <y2util/Url.h>

#include <y2pm/PMError.h>
#include <y2pm/PMYouPatch.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMYouPatchInfo.h>
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/InstYou.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/Wget.h>
#include <y2pm/MediaError.h>

#include <Y2PM.h>

using namespace std;


/******************************************************************
**
**
**	FUNCTION NAME : main
**	FUNCTION TYPE : int
**
**	DESCRIPTION :
*/
int main( int argc, char **argv )
{
  Y2Logging::setLogfileName( "-" );
  
  PMError err;
  DBG << err << endl;
  
  PMError err2( MediaError::E_error, "details text" );
  DBG << err2 << endl;
  
  err2 = MediaError::E_bad_url;
  DBG << err2 << endl;
}
