#include <cstdio>
#include <cstring>
#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>

#include <y2pm/PkgRelation.h>
#include <y2pm/PMSolvable.h>

using namespace std;

bool PkgRelation::matches( const PMSolvablePtr pkg ) const
{
	if (pkg->name() != _name)
		return false;
	if (_op == NONE)
		return true;
	return pkg->edition().compare( _op, _edition );
}


//
// This function checks if two version ranges overlap. It is needed to
// check if a relation holds. Easiest case: The whole range is
// accepted/provided if no relation+edition is present. The most
// general case is: p1 needs p2 rel1 ed1 and p2 is provided by p3 rel2
// ed2. So p1 requires some subset of the possible ranges defined by
// rel1 ed1, and some other subset defined by rel2 ed2 is available.
// PkgRelation::matches compares the two ranges if there is any
// intersection and returns true if there is.
//
bool PkgRelation::matches( const PkgRelation& rel2 ) const
{
	if (_name != rel2._name)
		return false;

	rel_op o1 = _op, o2 = rel2._op;

	// there is always an intersection if one of the version ranges is
	// the full range
	if (o1 == NONE || o2 == NONE)
		return true;
	// after that, there must be a relation on both packages!

	// there is always at least one point of intersection if both
	// relations point into the same "direction"
	if (((o1 == LT || o1 == LE) && (o2 == LT || o2 == LE)) ||
		((o1 == GT || o1 == GE) && (o2 == GT || o2 == GE)))
		return true;

	const PkgEdition& e1 = _edition;
	const PkgEdition& e2 = rel2._edition;
	// if one of the relations is =, we can use the other one directly
	if (o1 == EQ)
		return e1.compare( o2, e2 );
	if (o2 == EQ)
		return e2.compare( o1, e1 );

	// Otherwise, the relations point into different directions, and the
	// editions must be in the reverse one. Equality is allowed
	// only if both relations allow it, too.
	bool both_allow_eq = (o1 == LE || o1 == GE) && (o2 == LE || o2 == GE);
	rel_op op =
		 (o1 == LT || o1 == LE)
			   ? (both_allow_eq ? LE : LT)
			   : (both_allow_eq ? GE : GT);
	return e2.compare( op, e1 );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgRelation::asString
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string PkgRelation::asString() const
{
  // if you don't like implement your own format here,
  // but don't change toString().
  return toString( *this );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgRelation::toString
//	METHOD TYPE : string
//
//	DESCRIPTION : name[ op edition]
//
string PkgRelation::toString( const PkgRelation & t )
{
  const char * op = "";
  switch( t._op ) {
  case LT: op = "<";  break;
  case GT: op = ">";  break;
  case EQ: op = "=";  break;
  case LE: op = "<="; break;
  case GE: op = ">="; break;
  case NE: op = "!="; break;
  case NONE: return t._name; break;
  }

  return stringutil::form( "%s %s %s",
			   t._name->c_str(),
			   op,
			   PkgEdition::toString( t._edition ).c_str() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgRelation::fromString
//	METHOD TYPE : PkgRelation
//
//	DESCRIPTION : name[ op edition]
//
inline rel_op str2op( const string & t ) {
  switch ( t.size() ) {
  case 1:
    switch ( t[0] ) {
    case '<': return LT;
    case '>': return GT;
    case '=': return EQ;
    }
    break;
  case 2:
    if ( t[1] == '=' ) {
      switch ( t[0] ) {
      case '<': return LE;
      case '>': return GE;
      case '=': return EQ;
      case '!': return NE;
      }
    }
    break;
  }
  return NONE;
}

PkgRelation PkgRelation::fromString( string s )
{
  vector<string> words;
  unsigned num = stringutil::split( s, words );

  string n;
  string o;
  string e;

  switch ( num ) {

  case 1:
    return PkgRelation( PkgName(words[0]), NONE, PkgEdition() );
    break;

  case 3:
    {
      rel_op op = str2op( words[1] );
      if ( op != NONE )
	return PkgRelation( PkgName(words[0]), op, PkgEdition::fromString(words[2]) );
    }
    break;
  }

  ERR << "Error parsing PkgRelation from '" << s << "'" << endl;
  return PkgRelation( PkgName(), NONE, PkgEdition() );
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream&
**
**	DESCRIPTION :
*/
ostream& operator<<( ostream& os, const PkgRelation& rel )
{
  return os << rel.asString();
}

// Local Variables:
// tab-width: 4
// End:
