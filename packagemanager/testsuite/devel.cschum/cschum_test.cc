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
#include <y2pm/InstTarget.h>

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
  set_log_filename( "-" );

  MediaAccess media;
  
  PMError error = media.open( Url( "nfs://machcd3/machcd3" ) );
  if ( error ) {
    ERR << error << endl;
    return 1;
  }
  error = media.attach();
  if ( error ) {
    ERR << error << endl;
    return 1;
  }
  
  string file = "CDs/logs/suse81-patchcd-i386";
  
  error = media.provideFile( file );
  if ( error ) {
    ERR << error << endl;
    return 1;
  }
  
  system( ( "ls -l " + media.localPath( file ).asString() ).c_str() );
  
#if 0
  ExternalProgram prg( "bash myscript" );
  prg.close();
#endif

#if 0
  class MyCallbacks : public InstTarget::Callbacks
  {
    public:
      bool scriptProgress( int p )
      {
        static int i = 0;
        INT << "tick " << p << endl;
//        return i++ < 5;
        return true;
      }
  };

  MyCallbacks c;

  InstTarget::setCallbacks( &c );

  PMError error = Y2PM::instTarget().executeScript( "myscript" );

  DBG << error << endl;
#endif
  
#if 0
  if ( argc != 2 ) {
    cerr << "Usage: " << argv[ 0 ] << " <url>" << endl;
    exit( 1 );
  }

  Url u( argv[ 1 ] );
  
  Pathname path = u.getPath();
  
  DBG << path << endl;
  
  DBG << path.dirname() << endl;
  DBG << path.basename() << endl;
#endif

#if 0  
  Url u(
"http://weber/cgi-bin/suseservers.cgi?product=SuSE%20Linux&version=8.1&basearch=i386&business=0&distproduct=--&yast2-online-update=2.6.12-10&yast2-packagemanager=2.6.26-0&liby2util=2.6.18-4"
);
  PMError error = MediaAccess::getFile( u, "/var/lib/YaST2/you/youservers" );

  DBG << error << endl;
      
  return 0;
#endif 
  
#if 0
  if ( argc != 4 ) {
    cerr << "Usage: " << argv[0] << " <url> <user> <password>" << endl;
    exit( 1 );
  }

  InstYou you;
  PMError error = you.checkAuthorization( Url( argv[1] ), argv[2], argv[3] );
  cerr << error << endl;
#endif

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
