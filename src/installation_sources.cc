#include <getopt.h>

#include <iomanip>
#include <list>
#include <string>

#undef  Y2LOG
#define Y2LOG "PM_installation_sources"
#include <y2util/Y2SLog.h>

#include <y2util/Url.h>

#include <Y2PM.h>
#include <y2pm/PMError.h>
#include <y2pm/InstSrcManager.h>
#include <y2pm/InstSrcDescr.h>

using namespace std;

void usage()
{
  cout << "Usage:" << endl
       << "  installation_sources -a url   Add source at given URL." << endl
       << "  installation_sources -s       Show all available sources." << endl;
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

  bool showSources = false;
  bool addSource = false;

  int c;
  while( 1 ) {
    c = getopt( argc, argv, "sa:" );
    if ( c < 0 ) break;

    switch ( c ) {
      case 's':
        showSources = true;
        break;
      case 'a':
        addSource = true;
        urlStr = optarg;
        break;
      default:
        cerr << "Error parsing command line." << endl;
      case '?':
      case 'h':
        usage();
    }
  }

  if ( showSources && addSource ) usage();
  if ( !showSources && !addSource ) usage();

  InstSrcManager &instSrcMgr = Y2PM::instSrcManager();

  InstSrcManager::ISrcIdList sourceIds;

  if ( addSource ) {
    Url url( urlStr );
    if ( !url.isValid() ) {
      cerr << "URL is invalid." << endl;
      exit( 1 );
    }

    PMError error = instSrcMgr.scanMedia( sourceIds, url );
    if ( error ) {
      cerr << error << endl;
    }
  }

  if ( showSources ) {
    instSrcMgr.getSources( sourceIds );
  }

  InstSrcManager::ISrcIdList::const_iterator it;
  for( it = sourceIds.begin(); it != sourceIds.end(); ++it ) {
    cout << "InstSrc:" << endl;
    constInstSrcDescrPtr descr = (*it)->descr();
    cout << "  PRODUCT: " << descr->content_product().name << endl;
  }

  MIL << "END" << endl;
  return 0;
}
