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

void usage()
{
  cout << "Usage: checkpatch [-s] [-d] [-v] <patchfile>" << endl
       << endl
       << "-s  Show patch information" << endl
       << "-d  Show detailed patch information" << endl
       << "-v  Verbose mode" << endl;
       
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
  bool verbose = false;
  bool show = false;
  bool detailed = false;

  int c;
  while( 1 ) {
    c = getopt( argc, argv, "hvsd" );
    if ( c < 0 ) break;

    switch ( c ) {
      case 'v':
        verbose = true;
        break;
      case 's':
        show = true;
        break;
      case 'd':
        detailed = true;
        break;
      default:
        cerr << "Error parsing command line." << endl;
      case '?':
      case 'h':
        usage();
    }
  }

  if ( optind == argc ) usage();

  string patchFile = argv[ optind ];

  if ( verbose ) Y2Logging::setLogfileName( "-" );

  PMYouPatchInfoPtr patchInfo( new PMYouPatchInfo );
  std::list<PMYouPatchPtr> patches;

  PMError error = patchInfo->readFile( "", patchFile, patches );

  if ( error ) {
    cerr << "Check failed: " << error << endl;
    exit( 1 );
  } else {
    cout << "Check succeeded." << endl;
  }

  if ( show || detailed ) {
    list<PMYouPatchPtr>::const_iterator it;
    for( it = patches.begin(); it != patches.end(); ++it ) {
      cout << "PATCH: " << (*it)->name() << " (" << (*it)->shortDescription()
           << ")" << endl;
//      (*it)->dumpOn( cout );
      cout << "Summary: " << (*it)->summary() << endl;
      cout << "Size: " << (*it)->size().asString() << endl;
      cout << "Kind: " << (*it)->kindLabel( (*it)->kind() ) << endl;
      cout << "<description>" << endl << (*it)->longDescription() << endl
           << "</description>" << endl;
      list<PMPackagePtr> packages = (*it)->packages();
      list<PMPackagePtr>::const_iterator it2;
      for( it2 = packages.begin(); it2 != packages.end(); ++it2 ) {
        cout << "  Package:" << endl;

        cout << "    Name: " << (*it2)->name() << endl;
        cout << "    Version: " << (*it2)->version() << endl;
        cout << "    Release: " << (*it2)->release() << endl;

        cout << "    Size: " << (*it2)->size() << endl;
        cout << "    ArchiveSize: " << (*it2)->archivesize() << endl;
        cout << "    PatchRpmSize: " << (*it2)->patchRpmSize() << endl;

        if ( detailed ) {
          list<PkgEdition> editions = (*it2)->patchRpmBaseVersions();
          list<PkgEdition>::const_iterator it3;
          for( it3 = editions.begin(); it3 != editions.end(); ++it3 ) {
            cout << "    PATCHRPM BASEVERSION: '" << it3->asString() << "'" << endl;
          }
          cout << "    REQUIRES:" << endl;
          list<PkgRelation> relations = (*it2)->requires();
          list<PkgRelation>::const_iterator it4;
          for( it4 = relations.begin(); it4 != relations.end(); ++it4 ) {
            cout << "      " << (*it4).asString() << endl;          
          }
        }
      }
    }
  }

  return 0;
}
