/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       PMYouPackageDataProvider.cc
   Purpose:    Implements PackageDataProvider for YOU patches.
   Author:     Cornelius Schumacher <cschum@suse.de>
   Maintainer: Cornelius Schumacher <cschum@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/PMYouPackageDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPackageDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER(PMYouPackageDataProvider, PMPackageDataProvider, PMDataProvider );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPackageDataProvider::PMYouPackageDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMYouPackageDataProvider::PMYouPackageDataProvider()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPackageDataProvider::~PMYouPackageDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMYouPackageDataProvider::~PMYouPackageDataProvider()
{
}

const std::string
PMYouPackageDataProvider::summary() const
{
    return "";
}

const std::list<std::string>
PMYouPackageDataProvider::description() const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMYouPackageDataProvider::insnotify() const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMYouPackageDataProvider::delnotify() const
{
    return std::list<std::string>();
}

const FSize
PMYouPackageDataProvider::size() const
{
    return FSize(0);
}

PkgAttributeValue PMYouPackageDataProvider::getAttributeValue( constPMObjectPtr obj_r,
							       PMObject::PMObjectAttribute attr )
{
    PkgAttributeValue ret;
    return ret;
}

PkgAttributeValue PMYouPackageDataProvider::getAttributeValue( constPMPackagePtr pkg_r,
							       PMPackage::PMPackageAttribute attr )
{
    PkgAttributeValue ret;
    return ret;
}

#if 0
/** inject attibute to cache */
void PMYouPackageDataProvider::setAttributeValue(
    PMPackagePtr pkg, PMPackage::PMPackageAttribute attr, const PkgAttributeValue& value)
{
    _setAttributeValue(pkg,attr,value);
}

/** inject attibute to cache */
void PMYouPackageDataProvider::setAttributeValue(
    PMPackagePtr pkg, PMObject::PMObjectAttribute attr, const PkgAttributeValue& value)
{
    _setAttributeValue(pkg,attr,value);
}
#endif

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPackageDataProvider::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMYouPackageDataProvider::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}
