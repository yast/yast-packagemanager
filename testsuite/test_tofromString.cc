#include <iomanip>
#include <list>
#include <string>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>
#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgRelation.h>

using namespace std;
#undef  Y2LOG
#define Y2LOG "PM_ma_test"

bool chk( const PkgEdition & ed ) {
  string s = PkgEdition::toString( ed );
  PkgEdition e( PkgEdition::fromString( s ) );

  MIL << "in :" << ed << endl;
  MIL << "out:" << e << endl;
  if ( ed==e )
    INT << " == " << endl;
  else
    ERR << " != " << endl;

  return (ed==e);
}

bool chk( const PkgNameEd & ed ) {
  string s = PkgNameEd::toString( ed );
  PkgNameEd e( PkgNameEd::fromString( s ) );

  MIL << "in :" << ed << endl;
  MIL << "out:" << e << endl;
  if ( ed==e )
    INT << " == " << endl;
  else
    ERR << " != " << endl;

  return (ed==e);
}

bool chk( const PkgRelation & ed ) {
  string s = PkgRelation::toString( ed );
  PkgRelation e( PkgRelation::fromString( s ) );

  MIL << "in :" << ed << endl;
  MIL << "out:" << e << endl;
  if ( ed==e )
    INT << " == " << endl;
  else
    ERR << " != " << endl;

  return (ed==e);
}

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
  Y2Logging::setLogfileName("-");
  MIL << "START" << endl;

  chk( PkgEdition(PkgEdition::UNSPEC) );
  chk( PkgEdition(PkgEdition::MAXIMUM) );
  chk( PkgEdition() );
  chk( PkgEdition("","") );
  chk( PkgEdition("v","") );
  chk( PkgEdition("","r") );
  chk( PkgEdition("v","r") );
  chk( PkgEdition(1, 0) );
  chk( PkgEdition(1, "") );
  chk( PkgEdition(1, "v") );
  chk( PkgEdition(1, "","") );
  chk( PkgEdition(1, "v","") );
  chk( PkgEdition(1, "","r") );
  chk( PkgEdition(1, "v","r") );

  chk( PkgNameEd( PkgName(), PkgEdition()) );
  chk( PkgNameEd( PkgName("n"), PkgEdition()) );
  chk( PkgNameEd( PkgName("n-n"), PkgEdition()) );
  chk( PkgNameEd( PkgName("n-n-n"), PkgEdition()) );
  chk( PkgNameEd( PkgName(), PkgEdition("v","")) );
  chk( PkgNameEd( PkgName("n"), PkgEdition("v","")) );
  chk( PkgNameEd( PkgName("n-n"), PkgEdition("v","")) );
  chk( PkgNameEd( PkgName("n-n-n"), PkgEdition("v","")) );
  chk( PkgNameEd( PkgName(), PkgEdition("","r")) );
  chk( PkgNameEd( PkgName("n"), PkgEdition("","r")) );
  chk( PkgNameEd( PkgName("n-n"), PkgEdition("","r")) );
  chk( PkgNameEd( PkgName("n-n-n"), PkgEdition("","r")) );
  chk( PkgNameEd( PkgName(), PkgEdition("v","r")) );
  chk( PkgNameEd( PkgName("n"), PkgEdition("v","r")) );
  chk( PkgNameEd( PkgName("n-n"), PkgEdition("v","r")) );
  chk( PkgNameEd( PkgName("n-n-n"), PkgEdition("v","r")) );

  chk( PkgRelation( PkgName(), NONE, PkgEdition()) );
  chk( PkgRelation( PkgName("a"), NONE, PkgEdition()) );
  chk( PkgRelation( PkgName("a a"), NONE, PkgEdition()) );
  chk( PkgRelation( PkgName(), GT, PkgEdition("v")) );
  chk( PkgRelation( PkgName("a"), EQ, PkgEdition("v")) );
  chk( PkgRelation( PkgName("a a"), NE, PkgEdition("v")) );
  MIL << "end" << endl;
  return 0;
}
