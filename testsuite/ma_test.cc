#include <iomanip>
#include <list>
#include <string>

#undef  Y2LOG
#define Y2LOG "PM_ma_test"
#include <y2util/Y2SLog.h>

#include <y2pm/InstSrcManager.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcSuSE.h>
#include <y2pm/FAKEMediaInfo.h>

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
  MIL << "START" << endl;
  InstSrcManager::ISM();
  PMPackageManager::PM();

#if 1
  FAKEMediaInfoPtr media( new FAKEMediaInfo );
  InstSrcPtr  instSrc( new InstSrcSuSE( media ) );
  /*
  PMError err = InstSrcManager::ISM().scanMedia( instSrc, media, InstSrcManager::T_UNKNOWN );
  DBG << "scanMedia: " << instSrc << endl;
  if ( err ) {
    ERR << err << endl;
  }
  return 0;
  */
#endif

  InstSrcManager::ISM().enableSource(instSrc);

  list<PMPackagePtr> plist;

#if 0
  string n( "name_" );
  string v( "version_" );
  string r( "release_" );
  string a( "i386" );

  for ( unsigned i = 0; i < 10; ++i ) {
    string si( ::dec(i) );
    plist.push_back( new PMPackage( n+si, PkgEdition( (v+si).c_str(), (r+si).c_str() ), "i386") );
    SEC << *plist.rbegin() << endl;
  }
#endif

//  PMPackageManager::PM().addPackages( plist );



  MIL << "END" << endl;
  return 0;
}
