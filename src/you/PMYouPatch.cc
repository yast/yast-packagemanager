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

  File:       PMYouPatch.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Defines the YouPatch object.

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMYouPatch.h>
#include <y2pm/PMYouPatchDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatch
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER( PMYouPatch, PMObject, PMSolvable );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatch::PMYouPatch
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMYouPatch::PMYouPatch( const PkgName &    name_r,
			const PkgEdition & edition_r )
    : PMObject( name_r, edition_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatch::~PMYouPatch
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMYouPatch::~PMYouPatch()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatch::getAttributeName
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string PMYouPatch::getAttributeName( PMYouPatchAttribute attr ) const
{
  switch ( attr ) {

#define ENUM_OUT(V) case ATTR_##V: return #V; break
    //ENUM_OUT( WAHTEVER_IS_FIRST );
#undef ENUM_OUT

  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  case YOU_NUM_ATTRIBUTES:
    // illegal attr value
    break;
  }
  // HERE: illegal attr value or forgott do adjust switch.
  ERR << "Illegal YouPatchAttribute(" << attr << ')' << endl;
  return "";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatch::getAttributeValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION :
//
PkgAttributeValue PMYouPatch::getAttributeValue( PMYouPatchAttribute attr ) const
{
  if ( !_dataProvider ) {
    ERR << "No DataProvider for " << *this << endl;
    return PkgAttributeValue();
  }
  return _dataProvider->getAttributeValue( this, attr );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatch::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMYouPatch::dumpOn( ostream & str ) const
{
  PMObject::dumpOn( str );
  return str;
}

