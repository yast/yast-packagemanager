#include <getopt.h>

#include <iomanip>
#include <list>
#include <string>

#undef  Y2LOG
#define Y2LOG "PM_you_test"
#include <y2util/Y2SLog.h>

#include <y2util/Url.h>

#include <y2pm/PMError.h>
#include <y2pm/PMYouPatch.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMYouPatchInfo.h>
#include <y2pm/MediaAccess.h>

using namespace std;

void usage()
{
  cout << "Usage: online_update [-u url] [-p product] [-v version]"
       << "[-a arch] [security] [recommended] [document]"
       << " [optional]"
       << endl;
  exit( 1 );
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
  Y2SLog::setLogfileName( "test_you.log" );
  MIL << "START" << endl;

  const char *urlStr = 0;
  const char *productStr = 0;
  const char *versionStr = 0;
  const char *archStr = 0;
  const char *langStr = 0;

  int c;
  while( 1 ) {
    c = getopt( argc, argv, "hu:p:v:a:l:" );
    if ( c < 0 ) break;

    switch ( c ) {
      case 'u':
        urlStr = optarg;
        break;
      case 'p':
        productStr = optarg;
        break;
      case 'v':
        versionStr = optarg;
        break;
      case 'a':
        archStr = optarg;
        break;
      case 'l':
        langStr = optarg;
        break;
      default:
        cerr << "Error parsing command line." << endl;
      case '?':
      case 'h':
        usage();
    }
  }

  bool security = false;
  bool recommended = false;
  bool document = false;
  bool optional = false;

  bool custom = false;

  if (optind < argc) {
    while (optind < argc) {
      string arg = argv[optind++];
      if ( arg == "security" ) security = true;
      else if ( arg == "recommended" ) recommended = true;
      else if ( arg == "document" ) document = true;
      else if ( arg == "optional" ) optional = true;
      else usage();
      
      custom = true;
    }
  }

  if ( !custom ) {
    security = true;
    recommended = true;
  }

  cout << "Types of patches to be installed:";
  if ( security ) cout << " security";
  if ( recommended ) cout << " recommended";
  if ( document ) cout << " document";
  if ( optional ) cout << " optional";
  cout << endl;

  PMError error;

  // Set parameters.
  
  string lang;
  if ( langStr ) lang = langStr;
  else lang = "de";

  string product;
  if ( productStr ) product = productStr;
  else product = "SuSE-Linux";

  string version;
  if ( versionStr ) version = versionStr;
  else version = "8.1";
  
  string arch;
  if ( archStr ) arch = archStr;
  else arch = "i386";

  PMYouPatchInfo patchInfo( lang );
  PMYouPatchPaths *patchPaths = new PMYouPatchPaths( product, version, arch );

  cout << "Product:      " << product << endl;
  cout << "Version:      " << version << endl;
  cout << "Architecture: " << arch << endl;
  cout << "Language:     " << lang << endl;


  // Get URL of you source.

  Url url;

  if ( urlStr ) {
    url = Url( urlStr );
    if ( !url.isValid() ) {
      cerr << "Error: URL '" << urlStr << "' is not valid." << endl;
      exit( 1 );
    }
  } else {
    error = patchPaths->requestServers();
    if ( error ) {
      cerr << "Error while requesting servers: " << error << endl;
      exit( 1 );
    }
    url = patchPaths->defaultServer();
  }
  
  cout << "URL: " << url.asString() << endl;

  patchPaths->setPatchUrl( url );
  

  // Download patch infos.

  list<PMYouPatchPtr> patches;
  error = patchInfo.getPatches( patchPaths, patches );
  if ( error ) {
    cerr << "Error downloading patchinfos: " << error << endl;
    exit( 1 );
  }

  // Select patches.

  // If Yast2 patch, only install this one.

  bool yastPatch = false;

  list<PMYouPatchPtr> selectedPatches;
  list<PMYouPatchPtr>::const_iterator itPatch;
  for( itPatch = patches.begin(); itPatch != patches.end(); ++itPatch ) {
    if ( (*itPatch)->kind() == PMYouPatch::kind_yast ) {
      selectedPatches.push_back( *itPatch );
      yastPatch = true;
    }
  }

  if ( !yastPatch ) {
    for( itPatch = patches.begin(); itPatch != patches.end(); ++itPatch ) {
      int kind = (*itPatch)->kind();
      if ( kind == PMYouPatch::kind_security && security ||
           kind == PMYouPatch::kind_recommended && recommended ||
           kind == PMYouPatch::kind_document && document ||
           kind == PMYouPatch::kind_optional && optional ) {
        selectedPatches.push_back( *itPatch );
      }
    }
  }


  // Download packages.

  MediaAccess media;
  error = media.open( patchPaths->patchUrl(), patchPaths->localDir() );
  if ( error ) {
    cerr << "Error opening URL '" << patchPaths->patchUrl() << "'" << endl;
    exit( 1 );
  }
  error = media.attach();
  if ( error ) {
    cerr << "Error attaching media." << endl;
    exit( 1 );
  }

  for( itPatch = selectedPatches.begin(); itPatch != selectedPatches.end();
       ++itPatch ) {
//    cout << "PATCH: " << (*itPatch)->name() << endl;
    list<PMPackagePtr> packages = (*itPatch)->packages();
    list<PMPackagePtr>::const_iterator itPkg;
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
      Pathname rpmPath = patchPaths->rpmPath( *itPkg );
//      cout << "  RPM: " << (*itPkg)->name() << ": " << rpmPath.asString() << endl;
      error = media.provideFile( rpmPath );
      if ( error ) {
        cerr << "Error downloading RPM '" << (*itPkg)->name() << "' from '"
             << patchPaths->patchUrl() << "/" << rpmPath << "'" << endl;
        exit( 1 );
      }
    }
  }


  // Install packages.

  for( itPatch = selectedPatches.begin(); itPatch != selectedPatches.end();
       ++itPatch ) {
    cout << "INSTALL: " << (*itPatch)->name() << endl;
    list<PMPackagePtr> packages = (*itPatch)->packages();
    list<PMPackagePtr>::const_iterator itPkg;
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
      cout << "  rpm -i --force --nodeps "
           << media.localPath( patchPaths->rpmPath( *itPkg ) ) << endl;
    }
  }


  MIL << "END" << endl;
  return 0;
}
