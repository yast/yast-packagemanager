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
    string t ("TEST_DIST" );
    InstSrc::Type type( InstSrc::fromString(t) );
    Url url( "http://www.suse.de/" );
    Pathname dir( "/aa/bb" );
    
    descr->set_base_arch( arch );
    descr->set_type( type );
    descr->set_media_vendor( string( "SuSE" ) );
    descr->set_media_id( string( "12432" ) );
    descr->set_media_count( string( "7" ) );
    descr->set_url( url ); 
    MIL << "URL: " << url << endl;
    MIL << "TYPE: " << descr->type() << endl;
    descr->set_product_dir ( dir );
    
    // write
    PMError err = descr->writeCache( "/tmp/mediafile" );

    if ( err == PMError::E_ok )
    {
	// read
	descr->readCache( ndescr_r, "/tmp/mediafile" );
	ndescr_r->dumpOn ( cerr );
	MIL << ndescr_r << endl;
    }

    return 0;
}
