#include <getopt.h>

#include <iomanip>
#include <list>
#include <string>

#include <y2util/Y2SLog.h>

#include <y2util/Url.h>

#include <y2pm/MediaAccess.h>

using namespace std;

int main( int argc, char **argv )
{
  MIL << "START" << endl;

  if ( argc != 3 ) {
    cerr << "Usage: test_media <url> <dir>" << endl;
    return 1;
  }

  Url url( argv[ 1 ] );
  Pathname arg( argv[ 2 ] );
  
  cout << "URL: " << url.asString() << endl;
  cout << "Arg: " << arg.asString() << endl;

  MediaAccess media;
  
  PMError err = media.open( url );
  if ( err ) {
    cerr << "Error opening: " << err << endl;
    return 1;
  }
  
  err = media.attach();
  if ( err ) {
    cerr << "Error attaching: " << err << endl;
    return 1;
  }

  err = media.provideFile( "/media.1/media" );
  if ( err ) {
    cerr << "Error providing file: " << err << endl;
    return 1;
  }
  err = media.provideFile( "/content" );
  if ( err ) {
    cerr << "Error providing file: " << err << endl;
    return 1;
  }

#if 0
  list<string> files;
  
  cout << "Without dots:" << endl;
  err = media.dirInfo( files, dir, false );
  if ( err ) {
    cerr << "Error dirInfo: " << err << endl;
    return 1;
  }
  list<string>::const_iterator it;
  for( it = files.begin(); it != files.end(); ++it ) {
    cout << "  " << *it << endl;
  }

  cout << "With dots:" << endl;
  err = media.dirInfo( files, dir, true );
  if ( err ) {
    cerr << "Error dirInfo: " << err << endl;
    return 1;
  }
  for( it = files.begin(); it != files.end(); ++it ) {
    cout << "  " << *it << endl;
  }
#endif

  MIL << "END" << endl;
  
  return 0;
}
