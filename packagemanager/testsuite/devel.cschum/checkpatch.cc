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
  unsigned verbose = 0;
  bool show = false;
  bool detailed = false;

  int c;
  while( 1 ) {
    c = getopt( argc, argv, "hvsd" );
    if ( c < 0 ) break;

    switch ( c ) {
      case 'v':
        ++verbose;
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

  Pathname patchFile = argv[ optind ];
  Pathname dir = patchFile.dirname();
  patchFile = patchFile.basename();

  if ( verbose ) set_log_filename( "-" );
  if ( verbose > 1 )
  {
    Y2SLog::dbg_enabled_bm = true;
    set_log_debug(true);
  }

  PMYouSettingsPtr settings = new PMYouSettings;
  PMYouPatchInfoPtr patchInfo( new PMYouPatchInfo(settings) );
  PMYouPatchPtr patch;

  PMError error = patchInfo->readFile( dir, patchFile.asString(), patch );

  if ( error ) {
    cerr << "Check failed: " << error << endl;
    exit( 1 );
  } else {
    cout << "Check succeeded." << endl;
  }

  if ( show) {
    patch->dumpOn(cout);

    if(detailed)
    {
      list<PMPackagePtr> packages = patch->packages();
      list<PMPackagePtr>::const_iterator itPkg;
      cout << "Packages: " << endl;
      for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
        cout << "     RPM: " << (*itPkg)->nameEdArch() << " - " << (*itPkg)->summary() << endl;

	std::list<PMPackageDelta> d = (*itPkg)->deltas();
	if(!d.empty())
	{
	  cout << "Deltas:" << endl;
	  std::list<PMPackageDelta>::iterator it = d.begin();
	  std::list<PMPackageDelta>::iterator end = d.end();
	  for(; it != end; ++it)
	  {
	    cout << "  " << *it;
	  }
	}
      }
    }
  }

  return 0;
}
