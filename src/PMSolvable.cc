/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:       PMSolvable.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>
#include <sstream>

#include <y2util/Y2SLog.h>
#include <y2pm/PMSolvable.h>

using namespace std;

/******************************************************************
**
**
**	FUNCTION NAME : PkgRelList2AttributeValue
**	FUNCTION TYPE : PkgAttributeValue
**
**	DESCRIPTION :
*/
inline PkgAttributeValue PkgRelList2AttributeValue( const PMSolvable::PkgRelList_type & rellist_r )
{
  PkgAttributeValue ret;
  for( PMSolvable::PkgRelList_const_iterator it = rellist_r.begin(); it != rellist_r.end(); ++it ) {
    ret.push_back( it->asString() );
  }
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSolvable
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMSolvable);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::PMSolvable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSolvable::PMSolvable()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::PMSolvable
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSolvable::PMSolvable( const PkgName& name,
			const PkgEdition& edition,
			const PkgArch& arch )
      : _name(name), _edition(edition), _arch(arch)
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::~PMSolvable
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMSolvable::~PMSolvable()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::getAttributeName
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string PMSolvable::getAttributeName( PMSolvableAttribute attr ) const
{
  switch ( attr ) {

#define ENUM_OUT(V) case ATTR_##V: return #V; break
    ENUM_OUT( NAME );
    ENUM_OUT( VERSION );
    ENUM_OUT( RELEASE );
    ENUM_OUT( ARCH );
    ENUM_OUT( REQUIRES );
    ENUM_OUT( PREREQUIRES );
    ENUM_OUT( PROVIDES );
    ENUM_OUT( OBSOLETES );
    ENUM_OUT( CONFLICTS );
#undef ENUM_OUT

  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  case PMSLV_NUM_ATTRIBUTES:
    // illegal attr value
    break;
  }
  // HERE: illegal attr value or forgott do adjust switch.
  ERR << "Illegal PMSolvableAttribute(" << attr << ')' << endl;
  return "";
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::getAttributeValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION :
//
PkgAttributeValue PMSolvable::getAttributeValue( PMSolvableAttribute attr ) const
{
  switch ( attr ) {

  case ATTR_NAME:
    return PkgAttributeValue( name() );
    break;

  case ATTR_VERSION:
    return PkgAttributeValue( edition().version() );
    break;

  case ATTR_RELEASE:
    return PkgAttributeValue( edition().release() );
    break;

  case ATTR_ARCH:
    return PkgAttributeValue( arch() );
    break;

  case ATTR_REQUIRES:
    return PkgRelList2AttributeValue( requires() );
    break;

  case ATTR_PREREQUIRES:
    return PkgRelList2AttributeValue( prerequires() );
    break;

  case ATTR_PROVIDES:
    return PkgRelList2AttributeValue( provides() );
    break;

  case ATTR_CONFLICTS:
    return PkgRelList2AttributeValue( conflicts() );
    break;

  case ATTR_OBSOLETES:
    return PkgRelList2AttributeValue( obsoletes() );
    break;

  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  case PMSLV_NUM_ATTRIBUTES:
    // illegal attr value
    break;
  }
  // HERE: illegal attr value or forgott do adjust switch.
  ERR << "Illegal PMSolvableAttribute " << attr << endl;
  return PkgAttributeValue();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSolvable::dumpOn
//	METHOD TYPE : std::ostream &
//
//	DESCRIPTION :
//
ostream & PMSolvable::dumpOn( ostream & os ) const
{
  Rep::dumpOn( os );
  os << endl;
  os << "Name: " << _name << endl;
  os << "Edition: " << _edition << endl;
  if (_requires.size())
    os << "Requires: " << _requires << endl;
  if (_prerequires.size())
    os << "PreRequires: " << _prerequires << endl;
  if (_conflicts.size())
    os << "Conflicts: " << _conflicts << endl;
  if (_provides.size())
    os << "Provides: " << _provides << endl;
  if (_obsoletes.size())
    os << "Obsoletes: " << _obsoletes << endl;

  //	os << "Size: " << _size << endl;

#if 0
  for( unsigned i = 0; i < Package::N_STRS; ++i ) {
    if (!pkg.strings[i] || pkg.strings[i]->empty())
      continue;
    // we need a named object for the tag name here, because egcs-2.91.66
    // otherwise gets confused with the temporary string and a SIGSEGV is
    // the result.
    string tagn = Tag::tagname(Package::str_tags[i]);
    os << tagn << ": " << modify_newlines(*pkg.strings[i]) << endl;
  }
  for( unsigned i = 0; i < Package::N_BINS; ++i ) {
    if (!pkg.binfields[i] || !pkg.binfields[i]->contents)
      continue;
    // we need a named object for the tag name here, because egcs-2.91.66
    // otherwise gets confused with the temporary string and a SIGSEGV is
    // the result.
    string tagn = Tag::tagname(Package::bin_tags[i]);
    os << tagn << ": "<< hex << setfill('0');
    for( unsigned j = 0; j < pkg.binfields[i]->size; ++j ) {
      os << setw(2) << (unsigned)(pkg.binfields[i]->contents[j]);
    }
    os << dec << setfill(' ') << endl;
  }
#endif
  return os;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream& operator<<( ostream& os, const PMSolvable::PkgRelList_type& rl )
{
  for( PMSolvable::PkgRelList_const_iterator q = rl.begin();
       q != rl.end(); ++q ){
    if (q != rl.begin())
      os << ", ";
    os << *q;
  }
  return os;
}
