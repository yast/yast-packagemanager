#define _GLIBCXX_CONCEPT_CHECKS
#include <iomanip>
#include <fstream>
#include <string>

#include <y2util/Y2SLog.h>
#include <y2util/TmpPath.h>

using namespace std;

/******************************************************************
**
**	FUNCTION NAME : main
**	FUNCTION TYPE : int
*/
int main( int argc, char * argv[] )
{
  set_log_filename( "-" );
  MIL << "START" << endl;

  TmpPath foo( "xfoobaa" );
  const TmpPath baa;
  MIL << foo << endl;
  MIL << baa << endl;
  foo = baa;

  TmpFile f(".", "file");
  TmpDir d(".","dir");
  MIL << f << endl;
  MIL << d << endl;

  ofstream s( f.path().asString().c_str() );
  s << "lkgjsj" << endl;
  s.close();
  int i;
  cin >> i;
  SEC << "STOP" << endl;
  return 0;
}

