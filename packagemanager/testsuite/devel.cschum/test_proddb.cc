#include <getopt.h>

#include <iomanip>
#include <list>
#include <string>

#undef  Y2LOG
#define Y2LOG "PM_cschum_test"
#include <y2util/Y2SLog.h>

#include <y2util/Url.h>

#include <y2pm/PMError.h>
#include <y2pm/PMYouPatch.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMYouPatchInfo.h>
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/InstYou.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/InstTarget.h>
#include <y2pm/InstTargetProdDB.h>

#include <Y2PM.h>

using namespace std;

int main( int argc, char **argv )
{
  set_log_filename( "-" );

  MIL << "START" << endl;

  InstTargetProdDB prodDb;
  prodDb.open();

  std::list<constInstSrcDescrPtr> products = prodDb.getProducts();

  std::list<constInstSrcDescrPtr>::const_iterator it;
  for( it = products.begin(); it != products.end(); ++it ) {
    INT << "Content Product: " << (*it)->content_product() << endl;
  }

  InstSrcDescrPtr newProduct = new InstSrcDescr;
  newProduct->set_content_product( PkgNameEd( PkgName( "SUSE CORE" ),
                                              PkgEdition( "9" ) ) );
  
  prodDb.install( newProduct );

  InstSrcDescrPtr newProduct2 = new InstSrcDescr;
  newProduct2->set_content_product( PkgNameEd( PkgName( "SUSE SLES" ),
                                              PkgEdition( "9" ) ) );
  
  prodDb.install( newProduct2 );

  products = prodDb.getProducts();

  for( it = products.begin(); it != products.end(); ++it ) {
    INT << "Content Product: " << (*it)->content_product() << endl;
  }

  MIL << "END" << endl;
  return 0;
}
