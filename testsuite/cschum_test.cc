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

#include <Y2PM.h>

using namespace std;

void printRel( PkgEdition left, PkgEdition right )
{
  D__ << left << " <-> " << right << endl;

  cout << left << " ";
  if ( left < right ) { D__ << "is <" << endl; cout << "<"; }
  else if ( left > right ) { D__ << "is >" << endl; cout << ">"; }
  else if ( left == right ) { D__ << "is ==" << endl; cout << "=="; }
  else cout << "[undefined]";
  cout << " " << right << endl;

  D__ << "-----------------------------------------------" << endl;
}

void printEd( PkgEdition ed )
{
  cout << ed << ": version: " << ed.version() << " release: " << ed.release()
       << " buildtime: " << ed.buildtime() << endl;
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
  Y2Logging::setLogfileName( "cschum_test.log" );
  MIL << "START" << endl;

  if ( argc != 4 ) {
    cerr << "Usage: " << argv[0] << " <url> <user> <password>" << endl;
    exit( 1 );
  }

  InstYou you;
  PMError error = you.checkAuthorization( Url( argv[1] ), argv[2], argv[3] );
  cerr << error << endl;

#if 0
  if ( argc != 2 ) {
    cerr << "Usage: " << argv[0] << " <url>" << endl;
    exit( 1 );
  }

  string url( argv[1] );
  string dest( "wgot" );

  cout << "Getting '" << url << "' to '" << dest << "'" << endl;

  Wget wget;
  
  WgetStatus status = wget.getFile( Url( url ), dest );
  
  cout << "STATUS: " << wget.error_string( status ) << endl;
#endif

#if 0
  InstTarget &TMGR( Y2PM::instTarget( true ) );

#if 1
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
#endif

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

  cout << "YOUTYPE: " << product->content_youtype() << endl;
  cout << "YOUPATH: " << product->content_youpath() << endl;
#endif

#if 0
  PMPackageManager &mgr = Y2PM::packageManager();
  
  PMSelectablePtr sel = mgr.getItem( "mydummy" );
  
  if ( !sel ) {
    cerr << "no selectable" << endl;
  } else {
    PMPackagePtr pkg = sel->installedObj();

    if ( !pkg ) {
      cerr << "no installed obj" << endl;
    } else {
      cout << pkg->name() << ": " << pkg->version() << " (" << pkg->arch() <<")" << endl;
    }
  }
#endif

#if 0
  PkgEdition five( "0.9-0" );
  printEd( five );
  PkgEdition one( "1.0-0" );
  printEd( one );
  PkgEdition two( "1.0-1" );
  printEd( two );
  PkgEdition three( "1.0-2" );
  printEd( three );
  PkgEdition four( "1.1-0" );
  printEd( four );
  
  printRel( five, two );
  printRel( one, two );
  printRel( two, two );
  printRel( three, two );
  printRel( four, two );
#endif

#if 0
  cout << "num: " << Y2PM::youPatchManager().size() << endl;

  const PMYouPatchManager &mgr = Y2PM::youPatchManager();

  PMManager::PMSelectableVec::const_iterator it;
  for ( it = mgr.begin(); it != mgr.end(); ++it ) {
    PMSelectablePtr selectable = *it;
    PMYouPatchPtr installed = selectable->installedObj();
    if ( installed ) {
      cout << installed->name() << endl;
    }
  }
#endif

#if 0
  list<PMYouPatchPtr> patches = Y2PM::instTarget().getPatches();

  list<PMYouPatchPtr>::const_iterator it;
  for( it = patches.begin(); it != patches.end(); ++it ) {
    cout << "PATCH: " << (*it)->name() << " (" << (*it)->shortDescription()
         << ")" << endl;
  }
#endif

  MIL << "END" << endl;
  return 0;
}
