#include <getopt.h>

#include <iomanip>
#include <list>
#include <string>

#undef  Y2LOG
#define Y2LOG "callback_test"
#include <y2util/Y2SLog.h>

#include <y2util/Url.h>

#include <Y2PM.h>

using namespace std;

class MyCallbacks : public InstYou::Callbacks
{
  bool progress( int )
  {
    cout << "myCallback" << endl;
    return true;
  }

  bool patchProgress( int, const string &str )
  {
    cout << "myOtherCallback: " << str << endl;
    return true;
  }
};


bool myCallback( int, void * )
{
  cout << "myCallback" << endl;
  return true;
}

bool myOtherCallback( int, const string &str, void * )
{
  cout << "myOtherCallback: " << str << endl;
  return true;
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

  cout << "Hallo" << endl;

  Y2PM::youPatchManager().instYou().setCallbacks( new MyCallbacks );

  Y2PM::youPatchManager().instYou().progress( 1 );
  Y2PM::youPatchManager().instYou().progress( 2 );
  
  Y2PM::youPatchManager().instYou().patchProgress( 1, "hallo" );
  Y2PM::youPatchManager().instYou().patchProgress( 1, "huhu" );

  return 0;
}
