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

  File:       PMPackage.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Defines the Package object.

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMPackage.h>
#include <y2pm/PMPackageDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackage
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER( PMPackage, PMObject, PMSolvable );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackage::PMPackage
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMPackage::PMPackage( const PkgName &    name_r,
		      const PkgEdition & edition_r,
		      const PkgArch &    arch_r,
		      PMPackageDataProviderPtr dataProvider_r )
    : PMObject( name_r, edition_r )
    , _arch( arch_r )
    , _dataProvider( dataProvider_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackage::~PMPackage
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMPackage::~PMPackage()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackage::getAttributeName
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string PMPackage::getAttributeName( PMPackageAttribute attr ) const
{
    if (attr < PMPackage::PKG_ATTR_BEGIN)
	return getAttributeName((PMObject::PMObjectAttribute)attr);

  switch ( attr ) {

#define ENUM_OUT(V) case ATTR_##V: return #V; break
    ENUM_OUT( BUILDTIME );
    ENUM_OUT( BUILDHOST );
    ENUM_OUT( INSTALLTIME );
    ENUM_OUT( DISTRIBUTION );
    ENUM_OUT( VENDOR );
    ENUM_OUT( LICENSE );
    ENUM_OUT( PACKAGER );
    ENUM_OUT( GROUP );
    ENUM_OUT( CHANGELOG );
    ENUM_OUT( URL );
    ENUM_OUT( OS );
    ENUM_OUT( ARCH );
    ENUM_OUT( PREIN );
    ENUM_OUT( POSTIN );
    ENUM_OUT( PREUN );
    ENUM_OUT( POSTUN );
    ENUM_OUT( SOURCERPM );
    ENUM_OUT( ARCHIVESIZE );
    ENUM_OUT( AUTHOR );
    ENUM_OUT( FILENAMES );
    ENUM_OUT( RECOMMENDS );
    ENUM_OUT( SUGGESTS );
    ENUM_OUT( LOCATION );
    ENUM_OUT( KEYWORDS );
#undef ENUM_OUT

  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  case PKG_NUM_ATTRIBUTES:
    // illegal attr value
    break;
  }
  // HERE: illegal attr value or forgott do adjust switch.
  ERR << "Illegal PackageAttribute(" << attr << ')' << endl;
  return "";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackage::getAttributeValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION :
//
PkgAttributeValue PMPackage::getAttributeValue( PMPackageAttribute attr ) const
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
//	METHOD NAME : PMPackage::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMPackage::dumpOn( ostream & str ) const
{
  PMObject::dumpOn( str ) << '(' << _arch << ')';
  return str;
}

