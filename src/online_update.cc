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
#include <y2pm/PMYouProduct.h>

using namespace std;

void usage()
{
  cout << "Usage: online-update [options] [types]"
       << endl << endl
       << "-u, --url URL            Base URL of directory tree used to get patches from." << endl
       << "                         Supported protocols: http, ftp, smb, nfs, cd, dvd, file." << endl
       << "                         If no protocol is given a local file is assumed." << endl
       << "                         Examples: 'ftp://ftp.suse.com/pub/suse', 'cd:///'," << endl
       << "                                   'file:/var/lib/YaST2/you/mnt'" << endl
       << "                                   '/var/lib/YaST2/you/mnt'" << endl
       << endl
       << "-g, --download-only      Only download patches, don't install." << endl
       << "-G, --get-all            Retrieve all available patches and packages for the" << endl
       << "                         given product, don't install" << endl
       << "-i, --install-only       Install downloaded patches, don't download." << endl
       << endl
       << "-q, --quick-check        Quick check for new updates. Doesn't check for types" << endl
       << "                         of updates." << endl
       << "-k, --check              Check for new updates." << endl
       << endl
       << "-c, --show-config        Show configuration. Don't do anything." << endl
       << endl
       << "-p, --product PRODUCT    Name of product to get patches for." << endl
       << "-v, --version VERSION    Version of product to get patches for." << endl
       << "-a, --architecture ARCH  Base architecture of product to get patches for." << endl
       << "-l, --language LANGCODE  Language used to show patch descriptions." << endl
       << endl
       << "-r, --reload             Reload patches from server." << endl
       << "-d, --dry-run            Dry run. Only get patches, don't install them." << endl
       << "-n, --no-sig-check       No signature check of downloaded files." << endl
       << endl
       << "-s, --show-patches       Show list of patches (Additionaly use -d to only show list " << endl
       << "                         of patches without installing them)." << endl
       << "-V, --verbose            Be verbose." << endl
       << "-D, --debug              Debug output." << endl
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
  MIL << "CMD: online_update";
  for ( int i = 1; i < argc; ++i ) MIL << " " << argv[ i ];
  MIL << endl;

  const char *urlStr = 0;
  const char *productStr = 0;
  const char *versionStr = 0;
  const char *archStr = 0;
  const char *langStr = 0;

  bool dryrun = false;
  bool checkSig = true;
  bool showPatches = false;
  bool verbose = false;
  bool debug = false;
  bool autoGet = false;
  bool autoInstall = false;
  bool reload = false;
  bool showConfig = false;
  bool checkUpdates = false;
  bool quickCheckUpdates = false;
  bool getAll = false;
  
  int c;
  while( 1 ) {
    static struct option long_options[] = {
      { "quick-check", no_argument, 0, 'q' },
      { "check", no_argument, 0, 'k' },
      { "reload", no_argument, 0, 'r' },
      { "show-config", no_argument, 0, 'c' },
      { "download-only", no_argument, 0, 'g' },
      { "install-only", no_argument, 0, 'i' },
      { "help", no_argument, 0, 'h' },
      { "dry-run", no_argument, 0, 'd' },
      { "no-sig-check", no_argument, 0, 'n' },
      { "show-patches", no_argument, 0, 's' },
      { "verbose", no_argument, 0, 'V' },
      { "debug", no_argument, 0, 'D' },
      { "url", required_argument, 0, 'u' },
      { "product", required_argument, 0, 'p' },
      { "version", required_argument, 0, 'v' },
      { "architecture", required_argument, 0, 'a' },
      { "language", required_argument, 0, 'l' },
      { "get-all", no_argument, 0, 'G' },
      { 0, 0, 0, 0 }
    };

    int option_index = 0;

    c = getopt_long( argc, argv, "qkrcgihdnsVDu:p:v:a:l:G", long_options,
                     &option_index );
    if ( c < 0 ) break;

    switch ( c ) {
      case 0:
      printf ("option %s", long_options[option_index].name);
      if (optarg)
          printf (" with arg %s", optarg);
      printf ("\n");
      break;
        
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
      case 'G':
        getAll = true;
        break;
      default:
        cerr << "Error parsing command line." << endl;
      case '?':
      case 'h':
        usage();
    }
  }

  if ( getuid() != 0 && !checkUpdates && !quickCheckUpdates && !showConfig ) {
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
  else version = "9.1";
  
  string arch;
  if ( archStr ) arch = archStr;
  else arch = "i386";

  PMYouSettingsPtr settings = new PMYouSettings( product, version, arch );
  PMYouPatchInfoPtr patchInfo = new PMYouPatchInfo( settings );

  InstYou you( patchInfo, settings );

  if ( !productStr && !versionStr && !archStr ) {
    you.initProduct();
  }

  settings->setLangCode( LangCode( lang ) );
  settings->setReloadPatches( reload );
  settings->setCheckSignatures( checkSig );
  settings->setDryRun( dryrun );
  settings->setNoExternalPackages( autoInstall );
  settings->setGetAll( getAll );

  list<PMYouProductPtr> products = you.settings()->products();

  if ( showConfig ) {
    int i = 0;
    list<PMYouProductPtr>::const_iterator itProd;
    for( itProd = products.begin(); itProd != products.end(); ++itProd ) {
      PMYouProductPtr prod = *itProd;
      cout << "Product " << i;
      if ( ( products.size() > 1 ) &&  ( i == 0 ) ) {
        cout << " (primary product)";
      }
      cout << endl;
      cout << "  Name:      " << prod->product() << endl;
      cout << "  Version:      " << prod->version() << endl;
      cout << "  Architecture: " << prod->baseArch() << endl;
      if ( verbose || debug ) {
        cout << "  Arch list: ";
        std::list<PkgArch> archs = prod->archs();
        std::list<PkgArch>::const_iterator it;
        for( it = archs.begin(); it != archs.end(); ++it ) {
          cout << (*it).asString() << " ";
        }
        cout << endl;
        cout << "  Business Product: "
             << ( prod->businessProduct() ? "Yes" : "No" ) << endl;
        cout << "  Distribution: " << prod->distProduct() << endl;
        cout << "  YOU Path: " << prod->patchPath() << endl;
      }
      ++i;
    }
    cout << "Language:     " << you.settings()->langCode() << endl;
    cout << "Directory:    " << you.settings()->directoryFileName() << endl;
  
    exit( 0 );
  }

  // Get URL of you source.

  PMYouServer server;

  if ( autoInstall ) {
    server.setUrl( "dir://" + you.settings()->attachPoint().asString() );
  } else {
    if ( urlStr ) {
      if ( !Url( urlStr ).isValid() ) {
        cerr << "Error: URL '" << urlStr << "' is not valid." << endl;
        exit( -1 );
      }
      server.setUrl( urlStr );
    } else {
      PMYouServers youServers( you.settings() );
      error = youServers.requestServers( checkUpdates || quickCheckUpdates );
      if ( error ) {
        cerr << "Error while requesting servers: " << error << endl;
        exit( -1 );
      }
      server = youServers.currentServer();
    }
  }

  settings->setPatchServer( server );

  if ( verbose ) {
    cout << "Server URL: " << server.url() << endl;
    cout << "Server Name: " << server.name() << endl;
    cout << "Directory File: " << server.directory() << endl;
    int i = 0;
    list<PMYouProductPtr>::const_iterator itProd;
    for( itProd = products.begin(); itProd != products.end(); ++itProd ) {
      cout << "Path " << i++ << ": " << (*itProd)->patchPath() << endl;
    }
  }

  if ( quickCheckUpdates ) {
    int updates = you.quickCheckUpdates();
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

  error = you.retrievePatchDirectory();
  if ( error ) {
    if ( error == MediaError::E_login_failed ) {
      you.readUserPassword();
      error = you.retrievePatchDirectory();
      if ( error ) {
        cerr << error << endl;
        exit( -1 );
      }
    } else {
      cerr << error << endl;
      exit( -1 );
    }
  }

  error = you.retrievePatchInfo();
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

  error = you.retrievePatches();
  if ( error ) {
    cerr << "Error retrieving patches: " << error << endl;
    exit( -1 );
  }

  if ( getAll || ( autoGet && !autoInstall ) ) {
    if ( verbose ) cout << "Got patches." << endl;
    return 0;
  }

  error = you.installPatches();
  if ( error ) {
    cerr << "Error installing packages: " << error << endl;
    exit( -1 );
  }

  MIL << "online_update done" << endl;
  return 0;
}
