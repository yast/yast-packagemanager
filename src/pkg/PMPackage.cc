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

std::string PMPackage::getAttributeName(PMObjectAttribute attr)
{
    return PMObject::getAttributeName(attr);
}

std::string PMPackage::getAttributeName(PMPackageAttribute attr)
{
    const char* str = NULL;
    switch(attr)
    {
	case ATTR_BUILDTIME:
	    str = "BUILDTIME";
	    break;
	case ATTR_BUILDHOST:
	    str = "BUILDHOST";
	    break;
	case ATTR_INSTALLTIME:
	    str = "INSTALLTIME";
	    break;
	case ATTR_DISTRIBUTION:
	    str = "DISTRIBUTION";
	    break;
	case ATTR_VENDOR:
	    str = "VENDOR";
	    break;
	case ATTR_LICENSE:
	    str = "LICENSE";
	    break;
	case ATTR_PACKAGER:
	    str = "PACKAGER";
	    break;
	case ATTR_GROUP:
	    str = "GROUP";
	    break;
	case ATTR_CHANGELOG:
	    str = "CHANGELOG";
	    break;
	case ATTR_URL:
	    str = "URL";
	    break;
	case ATTR_OS:
	    str = "OS";
	    break;
	case ATTR_ARCH:
	    str = "ARCH";
	    break;
	case ATTR_PREIN:
	    str = "PREIN";
	    break;
	case ATTR_POSTIN:
	    str = "POSTIN";
	    break;
	case ATTR_PREUN:
	    str = "PREUN";
	    break;
	case ATTR_POSTUN:
	    str = "POSTUN";
	    break;
	case ATTR_SOURCERPM:
	    str = "SOURCERPM";
	    break;
	case ATTR_ARCHIVESIZE:
	    str = "ARCHIVESIZE";
	    break;
	case PMPackage::PKG_NUM_ATTRIBUTES:
	    // invalid
	    return "invalid query";
    }
    
    return str;
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

