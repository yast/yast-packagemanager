/*
  YaST Online Update (YOU) command line tool

  Textdomain "packagemanager"

*/

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
#include <y2pm/PMLocale.h>

using namespace std;

void usage()
{
  cout << _("Usage: online-update [options] [types]")
       << endl << endl
       << _("-u, --url URL            Base URL of directory tree from which to get patches.\n"
            "                         Supported protocols: http, ftp, smb, nfs, cd, dvd, file.\n"
            "                         If no protocol is given, a local file is assumed.\n"
            "                         Examples: 'ftp://ftp.suse.com/pub/suse', 'cd:///',\n"
            "                                   'file:/var/lib/YaST2/you/mnt'\n"
            "                                   '/var/lib/YaST2/you/mnt'") << endl
       << endl
       << _("-g, --download-only      Only download patches. Do not install.") << endl
       << _("-G, --get-all            Retrieve all available patches and packages for the\n"
            "                         given product. Do not install.") << endl
       << _("-i, --install-only       Install downloaded patches. Do not download.") << endl
       << endl
       << _("-q, --quick-check        Quick check for new updates. Does not check for types\n"
            "                         of updates.") << endl
       << _("-k, --check              Check for new updates.") << endl
       << endl
       << _("-c, --show-config        Show configuration. Do not do anything.") << endl
       << endl
       << _("-p, --product PRODUCT    Name of product for which to get patches.") << endl
       << _("-v, --version VERSION    Version of product for which to get patches.") << endl
       << _("-a, --architecture ARCH  Base architecture of product for which to get patches.") << endl
       << _("-l, --language LANGCODE  Language used to show patch descriptions.") << endl
       << endl
       << _("-r, --reload             Reload patches from server.") << endl
       << _("-d, --dry-run            Dry run. Only get patches, but do not install them.") << endl
       << _("-n, --no-sig-check       No signature check of downloaded files.") << endl
       << endl
       << _("-s, --show-patches       Show list of patches (Additionaly use -d only to show list\n"
            "                         of patches without installing them).") << endl
       << _("-V, --verbose            Be verbose.") << endl
       << _("-D, --debug              Debug output.") << endl
       << endl
       << "security | recommended | patchlevel | document | optional   "
       << _("Types of patches to install.") << endl;
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
  setlocale(LC_ALL, "" );
  PMLocale::setTextdomain();

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
        cerr << _("Error parsing command line.") << endl;
      case '?':
      case 'h':
        usage();
    }
  }

  if ( debug ) verbose = true;

  if ( getuid() != 0 && !checkUpdates && !quickCheckUpdates && !showConfig ) {
    cerr << _("You need root permissions to run this command. Use the -q or -k\n"
              "options to check for the availabilty of updates without needing\n"
              "root permissions.") << endl;
    exit( 1 );
  }

  int kinds = PMYouPatch::kind_invalid;

  if (optind < argc) {
    while (optind < argc) {
      string arg = argv[optind++];
      if ( arg == "security" ) kinds |= PMYouPatch::kind_security;
      else if ( arg == "recommended" ) kinds |= PMYouPatch::kind_recommended;
      else if ( arg == "patchlevel" ) kinds |= PMYouPatch::kind_patchlevel;
      else if ( arg == "document" ) kinds |= PMYouPatch::kind_document;
      else if ( arg == "optional" ) kinds |= PMYouPatch::kind_optional;
      else if ( arg == "all" ) {
        kinds = PMYouPatch::kind_all;
        break;
      } else usage();
    }
  }

  if ( kinds == PMYouPatch::kind_invalid ) {
    kinds = PMYouPatch::kind_security | PMYouPatch::kind_recommended |
            PMYouPatch::kind_patchlevel;
  }

  if ( verbose ) {
    cout << "Types of patches to be installed:";
    if ( kinds & PMYouPatch::kind_security ) cout << " security";
    if ( kinds & PMYouPatch::kind_recommended ) cout << " recommended";
    if ( kinds & PMYouPatch::kind_patchlevel ) cout << " patchlevel";
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
  settings->setGetOnly( autoGet );

  list<PMYouProductPtr> products = you.settings()->products();

  if ( showConfig ) {
    int i = 0;
    list<PMYouProductPtr>::const_iterator itProd;
    for( itProd = products.begin(); itProd != products.end(); ++itProd ) {
      PMYouProductPtr prod = *itProd;
      cout << _("Product ") << i;
      if ( ( products.size() > 1 ) &&  ( i == 0 ) ) {
        cout << _(" (primary product)");
      }
      cout << endl;
      cout << _("  Name:      ") << prod->product() << endl;
      cout << _("  Version:      ") << prod->version() << endl;
      cout << _("  Architecture: ") << prod->baseArch() << endl;
      if ( verbose ) {
        cout << _("  Arch List: ");
        std::list<PkgArch> archs = prod->archs();
        std::list<PkgArch>::const_iterator it;
        for( it = archs.begin(); it != archs.end(); ++it ) {
          cout << (*it).asString() << " ";
        }
        cout << endl;
        cout << _("  Business Product: ")
             << ( prod->businessProduct() ? _("Yes") : _("No") ) << endl;
        cout << _("  Distribution: ") << prod->distProduct() << endl;
        cout << _("  YOU Path: ") << prod->patchPath() << endl;
      }
      ++i;
    }
    cout << _("Language:     ") << you.settings()->langCode() << endl;
    cout << _("Directory:    ") << you.settings()->directoryFileName() << endl;
  
    exit( 0 );
  }

  // Get URL of you source.

  PMYouServer server;

  if ( autoInstall ) {
    server.setUrl( "dir://" + you.settings()->attachPoint().asString() );
  } else {
    if ( urlStr ) {
      if ( !Url( urlStr ).isValid() ) {
        cerr << _("Error: URL '") << urlStr << _("' is not valid.") << endl;
        exit( -1 );
      }
      server.setUrl( urlStr );
      server.setType( PMYouServer::Custom );
    } else {
      PMYouServers youServers( you.settings() );
      error = youServers.requestServers( checkUpdates || quickCheckUpdates );
      if ( error ) {
        cerr << _("Error while requesting servers: ") << error << endl;
        exit( -1 );
      }
      server = youServers.currentServer();
    }
  }

  settings->setPatchServer( server );

  if ( verbose ) {
    cout << _("Server URL: ") << server.url() << endl;
    cout << _("Server Name: ") << server.name() << endl;
    cout << _("Directory File: ") << server.directory() << endl;
    int i = 0;
    list<PMYouProductPtr>::const_iterator itProd;
    for( itProd = products.begin(); itProd != products.end(); ++itProd ) {
      cout << _("Path ") << i++ << ": " << (*itProd)->patchPath() << endl;
    }
  }

  if ( quickCheckUpdates ) {
    int updates = you.quickCheckUpdates();
    if ( updates < 0 ) {
      cerr << _("Unable to check for updates.") << endl;
      return -1;
    } else if ( updates == 0 ) {
      cout << _("No new updates available.") << endl;
      return 0;
    } else if ( updates == 1 ) {
      cout << _("1 new update available.") << endl;
      return 1;
    } else {
      cout << updates << _(" new updates available.") << endl;
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
    cerr << _("Error retrieving patches: ") << error << endl;
    exit( -1 );
  }

  you.selectPatches( kinds );

  if ( debug || showPatches ) {
    if ( verbose ) cout << _("Patches:") << endl;
    you.showPatches( verbose );
  }

  if ( checkUpdates ) {
    if ( Y2PM::youPatchManager().securityUpdatesAvailable() ) {
      cout << _("Security updates available.") << endl;
      return 2;
    } else if ( Y2PM::youPatchManager().updatesAvailable() ) {
      cout << _("Updates available.") << endl;
      return 1;
    } else {
      cout << _("No updates available.") << endl;
    }
    return 0;
  }

  error = you.processPatches();

  bool installedPatches = you.installedPatches();
  if ( verbose ) {
    if ( !installedPatches ) {
      cout << _("No patches have been installed.") << endl;
    } else {
      cout << _("Patches have been installed.") << endl;
    }
  }

  if ( error ) {
    cerr << _("Error processing patches: ") << error << endl;
    exit( -1 );
  }

  if ( getAll || ( autoGet && !autoInstall ) ) {
    if ( verbose ) cout << _("Got patches.") << endl;
  }

  MIL << "online_update done" << endl;
  return 0;
}
