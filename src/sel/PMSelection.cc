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

  File:       PMSelection.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Defines the software Selection object.

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMSelection.h>
#include <y2pm/PMSelectionDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelection
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER( PMSelection, PMObject, PMSolvable );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelection::PMSelection
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSelection::PMSelection( const PkgName &    name_r,
			  const PkgEdition & edition_r,
			  const PkgArch &    arch_r,
			  PMSelectionDataProviderPtr dataProvider_r )
    : PMObject( name_r, edition_r, arch_r )
    , _dataProvider( dataProvider_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelection::~PMSelection
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMSelection::~PMSelection()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelection::getAttributeName
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string PMSelection::getAttributeName( PMSelectionAttribute attr ) const
{
  switch ( attr ) {

#define ENUM_OUT(V) case ATTR_##V: return #V; break
    ENUM_OUT( CATEGORY );
    ENUM_OUT( VISIBLE );
    ENUM_OUT( INSTALL );
    ENUM_OUT( DELETE );
    ENUM_OUT( ARCHIVESIZE );
#undef ENUM_OUT

  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  case PMSEL_NUM_ATTRIBUTES:
    // illegal attr value
    break;
  }
  // HERE: illegal attr value or forgott do adjust switch.
  ERR << "Illegal SelectionAttribute(" << attr << ')' << endl;
  return "";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelection::getAttributeValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION :
//
PkgAttributeValue PMSelection::getAttributeValue( PMSelectionAttribute attr ) const
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
//	METHOD NAME : PMSelection::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMSelection::dumpOn( ostream & str ) const
{
  PMObject::dumpOn( str );
  return str;
}

