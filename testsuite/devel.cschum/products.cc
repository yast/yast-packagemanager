#include <getopt.h>

#include <iomanip>
#include <list>
#include <string>

#undef  Y2LOG
#define Y2LOG "PM_cschum_test"
#include <y2util/Y2SLog.h>

#include <y2util/Url.h>

#include <y2pm/PMError.h>
#include <y2pm/PMYouPatch.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMYouPatchInfo.h>
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/InstYou.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/Wget.h>
#include <y2pm/InstTarget.h>

#include <Y2PM.h>

using namespace std;

int main( int argc, char **argv )
{
//  Y2Logging::setLogfileName( "-" );

  if ( argc < 1 || argc > 2 || argc == 2 && strcmp( argv[ 1 ], "-i" ) != 0 ) {
    cerr << "Usage: " << argv[ 0 ] << " [-i]" << endl;
    exit( 1 );
  }

  bool install = ( argc == 2 );

  InstTarget &TMGR( Y2PM::instTarget( true ) );

  if ( install ) {
    InstSrcManager &MGR( Y2PM::instSrcManager() );

    InstSrcManager::ISrcIdList nids;
    MGR.getSources( nids, true ); // nids = InstSrcIds aller enableten Quellen

    if ( nids.begin() == nids.end() ) {
      cerr << "No sources." << endl;
      exit( 1 );
    }

    // such die passende Quelle raus und hol ihre InstSrcDescr
    constInstSrcDescrPtr p = (*nids.begin())->descr();

    D__ << "DESCR: " << p->content_product().name << endl;

    PMError err = TMGR.installProduct(p); // Installiert das Produkt auf dem InstTarget

    if ( err ) {
      E__ << err << endl;
      cerr << err << endl;
    }
  }

  const std::list<constInstSrcDescrPtr> &products = TMGR.getProducts();
  
  std::list<constInstSrcDescrPtr>::const_iterator it = products.begin();

  if ( it == products.end() ) {
    cerr << "No products installed." << endl;
    exit ( 1 );
  }

  constInstSrcDescrPtr product = *it;

  PkgNameEd prodEd = product->content_product();

  cout << "PRODUCT NAME: " << prodEd.name << endl;
  cout << "PRODUCT VERSION: " << prodEd.edition.version() << endl;
  cout << "BASEARCH: " << TMGR.baseArch() << endl;

  cout << "DISTPRODUCT: " << product->content_distproduct().asString() << endl;

  cout << "YOUURL: " << product->content_youurl() << endl;
  cout << "YOUTYPE: " << product->content_youtype() << endl;
  cout << "YOUPATH: " << product->content_youpath() << endl;

  InstSrcDescr::ArchMap archMap = product->content_archmap();
  string basearch = TMGR.baseArch();
  
  list<PkgArch> archs = archMap[ basearch ];
  
  cout << "ARCHS: ";
  list<PkgArch>::const_iterator it2;
  for( it2 = archs.begin(); it2 != archs.end(); ++it2 ) {
    cout << " " << *it2;
  }
  cout << endl;

  return 0;
}
