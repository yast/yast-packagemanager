#include <iomanip>
#include <list>
#include <string>

#include <y2util/Y2SLog.h>
#include <y2pm/InstSrcDescr.h>

#include <Y2PM.h>
#include <y2pm/RpmDb.h>

using namespace std;
#undef  Y2LOG
#define Y2LOG "gs_test"


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
    InstSrcDescrPtr ndescr_r;

    // InstSrcDescr to write
    InstSrcDescr * descr = new InstSrcDescr();

    PkgArch arch( "i586" );
    string t ("T_TEST_DIST" );
    InstSrc::Type type( InstSrc::fromString(t) );
    
    descr->set_base_arch( arch );
    descr->set_type( type );

    // write
    PMError err = descr->writeCache( "/tmp/mediafile" );

    MIL << "before READ" << endl; 

    if ( err == PMError::E_ok )
    {
	// read
	descr->readCache( ndescr_r, "/tmp/mediafile" );
	ndescr_r->dumpOn ( cerr );
	MIL << ndescr_r << endl;
    }

    return 0;
}
