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
  Y2SLog::setLogfileName( "test_patchinfo.log" );
  MIL << "START" << endl;

  const char *readFile = 0;
  bool getServers = false;

  int c;
  while( 1 ) {
    c = getopt( argc, argv, "hsf:" );
    if ( c < 0 ) break;

    switch ( c ) {
      case 'f':
        readFile = optarg;
        break;
      case 's':
        getServers = true;
        break;
      default:
        cerr << "Error parsing command line." << endl;
      case '?':
      case 'h':
        cout << "Usage: " << argv[0] << " [-f patchfile]" << endl;
        exit( 1 );
    }
  }

  if ( getServers ) {
    PMYouPatchPaths paths( "eins", "zwei", "drei" );

    PMError error = paths.requestServers("http://localhost/you/","suseservers.txt");
    if ( error ) {
      cerr << "requestServers failed: " << error << endl;
      return 1;
    }
    
    list<Url> servers = paths.servers();
    
    list<Url>::const_iterator itServers;
    for( itServers = servers.begin(); itServers != servers.end(); ++itServers ) {
      cout << "URL: " << itServers->asString() << endl;
    }
    
    return 0;
  }

  PMYouPatchInfoPtr patchInfo = new PMYouPatchInfo( "german" );
  list<PMYouPatchPtr> patches;

  PMError error;

  if ( readFile ) {
    error = patchInfo->readFile( "", readFile, patches );
  } else {
    string patchPath =
        "/build/yast2-cvs/yast2/source/packagemanager/testsuite/patches/";

#if 0
    PMYouPatchPathsPtr paths = new PMYouPatchPaths( "eMail-Server", "3.1", "i386" );
#else
    PMYouPatchPathsPtr paths = new PMYouPatchPaths( "Dummy", "1.0", "i386" );
#endif

#if 0
    paths->setPatchUrl( Url( "dir:///" ) );
    paths->setPatchPath( patchPath );
#else
    paths->setPatchUrl( Url( "http://localhost/you/" ) );
#endif

    PMError error = patchInfo->getPatches( paths, patches );
    if ( error != PMError::E_ok ) {
      cerr << error << endl;
      exit( 1 );
    }
  }

  if ( error != PMError::E_ok ) {
    cerr << error << endl;
  } else {

    list<PMYouPatchPtr>::const_iterator it;
    for( it = patches.begin(); it != patches.end(); ++it ) {
      cout << "PATCH: " << (*it)->name() << " (" << (*it)->shortDescription()
           << ")" << endl;
//      (*it)->dumpOn( cout );
      list<PMPackagePtr> packages = (*it)->packages();
      list<PMPackagePtr>::const_iterator it2;
      for( it2 = packages.begin(); it2 != packages.end(); ++it2 ) {
        cout << "  PKG: " << (*it2)->name() << endl;
#if 0
        cout << "    REQUIRES:" << endl;
        list<PkgRelation> relations = (*it2)->requires();
        list<PkgRelation>::const_iterator it3;
        for( it3 = relations.begin(); it3 != relations.end(); ++it3 ) {
          cout << "      " << (*it3).asString() << endl;          
        }
#endif
      }
    }
  }

  MIL << "END" << endl;
  return 0;
}
