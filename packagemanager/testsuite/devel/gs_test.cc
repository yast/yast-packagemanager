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
    
    descr->set_type( type );
    descr->set_media_vendor( Vendor( "SuSE" ) );
    descr->set_media_id( string( "12432" ) );
    descr->set_content_vendor( Vendor("Caldera" ) );
    descr->set_content_product( PkgNameEd::fromString("firewall") );
    descr->set_default_activate( false );
    descr->set_url( url ); 
    MIL << "URL: " << url << endl;
    MIL << "TYPE: " << descr->type() << endl;
    descr->set_product_dir ( dir );

    InstSrcDescr::ArchMap archmap;
    list<PkgArch> archs;
    archs.push_back( PkgArch ("i586") );
    archs.push_back( PkgArch ("i486") );
    archs.push_back( PkgArch ("i386") );
    archs.push_back( PkgArch ("noarch") );
    archmap[PkgArch("i586")] = archs;
    descr->set_content_archmap( archmap );

    InstSrcDescr::LabelMap labelmap;
    labelmap[LangCode("en")] = "english";
    labelmap[LangCode("de")] = "german";
    descr->set_content_labelmap( labelmap );

    InstSrcDescr::LinguasList linguas;
    linguas.push_back( LangCode("en") );
    linguas.push_back( LangCode("de") );
    descr->set_content_linguas( linguas );
     
    // write
    PMError err = descr->writeCache( "/tmp/" );

    if ( err == PMError::E_ok )
    {
	// read
	descr->readCache( ndescr_r, "/tmp/" );
	ndescr_r->dumpOn ( cerr );
	MIL << ndescr_r << endl;
    }

    return 0;
}
