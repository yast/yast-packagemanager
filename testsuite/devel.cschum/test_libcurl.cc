#include <getopt.h>

#include <iostream>
#include <iomanip>
#include <list>
#include <string>

#include <curl/curl.h>

using namespace std;

int main( int argc, char **argv )
{
  const char *url1 = "ftp://dist/full/full-i386/media.1/media";
  const char *url2 = "ftp://dist/full/full-i386/content";

  bool error = false;

  CURLcode ret = curl_global_init( CURL_GLOBAL_ALL );
  if ( ret != 0 ) {
    cerr << "global init failed" << endl;
    exit( 1 );
  }  
  
  CURL *curl = curl_easy_init();
  if ( !curl ) {
    cerr << "easy init failed" << endl;
    error = true;
  } else {
    char curlError[ CURL_ERROR_SIZE ];
    ret = curl_easy_setopt( curl, CURLOPT_ERRORBUFFER, curlError );
    if ( ret != 0 ) {
      cerr << "Error setting error buffer" << endl;
      error = true;
    } else {
      cout << "---- URL1: " << url1 << endl;
      ret = curl_easy_setopt( curl, CURLOPT_URL, url1 );
      if ( ret != 0 ) {
        cerr << "easy setopt failed." << endl;
        error = true;
      } else {
        ret = curl_easy_perform( curl );
        if ( ret != 0 ) {
          cerr << "easy perform failed: " << curlError << endl;
          error = true;
        }
      }

      cout << "---- URL2: " << url2 << endl;
      ret = curl_easy_setopt( curl, CURLOPT_URL, url2 );
      if ( ret != 0 ) {
        cerr << "easy setopt failed." << endl;
        error = true;
      } else {
        ret = curl_easy_perform( curl );
        if ( ret != 0 ) {
          cerr << "easy perform failed: " << curlError << endl;
          error = true;
        }
      }
    }
    curl_easy_cleanup( curl );
  }
  
  curl_global_cleanup();

  if ( error ) exit( 1 );
  
  return 0;
}
