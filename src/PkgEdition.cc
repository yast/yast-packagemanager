#include <cstdio>
#include <cstring>
#include <alloca.h>
#include <assert.h>
#include <cctype>

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>

#include <y2pm/PkgEdition.h>

using namespace std;

const char* op_str[GE+1] = { "none","==","!=","<","<=",">",">=" };

// Use a 'version-release' form for these strings.
// (i.e. exactly one '-'). Needed for toString/fromString
const std::string PkgEdition::_str_UNSPEC(  "EDITION-UNSPEC" );
const std::string PkgEdition::_str_MAXIMUM( "EDITION-MAXIMUM");

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgEdition::xconstruct
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PkgEdition::xconstruct( type_enum xtype,
			     unsigned epoch, const std::string & v, const std::string & r,
			     time_t buildtime )
{
  _type = xtype;

  if ( _type == MAXIMUM || _type == UNSPEC ) {
    _epoch     = 0;
    _version   = "";
    _release   = "";
    _buildtime = 0;
    return;
  }

  _epoch     = epoch;
  _version   = v;
  _release   = r;
  _buildtime = buildtime;

  // check whether to strip release from version

  string::size_type sep = _version.rfind( '-' );

  if ( sep != string::npos ) {
    if ( _release.empty() ) {
      _release = _version.substr( sep+1 );
    } else {
      INT << "Explicit release overrides release coded in version: v '" << v << "' r '" << r << "'" << endl;
    }
    _version.erase( sep );
  }

  // check whether epoch coded in version

  sep = _version.find( ':' );

  if ( sep != string::npos ) {
    if ( _epoch ) {
      INT << "Explicit epoch overrides epoch coded in version: v '" << v << "' e '" << epoch << "'" << endl;
    } else {
      int e = atoi( _version.substr( 0, sep ).c_str() );
      if ( e ) {
	_epoch = e;
      }
    }
    _version.erase( 0, sep+1 );
  }
}

//
// compare two editions
//
// The main function handles the special cases of unspecified and maximum
// versions; then it looks at the relation operator and passes the real work
// on to edition_eq and edition_lt.
//
bool PkgEdition::compare( rel_op op, const PkgEdition& e2 ) const
{
  // unspecified versions are uncomparable and always return FALSE, except
  // both editions are UNSPEC
  if (_type == UNSPEC || e2._type == UNSPEC)
    return (_type == UNSPEC && e2._type == UNSPEC && op == EQ);

  // if both editions are MAXIMUM, they're equal.
  // if only one is MAXIMUM, that one is greater
  if (_type == MAXIMUM && e2._type == MAXIMUM)
    return op == EQ || op == LE || op == GE;
  else if (_type == MAXIMUM)
    return op == GT || op == GE;
  else if (e2._type == MAXIMUM)
    return op == LT || op == LE;

  assert( op != NONE );
  assert( _type == NORMAL );
  switch( op ) {
  case EQ:
    return edition_eq( e2 );
  case NE:
    return !edition_eq( e2 );
  case LT:
    return edition_lt( e2 );
  case LE:
    return edition_eq( e2 ) || edition_lt( e2 );
  case GE:
    return !edition_lt( e2 );
  case GT:
    return !edition_eq( e2 ) && !edition_lt( e2 );
  case NONE: // make gcc happy
    return false;
  }
  return false;
}

// compare two editions for equality
bool PkgEdition::edition_eq( const PkgEdition& e2 ) const
{
  // compare epochs
  if (_epoch != e2._epoch)
    return false;

  // empty version means any version matches
  if(_version.empty() || e2._version.empty())
    return true;
  if (rpmvercmp( _version, e2._version ) != 0)
    return false;

  // empty release means any release matches
  if(_release.empty() || e2._release.empty())
    return true;
  if (rpmvercmp( _release, e2._release ) != 0)
    return false;

  // compare buildtimes iff provided by both
  if ( _buildtime && e2._buildtime )
    return( _buildtime == e2._buildtime );

  return true;
}

// true if this edition is less than (and not equal to) e2
bool PkgEdition::edition_lt( const PkgEdition& e2 ) const
{
  // compare epochs
  if ( _epoch != e2._epoch )
    return( _epoch < e2._epoch );

  // next compare versions
  int d = rpmvercmp( _version, e2._version );
  if ( d != 0 )
    return( d < 0 );

  // next compare releases
  d = rpmvercmp( _release, e2._release );
  if ( d != 0 )
    return( d < 0 );

  // compare buildtimes iff provided by both
  if ( _buildtime && e2._buildtime )
    return ( _buildtime < e2._buildtime );

  return false; // everything's equal
}

/**
 * Return -1,0,1 if versions are <,==,>
 **/
int PkgEdition::rpmvercmp( const std::string & lhs, const std::string & rhs ) const
{
  int  num1, num2;
  char oldch1, oldch2;
  char * str1, * str2;
  char * one, * two;
  int  rc;
  int  isnum;

  // equal?
  if ( lhs == rhs )  return 0;

  // empty is less than anything else:
  if ( lhs.empty() ) return -1;
  if ( rhs.empty() ) return  1;

  str1 = (char*)alloca( lhs.size() + 1 );
  str2 = (char*)alloca( rhs.size() + 1 );

  strcpy( str1, lhs.c_str() );
  strcpy( str2, rhs.c_str() );

  one = str1;
  two = str2;

  // split strings into segments of alpha or digit
  // sequences and compare them accordingly.
  while ( *one && *two ) {

    // skip non alphanumerical chars
    while ( *one && ! isalnum( *one ) ) ++one;
    while ( *two && ! isalnum( *two ) ) ++two;
    if ( ! ( *one && *two ) )
      break; // reached end of string

    // remember segment start
    str1 = one;
    str2 = two;

    // jump over segment, type determined by str1
    if ( isdigit( *str1 ) ) {
      while ( isdigit( *str1 ) ) ++str1;
      while ( isdigit( *str2 ) ) ++str2;
      isnum = 1;
    } else {
      while ( isalpha( *str1 ) ) ++str1;
      while ( isalpha( *str2 ) ) ++str2;
      isnum = 0;
    }

    // one == str1 -> can't be as strings are not empty
    // two == str2 -> mixed segment types
    if ( two == str2 ) return( isnum ? 1 : -1 );

    // compare according to segment type
    if ( isnum ) {
      // atoi() may overflow on long segments
      // skip leading zeros
      while ( *one == '0' ) ++one;
      while ( *two == '0' ) ++two;
      // compare number of digits
      num1 = str1 - one;
      num2 = str2 - two;
      if ( num1 != num2 ) return( num1 < num2 ? -1 : 1 );
    }

    // strcmp() compares alpha AND equal sized number segments
    // temp. \0-terminate segment
    oldch1 = *str1;
    *str1 = '\0';
    oldch2 = *str2;
    *str2 = '\0';

    rc = strcmp( one, two );
    if ( rc ) return rc;

    // restore original strings
    *str1 = oldch1;
    *str2 = oldch2;

    // prepare for next cycle
    one = str1;
    two = str2;
  }

  // check which strings are now empty
  if ( !*one ) {
    return( !*two ? 0 : -1 );
  }
  return 1;
}


string PkgEdition::asString() const
{
  // if you don't like implement your own format here,
  // but don't change toString().
  return toString( *this );
}

ostream& operator<<( ostream& os, const PkgEdition& e )
{
  os << e.asString();
  return os;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgEdition::toString
//	METHOD TYPE : string
//
//	DESCRIPTION : [epoch:]version-release | EDITION-UNSPEC | EDITION-MAXIMUM
//                    (exactly one '-' in string)
//
string PkgEdition::toString( const PkgEdition & t )
{
  if ( t._type == UNSPEC )
    return _str_UNSPEC;
  if ( t._type == MAXIMUM )
    return _str_MAXIMUM;

  string ret;

  if ( t._epoch ) {
    ret += stringutil::form( "%d:", t._epoch );
  }

  ret += stringutil::form( "%s-%s",
			   t._version.c_str(),
			   t._release.c_str() );
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgEdition::fromString
//	METHOD TYPE : PkgEdition
//
//	DESCRIPTION : [epoch:]version-release | EDITION-UNSPEC | EDITION-MAXIMUM
//                    (exactly one '-' in string)
//
PkgEdition PkgEdition::fromString( string s )
{
  if ( s == _str_UNSPEC ) {
    return PkgEdition( UNSPEC );
  }
  if ( s == _str_MAXIMUM ) {
    return PkgEdition( MAXIMUM );
  }
  return PkgEdition( s );
}

