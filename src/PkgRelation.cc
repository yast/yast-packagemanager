#include <cstdio>
#include <cstring>
#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>
#include <y2util/TagParser.h>

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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgRelation::fromString
//	METHOD TYPE : PkgRelation
//
//	DESCRIPTION : name[ op edition]
//
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgRelation::string2DepCompare
//	METHOD TYPE : PkgRelation
//
//	DESCRIPTION : name[ op edition]
//
rel_op PkgRelation::string2DepCompare( const string & str_tr )
{
  enum DepCompare { // ('!=' is not supported by rpm)
    DNONE = 0x00,
    DEQ   = 0x01,
    DLT   = 0x10,
    DGT   = 0x20,
    DLTE  = DLT|DEQ,
    DGTE  = DGT|DEQ,
  };

  unsigned ret_ei = NONE;
  switch( str_tr.size() ) {
  case 2:
    switch ( str_tr[1] ) {
    case '=': ret_ei |= DEQ; break;
    case '<': ret_ei |= DLT; break;
    case '>': ret_ei |= DGT; break;
    default:  return NONE;
    }
    // fall through
  case 1:
    switch ( str_tr[0] ) {
    case '=': ret_ei |= DEQ; break;
    case '<': ret_ei |= DLT; break;
    case '>': ret_ei |= DGT; break;
    default:  return NONE;
    }
    break;
  default:
    return NONE;
    break;
  }
  if(ret_ei == (DLT|DGT))
    ret_ei = NONE;

  switch(ret_ei)
  {
    case DEQ:
      return EQ;
    case DLT:
      return LT;
    case DGT:
      return GT;
    case DLTE:
      return LE;
    case DGTE:
      return GE;
    default:
      return NONE;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PkgRelation::parseRelations
//	METHOD TYPE : PkgRelation
//
//	DESCRIPTION : Parse dep string as found in common.pkd,
//                    e.g. groff >= 1.17 less /bin/sh
//
PMSolvable::PkgRelList_type PkgRelation::parseRelations( const string & data_tr )
{
  PMSolvable::PkgRelList_type ret_VCi;
  vector<string> data_Vti( TagParser::split2words( data_tr ) );

  struct 
  {
    string name;
    rel_op compare;
    string version;
    void clear()
    {
      name = version.erase();
      compare=NONE;
    }
  } cdep_Ci;

  cdep_Ci.compare=NONE;
  rel_op depOp_ei = NONE;

  for ( unsigned i = 0; i < data_Vti.size(); ++i ) {
    depOp_ei = string2DepCompare( data_Vti[i] );

    if ( depOp_ei == NONE ) {
       // string value
      if ( cdep_Ci.name.empty() ) {           // no previous. remember new name
	cdep_Ci.name = data_Vti[i];
      } else if ( cdep_Ci.compare != NONE ) { // remember version after op and store
	cdep_Ci.version = data_Vti[i];
	PkgRelation dep( PkgName( cdep_Ci.name ), cdep_Ci.compare,
                         PkgEdition( cdep_Ci.version ) );
	ret_VCi.push_back( dep );
	cdep_Ci.clear();
      } else {                                // store previous and remember new name
	PkgRelation dep( PkgName( cdep_Ci.name ), cdep_Ci.compare,
                         PkgEdition( cdep_Ci.version ) );
	ret_VCi.push_back( dep );
	cdep_Ci.clear();
	cdep_Ci.name = data_Vti[i];
      }
    } else {
      // operator value
      if ( cdep_Ci.name.empty() || cdep_Ci.compare != NONE ) {
	ERR << "Missplaced operator " << op_str[depOp_ei] << " in dependency of "
	  << cdep_Ci.name << " (" << data_tr << ")" << endl;
	cdep_Ci.clear();
	break;
      } else {
	cdep_Ci.compare = depOp_ei;
      }
    }
  }

  if ( cdep_Ci.name.size() ) {
    if ( cdep_Ci.compare == NONE || cdep_Ci.version.size() ) {
      PkgRelation dep( PkgName( cdep_Ci.name ), cdep_Ci.compare,
                       PkgEdition( PkgEdition::fromString(cdep_Ci.version) ) );
      ret_VCi.push_back( dep );
    } else {
	ERR << "Missplaced operator " << op_str[depOp_ei] << " in dependency of "
	  << cdep_Ci.name << " (" << data_tr << ")" << endl;
    }
  }

  return ret_VCi;
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
