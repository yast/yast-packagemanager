#include <getopt.h>

#include <iomanip>
#include <list>
#include <string>

#undef  Y2LOG
#define Y2LOG "PM_cschum_test"
#include <y2util/Y2SLog.h>

#include <y2util/Url.h>

#include <y2pm/PMError.h>
#include <y2pm/PMYouPatch.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMYouPatchInfo.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/InstYou.h>

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
  Y2Logging::setLogfileName( "cschum_test.log" );
  MIL << "START" << endl;

  PMYouPatchPtr patch = new PMYouPatch( PkgName( "mypatch" ),
                                        PkgEdition( "1.0" ),
                                        PkgArch( "i386" ),
                                        PMYouPatchDataProviderPtr() );
  
  list<PMYouPatchPtr> patches;
  
  patches.push_back( patch );
  
  Y2PM::youPatchManager().poolAddCandidates( patches );

  MIL << "END" << endl;
  return 0;
}
