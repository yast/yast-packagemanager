/*
  test_content.cc

  test program for InstSrcData->tryGetDescr()

*/

#include <iomanip>
#include <list>
#include <string>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>

#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/MediaAccess.h>

using namespace std;
#undef  Y2LOG
#define Y2LOG "PM_test_content"

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
int main(int argc, char **argv)
{
  Y2SLog::setLogfileName("-");
  MIL << "START" << endl;
  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0] << " <media_url> <product_dir>" << endl;
    exit (1);
  }
  InstSrcPtr nsrc;

  Url      url     ( argv[1] );  // media
  Pathname proddir ( argv[2] );	      // product dir
  Pathname cache   ( "/tmp/tcache" ); // cachedir (must not exist)

  PMError err = InstSrc::vconstruct( nsrc, cache, url, proddir, InstSrc::T_UnitedLinux );

  SEC << err << endl;
  SEC << nsrc << endl;

  MIL << "END" << endl;
  return 0;
}
