#include <getopt.h>

#include <iomanip>
#include <list>
#include <string>

#undef  Y2LOG
#define Y2LOG "PM_online_update"
#include <y2util/Y2SLog.h>

#include <y2util/Url.h>

#include <Y2PM.h>

#include <y2pm/PMError.h>
#include <y2pm/PMYouPatch.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMYouPatchInfo.h>
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/InstYou.h>
#include <y2pm/PMYouServers.h>
#include <y2pm/YouError.h>

using namespace std;

void usage()
{
  cout << "Usage: online-update [-u url] [-p product] [-v version] "
       << "[-a arch] [-d] [-s] [-n] [-g] [-i] [security] [recommended] [document]"
       << " [optional]"
       << endl << endl
       << "-u url      Base URL of directory tree used to get patches from." << endl
       << "            Supported protocols: http, ftp, smb, nfs, cd, dvd, dir." << endl
       << "            Examples: 'ftp://ftp.suse.com/pub/suse', 'cd:///'," << endl
       << "                      'dir:///var/lib/YaST2/you/mnt'" << endl
       << endl
       << "-g          Only download patches, don't install." << endl
       << "-i          Install downloaded patches, don't download." << endl
       << endl
       << "-q          Quick check for new updates. Doesn't check for types" << endl
       << "            of updates." << endl
       << "-k          Check for new updates." << endl
       << endl
       << "-c          Show configuration. Don't do anything." << endl
       << endl
       << "-p product  Name of product to get patches for." << endl
       << "-v version  Version of product to get patches for." << endl
       << "-a arch     Base architecture of product to get patches for." << endl
       << endl
       << "-r          Reload patches from server." << endl
       << "-d          Dry run. Only get patches, don't install them." << endl
       << "-n          No signature check of downloaded files." << endl
       << endl
       << "-s          Show list of patches." << endl
       << "-V          Be verbose." << endl
       << "-D          Debug output." << endl
       << endl
       << "security | recommended | document | optional   Types of patches to be installed." << endl;
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
  MIL << "START" << endl;

  const char *urlStr = 0;
  const char *productStr = 0;
  const char *versionStr = 0;
  const char *archStr = 0;
  const char *langStr = 0;

  bool dryrun = false;
  bool checkSig = true;
  bool showPatches = true;
  bool verbose = false;
  bool debug = false;
  bool autoGet = false;
  bool autoInstall = false;
  bool reload = false;
  bool showConfig = false;
  bool checkUpdates = false;
  bool quickCheckUpdates = false;
  
  int c;
  while( 1 ) {
    c = getopt( argc, argv, "qkrcgihdnsVDu:p:v:a:l:" );
    if ( c < 0 ) break;

    switch ( c ) {
      case 'c':
        showConfig = true;
      case 'g':
        autoGet = true;
        break;
      case 'i':
        autoInstall = true;
        break;
      case 'd':
        dryrun = true;
        break;
      case 'n':
        checkSig = false;
        break;
      case 's':
        showPatches = true;
        break;
      case 'V':
        verbose = true;
        break;
      case 'D':
        debug = true;
        break;
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
      case 'k':
        checkUpdates = true;
        break;
      case 'q':
        quickCheckUpdates = true;
        break;
      case 'r':
        reload = true;
        break;
      default:
        cerr << "Error parsing command line." << endl;
      case '?':
      case 'h':
        usage();
    }
  }

  if ( getuid() != 0 && !checkUpdates && !quickCheckUpdates ) {
    cerr << "You need root permissions to run this command. Use the -q or -k\n"
         << "options to check for the availabilty of updates without needing\n"
         << "root permissions." << endl;
    exit( 1 );
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

  if ( verbose ) {
    cout << "Types of patches to be installed:";
    if ( kinds & PMYouPatch::kind_security ) cout << " security";
    if ( kinds & PMYouPatch::kind_recommended ) cout << " recommended";
    if ( kinds & PMYouPatch::kind_document ) cout << " document";
    if ( kinds & PMYouPatch::kind_optional ) cout << " optional";
    cout << endl;
  }

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

  PMYouPatchPathsPtr patchPaths = new PMYouPatchPaths( product, version, arch );
  PMYouPatchInfoPtr patchInfo = new PMYouPatchInfo( patchPaths, lang );

  InstYou you( patchInfo, patchPaths );

  if ( !productStr && !versionStr && !archStr ) {
    you.initProduct();
  }

  if ( showConfig ) {
    cout << "Product:      " << you.paths()->product() << endl;
    cout << "Version:      " << you.paths()->version() << endl;
    cout << "Architecture: " << you.paths()->baseArch() << endl;
    if ( verbose || debug ) {
      cout << "Business Product: "
           << ( you.paths()->businessProduct() ? "Yes" : "No" ) << endl;
      cout << "Distribution: " << you.paths()->distProduct() << endl;
    }
    cout << "Language:     " << you.patchInfo()->langCode() << endl;
    cout << "Directory:    " << you.paths()->directoryFileName() << endl;
  
    exit( 0 );
  }

  // Get URL of you source.

  PMYouServer server;

  if ( autoInstall ) {
    server.url = "dir://" + you.paths()->attachPoint().asString();
  } else {
    if ( urlStr ) {
      if ( !Url( urlStr ).isValid() ) {
        cerr << "Error: URL '" << urlStr << "' is not valid." << endl;
        exit( -1 );
      }
      server.url = urlStr;
    } else {
      PMYouServers youServers( you.paths() );
      error = youServers.requestServers( checkUpdates || quickCheckUpdates );
      if ( error ) {
        cerr << "Error while requesting servers: " << error << endl;
        exit( -1 );
      }
      server = youServers.currentServer();
    }
  }

  patchPaths->setPatchUrl( server.url );

  if ( verbose ) {
    cout << "URL: " << server.url << endl;
    cout << "Path: " << you.paths()->patchPath() << endl;
  }

  if ( quickCheckUpdates ) {
    int updates = you.quickCheckUpdates( server );
    if ( updates < 0 ) {
      cerr << "Unable to check for updates." << endl;
      return -1;
    } else if ( updates == 0 ) {
      cout << "No new updates available." << endl;
      return 0;
    } else if ( updates == 1 ) {
      cout << "1 new update available." << endl;
      return 1;
    } else {
      cout << updates << " new updates available." << endl;
      return 1;
    }
  }

  error = you.retrievePatchDirectory( server );
  if ( error ) {
    if ( error == MediaError::E_login_failed ) {
      you.readUserPassword();
      error = you.retrievePatchDirectory( server );
      if ( error ) {
        cerr << error << endl;
        exit( -1 );
      }
    } else {
      cerr << error << endl;
      exit( -1 );
    }
  }

  error = you.retrievePatchInfo( server, reload, checkSig );
  if ( error ) {
    cerr << "Error retrieving patches: " << error << endl;
    exit( -1 );
  }

  you.selectPatches( kinds );

  if ( debug || showPatches ) {
    if ( verbose ) cout << "Patches:" << endl;
    you.showPatches( verbose );
  }

  if ( checkUpdates ) {
    if ( Y2PM::youPatchManager().securityUpdatesAvailable() ) {
      cout << "Security updates available." << endl;
      return 2;
    } else if ( Y2PM::youPatchManager().updatesAvailable() ) {
      cout << "Updates available." << endl;
      return 1;
    } else {
      cout << "No updates available." << endl;
    }
    return 0;
  }

  error = you.retrievePatches( reload, checkSig, autoInstall );
  if ( error ) {
    cerr << "Error retrieving packages: " << error << endl;
    exit( -1 );
  }

  if ( verbose && autoGet && !autoInstall ) {
    cout << "Got patches." << endl;
    return 0;
  }

  error = you.installPatches( dryrun );
  if ( error ) {
    cerr << "Error installing packages: " << error << endl;
    exit( -1 );
  }

  MIL << "END" << endl;
  return 0;
}
