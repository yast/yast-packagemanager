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

   File:       PMRpmPackageDataProvider.cc
   Purpose:    Implements PackageDataProvider for installed rpms
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/PMRpmPackageDataProvider.h>
#include <y2pm/PkgName.h>
#include <y2pm/RpmDb.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMRpmPackageDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER(PMRpmPackageDataProvider, PMPackageDataProvider, PMDataProvider );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMRpmPackageDataProvider::PMRpmPackageDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMRpmPackageDataProvider::PMRpmPackageDataProvider(RpmDbPtr rpmdb)
    : _rpmdb(rpmdb)
{
}

/**
 * hint before accessing multiple attributes
 */
void
PMRpmPackageDataProvider::startRetrieval() const
{
    return;
}

/**
 * hint after accessing multiple attributes
 */
void
PMRpmPackageDataProvider::stopRetrieval() const
{
    return;
}

const std::string
PMRpmPackageDataProvider::summary () const
{
    std::string value;
    _rpmdb->queryPackage (_package, "%{SUMMARY}", value);
    return value;
}

const std::list<std::string>
PMRpmPackageDataProvider::description () const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (_package, "%{DESCRIPTION}", value);
    return value;
}

const FSize
PMRpmPackageDataProvider::size () const
{
    std::string value;
    _rpmdb->queryPackage (_package, "%{SIZE}", value);
    return FSize (atoll(value.c_str()));
}

const Date
PMRpmPackageDataProvider::buildtime () const
{
    // take directly from PkgEdition, RpmDb sets it
    return Date (_package->edition().buildtime());
}

const std::string
PMRpmPackageDataProvider::buildhost () const
{
    std::string value;
    _rpmdb->queryPackage (_package, "%{SUMMARY}", value);
    return value;
}

const Date
PMRpmPackageDataProvider::installtime () const
{
    std::string value;
    _rpmdb->queryPackage (_package, "%{INSTALLTIME}", value);
    return Date (value);
}

const std::string
PMRpmPackageDataProvider::distribution () const
{
    std::string value;
    _rpmdb->queryPackage (_package, "%{DISTRIBUTION}", value);
    return value;
}

const std::string
PMRpmPackageDataProvider::vendor () const
{
    std::string value;
    _rpmdb->queryPackage (_package, "%{VENDOR}", value);
    return value;
}

const std::string
PMRpmPackageDataProvider::license () const
{
    std::string value;
    _rpmdb->queryPackage (_package, "%{LICENSE}", value);
    return value;
}

const std::string
PMRpmPackageDataProvider::packager () const
{
    std::string value;
    _rpmdb->queryPackage (_package, "%{PACKAGER}", value);
    return value;
}

const std::string
PMRpmPackageDataProvider::group () const
{
    std::string value;
    _rpmdb->queryPackage (_package, "%{GROUP}", value);
    return value;
}

const std::list<std::string>
PMRpmPackageDataProvider::changelog () const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (_package, "[* %{CHANGELOGTIME:day} %{CHANGELOGNAME}\\n\\n%{CHANGELOGTEXT}\\n\\n]", value);
    return value;
}

const std::string
PMRpmPackageDataProvider::url () const
{
    std::string value;
    _rpmdb->queryPackage (_package, "%{URL}", value);
    return value;
}

const std::string
PMRpmPackageDataProvider::os () const
{
    std::string value;
    _rpmdb->queryPackage (_package, "%{OS}", value);
    return value;
}

const std::list<std::string>
PMRpmPackageDataProvider::prein () const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (_package, "%{PREIN}", value);
    return value;
}

const std::list<std::string>
PMRpmPackageDataProvider::postin () const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (_package, "%{POSTIN}", value);
    return value;
}

const std::list<std::string>
PMRpmPackageDataProvider::preun () const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (_package, "%{PREUN}", value);
    return value;
}

const std::list<std::string>
PMRpmPackageDataProvider::postun () const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (_package, "%{POSTUN}", value);
    return value;
}

const std::string
PMRpmPackageDataProvider::sourcerpm () const
{
    std::string value;
    _rpmdb->queryPackage (_package, "%{SOURCERPM}", value);
    return value;
}

const std::list<std::string>
PMRpmPackageDataProvider::filenames () const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (_package, "[%{FILENAMES}\n]", value);
    return value;
}

PkgAttributeValue PMRpmPackageDataProvider::getAttributeValue( constPMObjectPtr obj_r,
							       PMObject::PMObjectAttribute attr )
{
    return PkgAttributeValue();
}

PkgAttributeValue PMRpmPackageDataProvider::getAttributeValue( constPMPackagePtr pkg_r,
							       PMPackage::PMPackageAttribute attr )
{
    return PkgAttributeValue();
}

/** inject attibute to cache */
void PMRpmPackageDataProvider::setAttributeValue(
    PMPackagePtr pkg, PMPackage::PMPackageAttribute attr, const PkgAttributeValue& value)
{
    return;
}

/** inject attibute to cache */
void PMRpmPackageDataProvider::setAttributeValue(
    PMPackagePtr pkg, PMObject::PMObjectAttribute attr, const PkgAttributeValue& value)
{
    return;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMRpmPackageDataProvider::~PMRpmPackageDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMRpmPackageDataProvider::~PMRpmPackageDataProvider()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMRpmPackageDataProvider::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMRpmPackageDataProvider::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

