#include <iomanip>
#include <list>
#include <string>

#undef  Y2LOG
#define Y2LOG "PM_patchinfo_test"
#include <y2util/Y2SLog.h>

#include <y2pm/PMYouPatch.h>
#include <y2pm/PMYouPatchInfo.h>

using namespace std;

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
  Y2SLog::setLogfileName( "test_patchinfo.log" );
  MIL << "START" << endl;

  PMYouPatchInfo patchInfo( "german" );
  list<PMYouPatchPtr> patches;
  PMError error = patchInfo.readFile( "", "mypatch-123", patches );

  if ( error != PMError::E_ok ) {
    cerr << error << endl;
  } else {

    list<PMYouPatchPtr>::const_iterator it;
    for( it = patches.begin(); it != patches.end(); ++it ) {
      (*it)->dumpOn( cout );
    }
  }

  MIL << "END" << endl;
  return 0;
}
