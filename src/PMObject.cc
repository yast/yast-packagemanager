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

   File:       PMObject.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMObject.h>
#include <y2pm/PMSelectable.h>
#include <y2pm/PMDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMObject
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER( PMObject, PMSolvable, PMSolvable );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::PMObject
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMObject::PMObject( const PkgName &    name_r,
		    const PkgEdition & edition_r )
    : PMSolvable(name_r, edition_r)
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::~PMObject
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMObject::~PMObject()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::getAttributeName
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string PMObject::getAttributeName( PMObjectAttribute attr ) const
{
  switch ( attr ) {

#define ENUM_OUT(V) case ATTR_##V: return #V; break
    ENUM_OUT( SUMMARY );
    ENUM_OUT( DESCRIPTION );
    ENUM_OUT( SIZE );
#undef ENUM_OUT

  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  case PMOBJ_NUM_ATTRIBUTES:
    // illegal attr value
    break;
  }
  // HERE: illegal attr value or forgott do adjust switch.
  ERR << "Illegal PMObjectAttribute(" << attr << ')' << endl;
  return "";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::getAttributeValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION :
//
PkgAttributeValue PMObject::getAttributeValue( PMObjectAttribute attr ) const
{
  PMDataProviderPtr dataprovider( dataProvider() );
  if ( !dataprovider ) {
    ERR << "No dataprovider for " << *this << endl;
    return PkgAttributeValue();
  }
  return dataprovider->getAttributeValue( this, attr );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMObject::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str )
    << '(' << (_selectable ? "MANAGED" : "unmanaged" ) << ')'
    << '(' << _name << '-' << _edition.version() << '-' << _edition.release() << ')';
  return str;
}

///////////////////////////////////////////////////////////////////
//
// Shortcuts for UI
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::isInstalledObj
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMObject::isInstalledObj() const
{
  return( hasSelectable() && _selectable->installedObj() == this );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : PMObject::isCandidateObj
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMObject::isCandidateObj() const
{
  return( hasSelectable() && _selectable->candidateObj() == this );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : PMObject::isAvailableOnly
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMObject::isAvailableOnly() const
{
  return( hasSelectable()
	  && _selectable->installedObj() != this
	  && _selectable->candidateObj() != this );
}






