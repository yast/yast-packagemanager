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
#include <y2pm/InstYou.h>

using namespace std;

void usage()
{
  cout << "Usage: online-update [-u url] [-p product] [-v version]"
       << "[-a arch] [-d] [security] [recommended] [document]"
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

  bool dryrun = false;

  int c;
  while( 1 ) {
    c = getopt( argc, argv, "hdu:p:v:a:l:" );
    if ( c < 0 ) break;

    switch ( c ) {
      case 'd':
        dryrun = true;
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

  int kinds = PMYouPatch::kind_invalid;

  if (optind < argc) {
    while (optind < argc) {
      string arg = argv[optind++];
      if ( arg == "security" ) kinds |= PMYouPatch::kind_security;
      else if ( arg == "recommended" ) kinds |= PMYouPatch::kind_recommended;
      else if ( arg == "document" ) kinds |= PMYouPatch::kind_document;
      else if ( arg == "optional" ) kinds |= PMYouPatch::kind_optional;
      else if ( arg == "all" ) {
        kinds = PMYouPatch::kind_all;
        break;
      } else usage();
    }
  }

  if ( kinds == PMYouPatch::kind_invalid ) {
    kinds = PMYouPatch::kind_security | PMYouPatch::kind_recommended;
  }

  cout << "Types of patches to be installed:";
  if ( kinds & PMYouPatch::kind_security ) cout << " security";
  if ( kinds & PMYouPatch::kind_recommended ) cout << " recommended";
  if ( kinds & PMYouPatch::kind_document ) cout << " document";
  if ( kinds & PMYouPatch::kind_optional ) cout << " optional";
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

  PMYouPatchInfoPtr patchInfo = new PMYouPatchInfo( lang );
  PMYouPatchPathsPtr patchPaths = new PMYouPatchPaths( product, version, arch );

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

  InstYou you( patchInfo, patchPaths );

  error = you.retrievePatches( url );
  if ( error ) {
    cerr << "Error retrieving patches: " << error << endl;
    exit( 1 );
  }

  you.selectPatches( kinds );

  you.filterPatchSelection();

  error = you.retrievePackages();
  if ( error ) {
    cerr << "Error retrieving packages: " << error << endl;
    exit( 1 );
  }

  error = you.installPatches( dryrun );
  if ( error ) {
    cerr << "Error installing packages: " << error << endl;
    exit( 1 );
  }

  MIL << "END" << endl;
  return 0;
}
