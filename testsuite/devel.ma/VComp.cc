extern "C" {
#include <rpm/rpmlib.h>
}

#include <iostream>
#include <iomanip>

#include <y2pm/PkgEdition.h>

using namespace std;

inline string CmpOp( int res ) {
  if ( res )
    return( res < 0 ? "(<)" : "(>)" );
  return "(==)";
}

inline int RpmVerCmp( const string & lhs, const string & rhs ) {
  int ret = rpmvercmp( lhs.c_str(), rhs.c_str() );

  cout << "rpm-" << rpmEVR << "::rpmvercmp( "
    << lhs << ", " << rhs << ") == "
      << setw( 2 ) << ret << " " << CmpOp( ret )
	<< endl;
}


int main( int argc, char * argv[] )
{
  --argc;
  ++argv;

  if ( argc != 2 ) {
    cerr << "VComp: Two arguments expected!" << endl;
    cerr << "Usage: VComp edition1 edition2" << endl;
    return -1;
  }

  string sed1( argv[0] );
  string sed2( argv[1] );
  PkgEdition ed1( sed1 );
  PkgEdition ed2( sed2 );

  int fw = RpmVerCmp( sed1, sed2 );
  int rv = RpmVerCmp( sed2, sed1 );

  if (    ( fw < 0 && ! rv > 0 )
       || ( fw > 0 && ! rv < 0 )
       || ( fw == 0 && rv != 0 ) ) {
    cerr << "***rpm-" << rpmEVR << "::rpmvercmp( "
      << sed1 << ", " << sed2 << ") asymmetic result: "
	<< setw( 2 ) << fw << " " << CmpOp( fw )
	  << " | " << setw( 2 ) << rv << " " << CmpOp( rv )
	    << endl;
  }


#define VCOMP(OP) cout << sed1 << " " << setw( 2 ) << #OP << " " << sed2 << " -> " << (ed1 OP ed2) << endl

  VCOMP( <);
  VCOMP( >);
  VCOMP(==);
  VCOMP(!=);
  VCOMP(<=);
  VCOMP(>=);

  return 0;
}
