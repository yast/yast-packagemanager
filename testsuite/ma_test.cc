#include <iomanip>
#include <list>
#include <string>

#undef  Y2LOG
#define Y2LOG "PM_ma_test"
#include <y2util/Y2SLog.h>

#include <y2pm/PMPackageManager.h>
#include <y2pm/PMPackage.h>

using namespace std;

inline string dec( unsigned i ) {
  static char b[5];
  sprintf( b, "%u", i );
  return b;
}

/******************************************************************
**
**
**	FUNCTION NAME : main
**	FUNCTION TYPE : int
**
**	DESCRIPTION :
*/
int main()
{
  Y2SLog::setLogfileName("-");
  MIL << "START" << endl;
  PMPackageManager::PM();

  list<PMPackagePtr> plist;

  string n( "name_" );
  string v( "version_" );
  string r( "release_" );
  string a( "i386" );

  for ( unsigned i = 0; i < 10; ++i ) {
    string si( ::dec(i) );
    plist.push_back( new PMPackage( n+si, PkgEdition( (v+si).c_str(), (r+si).c_str() ), "i386") );
    SEC << *plist.rbegin() << endl;
  }

  plist.push_back( 0 );
  plist.push_back( *plist.begin() );
  string si( ::dec(5) );
  plist.push_back( new PMPackage( n+si, PkgEdition( (v+si).c_str(), (r+si).c_str() ), "i686") );
  plist.push_back( new PMPackage( n+si, PkgEdition( (v+si+".1").c_str(), (r+si).c_str() ), "i686") );


  PMPackageManager::PM().poolAddCandidates( plist );
  SEC << "=================================" << endl;
  PMPackageManager::PM().poolRemoveCandidates( plist );

  MIL << "END" << endl;
  return 0;
}
