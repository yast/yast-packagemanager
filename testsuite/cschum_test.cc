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

  int numPkgs = Y2PM::packageManager().size();
  cout << "Number of packages: " << numPkgs << endl;

  InstYou you;
  you.retrievePatches( Url( "http://localhost/you/" ) );

  int numPatches  = Y2PM::youPatchManager().size();
  cout << "Number of patches: " << numPatches << endl;

  numPkgs = Y2PM::packageManager().size();
  cout << "Number of packages: " << numPkgs << endl;

#if 0
  int numPkgs = Y2PM::packageManager().size();

  cout << "Number of packages: " << numPkgs << endl;

  PMSelectablePtr selectable = Y2PM::packageManager().getItem( "gdb" );

  if ( selectable ) {
    PMObjectPtr obj = selectable->installedObj();
    if ( obj ) {
      PMPackagePtr pkg( obj );
    
      cout << "NAME: " << pkg->name() << endl;
      cout << "VERSION: " << pkg->version() << endl;
    }
  }
#endif

  MIL << "END" << endl;
  return 0;
}
