#include <iomanip>
#include <list>
#include <string>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>

using namespace std;
#undef  Y2LOG
#define Y2LOG "PM_ma_test"

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

  list<PMPackagePtr> plist;

  string n( "name_" );
  string v( "version_" );
  string r( "release_" );
  string a( "i386" );

  for ( unsigned i = 0; i < 10; ++i ) {
    string si( ::dec(i) );
    plist.push_back( new PMPackage( PkgName(n+si), PkgEdition( (v+si).c_str(), (r+si).c_str() ), PkgArch("i386")) );
    SEC << *plist.rbegin() << endl;
  }

  plist.push_back( 0 );
  plist.push_back( *plist.begin() );
  string si( ::dec(5) );
  plist.push_back( new PMPackage( PkgName(n+si), PkgEdition( (v+si).c_str(), (r+si).c_str() ), PkgArch("i686")) );
  plist.push_back( new PMPackage( PkgName(n+si), PkgEdition( (v+si+".1").c_str(), (r+si).c_str() ), PkgArch("i686")) );


  Y2PM::packageManager().poolAddCandidates( plist );
  SEC << "=================================" << endl;

  INT << Y2PM::packageManager().size() << endl;
  PMSelectablePtr p = Y2PM::packageManager()["name_"];
  INT << p << endl;
  p = Y2PM::packageManager()["name_0"];
  INT << p << endl;

  SEC << "=================================" << endl;
  Y2PM::packageManager().poolRemoveCandidates( plist );

  MIL << "END" << endl;
  Y2PM::packageManager().REINIT();
  return 0;
}
