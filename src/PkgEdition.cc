#include <cstdio>
#include <cstring>
#include <alloca.h>
#include <cctype>

#include <y2util/stringutil.h>

#include <y2pm/PkgEdition.h>

using namespace std;

const char* op_str[GE+1] = { "none","==","!=","<","<=",">",">=" };

// Use a 'version-release' form for these strings.
// (i.e. exactly one '-')
const std::string PkgEdition::_str_UNSPEC(  "EDITION-UNSPEC" );
const std::string PkgEdition::_str_MAXIMUM( "EDITION-MAXIMUM");

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
	if (type == UNSPEC && e2.type == UNSPEC && op == EQ)
		return true;
	if (type == UNSPEC || e2.type == UNSPEC)
		return false;

	// if both editions are MAXIMUM, they're equal.
	// if only one is MAXIMUM, that one is greater
	if (type == MAXIMUM && e2.type == MAXIMUM)
		return op == EQ || op == LE || op == GE;
	else if (type == MAXIMUM)
		return op == GT || op == GE;
	else if (e2.type == MAXIMUM)
		return op == LT || op == LE;

	assert( op != NONE );
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
	if (type != e2.type) {
		// Special hack case. if one has an epoch and the other one does
		// not we just compare versions.
		if (!(((type == EPOCH) && (e2.type == NORMAL)) ||
		     ((type == NORMAL) && (e2.type == EPOCH)))
		)
			return false;
	}
	if ((type == EPOCH) && (e2.type == EPOCH) && (_epoch != e2._epoch))
		return false;
	if (rpmvercmp( _version, e2._version ) != 0)
		return false;
	if (rpmvercmp( _release, e2._release ))
		return false;
	// NOTE: we do not compare buildtimes here. If versions/release/libs
	// are the same, we assume the same RPM even with different buildtimes.
	// or we have to upgrade everything after every selfhosting run. -MM
	return (_metahash == e2._metahash);
}

// true if this edition is less than (and not equal to) e2
bool PkgEdition::edition_lt( const PkgEdition& e2 ) const
{
	// if only one edition has an epoch, it is less
	// if both have epochs, we know the result if epochs are different
	if (type == EPOCH) {
		if (e2.type == EPOCH) {
			if (_epoch < e2._epoch)
				return true;
			else if (_epoch > e2._epoch)
				return false;
		}
		else
			return false;
	}
	else if (e2.type == EPOCH)
		return true;

	// next compare versions
	int d = rpmvercmp( _version, e2._version );
	if (d != 0)
		return d < 0;

	// next compare releases
	if (rpmvercmp( _release, e2._release ) >= 0)
		return false;
	return (_buildtime < e2._buildtime); // no metahash needed here
}

int PkgEdition::rpmvercmp( const std::string & lhs, const std::string & rhs ) const
{
    int num1, num2;
    char oldch1, oldch2;
    char * str1, * str2;
    char * one, * two;
    int rc;
    int isnum;

    if ( lhs == rhs )  return 0;
    // empty is less than anything else:
    if ( lhs.empty() ) return -1;
    if ( rhs.empty() ) return  1;

    str1 = (char *)alloca(lhs.size() + 1);
    str2 = (char *)alloca(rhs.size() + 1);

    strcpy(str1, lhs.c_str());
    strcpy(str2, rhs.c_str());

    one = str1;
    two = str2;

    while (*one && *two) {
	while (*one && !isalnum(*one)) one++;
	while (*two && !isalnum(*two)) two++;

	str1 = one;
	str2 = two;

	if (isdigit(*str1)) {
	    while (*str1 && isdigit(*str1)) str1++;
	    while (*str2 && isdigit(*str2)) str2++;
	    isnum = 1;
	} else {
	    while (*str1 && isalpha(*str1)) str1++;
	    while (*str2 && isalpha(*str2)) str2++;
	    isnum = 0;
	}

	oldch1 = *str1;
	*str1 = '\0';
	oldch2 = *str2;
	*str2 = '\0';

	if (one == str1) return -1;	/* arbitrary */
	if (two == str2) return -1;

	if (isnum) {
	    num1 = atoi(one);
	    num2 = atoi(two);

	    if (num1 < num2)
		return -1;
	    else if (num1 > num2)
		return 1;
	} else {
	    rc = strcmp(one, two);
	    if (rc) return rc;
	}

	*str1 = oldch1;
	one = str1;
	*str2 = oldch2;
	two = str2;
    }

    if ((!*one) && (!*two)) return 0;

    if (!*one) return -1; else return 1;
}


string PkgEdition::as_string() const
{
  // if you don't like implement your own format here,
  // but don't change toString().
  return toString( *this );
}

ostream& operator<<( ostream& os, const PkgEdition& e )
{
  os << e.as_string();
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
  if ( t.type == UNSPEC )
    return _str_UNSPEC;
  if ( t.type == MAXIMUM )
    return _str_MAXIMUM;

  string ret;

  if ( t.type == EPOCH ) {
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

  string::size_type e_sep = s.find( ':' );
  string::size_type r_sep = s.rfind( '-' );

  string r;
  if ( r_sep != string::npos ) {
    r = s.substr( r_sep+1 );
    s.erase( r_sep );
  }

  if ( e_sep != string::npos ) {
    string v = s.substr( e_sep+1 );;
    s.erase( e_sep );
    int e = atoi( s.c_str() );
    return PkgEdition( e, v.c_str(), r.c_str() );
  }

  return PkgEdition( s.c_str(), r.c_str() );
}


// Local Variables:
// tab-width: 4
// End:
