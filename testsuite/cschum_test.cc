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

  cout << PMYouPatchInfo::translateLangCode( LangCode( "de" ) ) << endl;
  cout << PMYouPatchInfo::translateLangCode( LangCode( "argl" ) ) << endl;

#if 1
  list<PMYouPatchPtr> patches = Y2PM::instTarget().getPatches();

  list<PMYouPatchPtr>::const_iterator it;
  for( it = patches.begin(); it != patches.end(); ++it ) {
    cout << "PATCH: " << (*it)->name() << " (" << (*it)->shortDescription()
         << ")" << endl;
  }
#endif

  MIL << "END" << endl;
  return 0;
}
