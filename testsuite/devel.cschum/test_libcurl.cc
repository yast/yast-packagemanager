#include <getopt.h>

#include <iomanip>
#include <list>
#include <string>

#include <y2util/Y2SLog.h>

#include <y2util/Url.h>

#include <curl/curl.h>

using namespace std;

int main( int argc, char **argv )
{
  MIL << "START" << endl;

  if ( argc != 2 ) {
    cerr << "Usage: test_libcurl <url>" << endl;
  }

  cout << "URL: '" << argv[1] << "'" << endl;

  bool error = false;

  CURLcode ret = curl_global_init( CURL_GLOBAL_ALL );
  if ( ret != 0 ) {
    ERR << "global init failed" << endl;
    exit( 1 );
  }  
  
  CURL *curl = curl_easy_init();
  if ( !curl ) {
    ERR << "easy init failed" << endl;
    error = true;
  } else {
    ret = curl_easy_setopt( curl, CURLOPT_URL, argv[ 1 ] );
    if ( ret != 0 ) {
      ERR << "easy setopt failed." << endl;
      error = true;
    }

    ret = curl_easy_perform( curl );
    if ( ret != 0 ) {
      ERR << "easy perform failed." << endl;
      error = true;
    }

    curl_easy_cleanup( curl );
  }
  
  curl_global_cleanup();

  MIL << "END" << endl;

  if ( error ) exit( 1 );
  
  return 0;
}
