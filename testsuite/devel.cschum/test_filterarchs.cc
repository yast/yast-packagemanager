#include <getopt.h>

#include <iomanip>
#include <list>
#include <string>

#undef  Y2LOG
#define Y2LOG "PM_patchinfo_test"
#include <y2util/Y2SLog.h>

#include <y2pm/PMYouPatch.h>
#include <y2pm/PMYouPatchInfo.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/InstYou.h>
#include <y2pm/PMYouPackageDataProvider.h>
#include <Y2PM.h>

using namespace std;

PMYouPatchPtr patch;
PMYouPackageDataProviderPtr provider;
list<PMPackagePtr> refPackages;
InstYou *you;
bool failed;

void listPackages( const PMYouPatchPtr &patch )
{
  cout << "Patch: " << patch->nameEdArch() << endl;
  
  list<PMPackagePtr> packages = patch->packages();
  list<PMPackagePtr>::const_iterator it;
  for( it = packages.begin(); it != packages.end(); ++it ) {
    cout << "    RPM: " << (*it)->nameEdArch() << endl;
  }
}

void addPkg( const char *name, const char *edition, const char *arch,
             bool discard = false )
{
  PMPackagePtr pkg( new PMPackage( PkgName( name ), PkgEdition( edition ),
                                    PkgArch( arch ), provider ) );
  patch->addPackage( pkg );
  if ( !discard ) refPackages.push_back( pkg );
}


void prepareTest()
{
  patch->clearPackages();
  refPackages.clear();
}

bool finishTest()
{
  list<PMPackagePtr> packages = patch->packages();
  list<PMPackagePtr>::const_iterator it1 = packages.begin();
  list<PMPackagePtr>::const_iterator it2 = refPackages.begin();
  while( it1 != packages.end() && it2 != refPackages.end() ) {
    if ( *it1 != *it2 ) break;
    ++it1;
    ++it2;
  }
  if ( it1 != packages.end() || it2 != refPackages.end() ) {
    cout << "TEST FAILED" << endl;
    return false;
  }

  return true;
}

void doTest()
{
  cout << "  BEFORE: ";
  listPackages( patch );
  you->filterArchitectures( patch );
  cout << "  AFTER: ";
  listPackages( patch );  

  if ( !finishTest() ) failed = true;

  cout << endl;

  prepareTest();
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
  PMYouPatchPathsPtr patchPaths = new PMYouPatchPaths();
  PMYouPatchInfoPtr patchInfo = new PMYouPatchInfo( patchPaths );

  provider = new PMYouPackageDataProvider( patchInfo );

  you = new InstYou( patchInfo, patchPaths );

  list<PkgArch> archs;
  archs.push_back( PkgArch( "i686" ) );
  archs.push_back( PkgArch( "i586" ) );
  archs.push_back( PkgArch( "i486" ) );
  archs.push_back( PkgArch( "i386" ) );
  archs.push_back( PkgArch( "i286" ) );
  archs.push_back( PkgArch( "noarch" ) );
  patchPaths->setArchs( archs );
  
  patch = new PMYouPatch( PkgName( "mypatch" ), PkgEdition( "1" ),
                          PkgArch( "i386" ) );

  failed = false;

  addPkg( "one",   "1-0", "i786", true );
  addPkg( "two",   "1-0", "i586", true );
  addPkg( "two",   "1-0", "i686" );
  addPkg( "three", "1-0", "noarch", true );
  addPkg( "three", "1-0", "i686" );
  doTest();


  list<PMPackagePtr> installedPackages;
  installedPackages.push_back( new PMPackage( PkgName( "rpm" ),
                                              PkgEdition( "1-0" ),
                                              PkgArch( "i586" ),
                                              provider ) );
  Y2PM::packageManager().poolSetInstalled( installedPackages );

  addPkg( "rpm",   "2-0", "i586" );
  addPkg( "rpm",   "2-0", "i686", true );
  doTest();

  if ( failed ) cout << "FAILED" << endl;
  else cout << "PASSED" << endl;

  if ( failed ) return 1;
  else return 0;
}
