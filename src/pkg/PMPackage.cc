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

#include <sstream> // for querying provides, requires etc.

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

std::string PMPackage::getAttributeValue(PMSolvableAttribute attr)
{
    string str;
    switch(attr)
    {
	case ATTR_NAME:
	    str = this->name();
	    break;
	case ATTR_VERSION:
	    str = this->edition().version();
	    break;
	case ATTR_RELEASE:
	    str = this->edition().release();
	    break;
	case ATTR_REQUIRES:
	    {
		ostringstream ost;
		ost << (this->requires()) << endl;
		str = ost.str();
	    }
	    break;
	case ATTR_PREREQUIRES:
	    {
		ostringstream ost;
		ost << (this->prerequires()) << endl;
		str = ost.str();
	    }
	    break;
	case ATTR_PROVIDES:
	    {
		ostringstream ost;
		ost << (this->provides()) << endl;
		str = ost.str();
	    }
	    break;
	case ATTR_CONFLICTS:
	    {
		ostringstream ost;
		ost << (this->conflicts()) << endl;
		str = ost.str();
	    }
	    break;
	case ATTR_OBSOLETES:
	    {
		ostringstream ost;
		ost << (this->obsoletes()) << endl;
		str = ost.str();
	    }
	    break;
	case PMSLV_NUM_ATTRIBUTES:
	    // invalid
	    return "invalid query";
    }

    return str;
}

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

std::string PMPackage::getAttributeName(PMSolvableAttribute attr)
{
    return PMObject::getAttributeName(attr);
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
	case ATTR_AUTHOR:
	    str = "AUTHOR";
	    break;
	case ATTR_FILENAMES:
	    str = "FILENAMES";
	    break;
	case PMPackage::PKG_NUM_ATTRIBUTES:
	    // invalid
	    return "invalid query";
    }

    if(!str)
    {
	ERR << "invalid enum value" << endl;
	str = "invalid query";
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

