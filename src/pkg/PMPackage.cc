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

/-*/

#ifndef Y2LOG
#define Y2LOG __FILE__
#endif

#include <y2util/Y2SLog.h>

#include <y2pm/PMPackage.h>
#include <y2pm/PMPackageDataProvider.h>

using namespace std;


const char* const PMPackage::PackageAttributeNames[] = {
      "Buildtime",
      "Installtime",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)",
      "type yourself, i'm too lazy ;)"
};

const char* const PMPackage::PackageQueryFormat[] = {
      "%{SUMMARY}",
      "%{DESCRIPTION}",
      "%{SIZE}",
      "%{BUILDTIME}",
      "%{INSTALLTIME}",
      "%{DISTRIBUTION}",
      "%{VENDOR}",
      "%{LICENSE}",
      "%{PACKAGER}",
      "%{GROUP}",
      "%{CHANGELOG}",
      "%{URL}",
      "%{OS}",
      "%{ARCH}",
      "%{PREIN}",
      "%{POSTIN}",
      "%{PREUN}",
      "%{POSTUN}",
      "%{SOURCERPM}",
      "%{ARCHIVESIZE}"
};




///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackage
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(PMPackage);

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
		      const PkgArch &    arch_r )
    : PMObject( name_r, edition_r )
    , _arch( arch_r )
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
//	METHOD NAME : PMPackage::Summary
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
//
/*
string PMPackage::Summary() const
{
  if ( !_dataProvider ) {
    ERR << "No DataProvider for " << *this << endl;
    return PMObject::Summary();
  }

  // TBD: get data from _dataProvider
  return string( "Faked summary for Package" ) + string( name() );
}
*/

// get data from data provider
std::string PMPackage::getAttributeValue(PMObjectAttribute attr)
{
    if(_dataProvider == NULL)
    {
	ERR << name() << ": no dataprovider set" << endl;
	return "";
    }
    return _dataProvider->getAttributeValue(this,attr);
}

std::string PMPackage::getAttributeValue(PMPackageAttribute attr)
{
    if(_dataProvider == NULL)
    {
	ERR << name() << ": no dataprovider set" << endl;
	return "";
    }
    return _dataProvider->getAttributeValue(this,attr);
}

std::string PMPackage::getAttributeName(PMPackageAttribute attr)
{
    return PackageAttributeNames[attr-PMOBJ_NUM_ATTRIBUTES];
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
  PMObject::dumpOn( str );
  str << "Architecture: " << _arch << endl;
  return str;
}

