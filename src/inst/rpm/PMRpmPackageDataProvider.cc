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

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMRpmPackageDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER(PMRpmPackageDataProvider, PMPackageDataProvider, PMPackageDataProvider );


//-----------------------------------------------------------------
// static class members

PMPackagePtr PMRpmPackageDataProvider::_cachedPkg;
rpmCache     PMRpmPackageDataProvider::_theCache;


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
    , _attr_GROUP(0)
{
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

//---------------------------------------------------------

/**
 * fill _theCache with data from package
 */
inline void
PMRpmPackageDataProvider::fillCache (PMPackagePtr package) const
{
  if ( package != _cachedPkg ) {
    _rpmdb->queryCache (package, &_theCache);
    _cachedPkg = package;
  }
  return;
}

//---------------------------------------------------------
std::string
PMRpmPackageDataProvider::summary ( const PMPackage & pkg_r ) const
{
  return _attr_SUMMARY;
}

std::list<std::string>
PMRpmPackageDataProvider::description ( const PMPackage & pkg_r ) const
{
    fillCache (mkPtr(pkg_r));
    return _theCache._description;
}

FSize
PMRpmPackageDataProvider::size ( const PMPackage & pkg_r ) const
{
    return _attr_SIZE;
}

Date
PMRpmPackageDataProvider::buildtime ( const PMPackage & pkg_r ) const
{
    // take directly from PkgEdition, RpmDb sets it
    return Date (pkg_r.edition().buildtime());
}

std::string
PMRpmPackageDataProvider::buildhost ( const PMPackage & pkg_r ) const
{
    fillCache (mkPtr(pkg_r));
    return _theCache._buildhost;
}

Date
PMRpmPackageDataProvider::installtime ( const PMPackage & pkg_r ) const
{
    fillCache (mkPtr(pkg_r));
    return _theCache._installtime;
}

std::string
PMRpmPackageDataProvider::distribution ( const PMPackage & pkg_r ) const
{
    fillCache (mkPtr(pkg_r));
    return _theCache._distribution;
}

std::string
PMRpmPackageDataProvider::vendor ( const PMPackage & pkg_r ) const
{
    fillCache (mkPtr(pkg_r));
    return _theCache._vendor;
}

std::string
PMRpmPackageDataProvider::license ( const PMPackage & pkg_r ) const
{
    fillCache (mkPtr(pkg_r));
    return _theCache._license;
}

std::string
PMRpmPackageDataProvider::packager ( const PMPackage & pkg_r ) const
{
    fillCache (mkPtr(pkg_r));
    return _theCache._packager;
}

std::string
PMRpmPackageDataProvider::group ( const PMPackage & pkg_r ) const
{
    if (_attr_GROUP == 0)
	return "";
    return Y2PM::packageManager().rpmGroup (_attr_GROUP);
}

YStringTreeItem *
PMRpmPackageDataProvider::group_ptr ( const PMPackage & pkg_r ) const
{
    return _attr_GROUP;
}

std::list<std::string>
PMRpmPackageDataProvider::changelog ( const PMPackage & pkg_r ) const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (mkPtr(pkg_r), "[* %{CHANGELOGTIME:day} %{CHANGELOGNAME}\\n\\n%{CHANGELOGTEXT}\\n\\n]", value);
    return value;
}

std::string
PMRpmPackageDataProvider::url ( const PMPackage & pkg_r ) const
{
    fillCache (mkPtr(pkg_r));
    return _theCache._url;
}

std::string
PMRpmPackageDataProvider::os ( const PMPackage & pkg_r ) const
{
    fillCache (mkPtr(pkg_r));
    return _theCache._os;
}

std::list<std::string>
PMRpmPackageDataProvider::prein ( const PMPackage & pkg_r ) const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (mkPtr(pkg_r), "%{PREIN}", value);
    return value;
}

std::list<std::string>
PMRpmPackageDataProvider::postin ( const PMPackage & pkg_r ) const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (mkPtr(pkg_r), "%{POSTIN}", value);
    return value;
}

std::list<std::string>
PMRpmPackageDataProvider::preun ( const PMPackage & pkg_r ) const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (mkPtr(pkg_r), "%{PREUN}", value);
    return value;
}

std::list<std::string>
PMRpmPackageDataProvider::postun ( const PMPackage & pkg_r ) const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (mkPtr(pkg_r), "%{POSTUN}", value);
    return value;
}

std::string
PMRpmPackageDataProvider::sourcerpm ( const PMPackage & pkg_r ) const
{
    fillCache (mkPtr(pkg_r));
    return _theCache._sourcerpm;
}

std::list<std::string>
PMRpmPackageDataProvider::filenames ( const PMPackage & pkg_r ) const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (mkPtr(pkg_r), "[%{FILENAMES}\n]", value);
    return value;
}
