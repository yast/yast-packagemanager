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
			const PkgEdition & edition_r,
			PMYouPatchDataProviderPtr dataProvider_r )
#warning FIXME PkgArch("") used
    : PMObject( name_r, edition_r, PkgArch("") )
    , _kind( kind_invalid ), _updateOnlyInstalled( false )
    , _dataProvider( dataProvider_r )
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
#if 0
  switch ( attr ) {

#define ENUM_OUT(V) case ATTR_##V: return #V; break
    //ENUM_OUT( WAHTEVER_IS_FIRST );
#undef ENUM_OUT

  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  case PMYOU_NUM_ATTRIBUTES:
    // illegal attr value
    break;
  }
  // HERE: illegal attr value or forgott do adjust switch.
  ERR << "Illegal YouPatchAttribute(" << attr << ')' << endl;
#endif
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

  str << "Kind: " << kindLabel( _kind ) << endl;
  str << "ShortDescription: " << _shortDescription << endl;
  str << "LongDescription:" << endl << _longDescription << endl;
  str << "PreInformation:" << endl << _preInformation << endl;
  str << "PostInformation:" << endl << _postInformation << endl;
  str << "UpdateOnlyInstalled:" << ( _updateOnlyInstalled ? "true" : "false" ) << endl;

  return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatch::kindLabel
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
string PMYouPatch::kindLabel( Kind kind )
{
  switch ( kind ) {
    case kind_recommended:
      return "Recommended";
    case kind_security:
      return "Security";
    case kind_optional:
      return "Optional";
    case kind_document:
      return "Document";
    case kind_yast:
      return "YaST2";
    default:
      return "unknown";
   }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatch::kindLabel
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
void PMYouPatch::addPackage( const PMPackagePtr &pkg )
{
  _packages.push_back( pkg );
}
