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
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/InstYou.h>

#include <Y2PM.h>

using namespace std;

void printRel( PkgEdition left, PkgEdition right )
{
  D__ << left << " <-> " << right << endl;

  cout << left << " ";
  if ( left < right ) { D__ << "is <" << endl; cout << "<"; }
  else if ( left > right ) { D__ << "is >" << endl; cout << ">"; }
  else if ( left == right ) { D__ << "is ==" << endl; cout << "=="; }
  else cout << "[undefined]";
  cout << " " << right << endl;
}

void printEd( PkgEdition ed )
{
  cout << ed << ": version: " << ed.version() << " release: " << ed.release() << endl;
}

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

  PkgEdition one( "1.0-0" );
  printEd( one );
  PkgEdition two( "1.0-1" );
  printEd( two );
  PkgEdition three( "1.1-0" );
  printEd( three );
  
  printRel( one, two );
  printRel( two, two );
  printRel( three, two );

#if 0
  cout << "num: " << Y2PM::youPatchManager().size() << endl;

  const PMYouPatchManager &mgr = Y2PM::youPatchManager();

  PMManager::PMSelectableVec::const_iterator it;
  for ( it = mgr.begin(); it != mgr.end(); ++it ) {
    PMSelectablePtr selectable = *it;
    PMYouPatchPtr installed = selectable->installedObj();
    if ( installed ) {
      cout << installed->name() << endl;
    }
  }
#endif

#if 0
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
