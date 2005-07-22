#include <y2util/stringutil.h>

#include <y2pm/PkgName.h>

using namespace std;

///////////////////////////////////////////////////////////////////

UstringHash PkgName::_nameHash;

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
// struct PkgNameEd
//
///////////////////////////////////////////////////////////////////

ostream& operator<<( ostream& os, const PkgNameEd& ne )
{
  return os << ne.asString();
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgNameEd::asString
//	METHOD TYPE : string
//
//	DESCRIPTION : name-edition (edition contains one '-')
//
string PkgNameEd::asString() const
{
  // if you don't like implement your own format here,
  // but don't change toString().
  return toString( *this );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgNameEd::toString
//	METHOD TYPE : string
//
//	DESCRIPTION : name-edition (edition contains one '-')
//
string PkgNameEd::toString( const PkgNameEd & t )
{
  return stringutil::form( "%s-%s",
			   t.name->c_str(),
			   PkgEdition::toString( t.edition ).c_str() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgNameEd::fromString
//	METHOD TYPE : PkgNameEd
//
//	DESCRIPTION : name-edition (edition contains one '-')
//
PkgNameEd PkgNameEd::fromString( string s )
{
  string::size_type n_sep = s.rfind( '-' );

  if ( n_sep && n_sep != string::npos ) {
    n_sep = s.rfind( '-', n_sep-1 );
    if ( n_sep != string::npos ) {
      string n = s.substr( 0, n_sep );
      string e = s.substr( n_sep+1 );

      return PkgNameEd( PkgName( n ), PkgEdition::fromString( e ) );
    }
  }

  // oops: asume name
  return PkgNameEd( PkgName( s ), PkgEdition() );
}


// Local Variables:
// tab-width: 4
// End:
