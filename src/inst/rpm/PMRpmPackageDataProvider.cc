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

IMPL_DERIVED_POINTER(PMRpmPackageDataProvider, PMPackageDataProvider, PMDataProvider );


//-----------------------------------------------------------------
// static class members

PMPackagePtr PMRpmPackageDataProvider::_cachedPkg;
struct rpmCache *PMRpmPackageDataProvider::_theCache = 0;


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
    if (_theCache == 0)
	_theCache = new (struct rpmCache);
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
    if (_theCache != 0)
	delete _theCache;
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


//---------------------------------------------------------

/**
 * fill _theCache with data from package
 */
void
PMRpmPackageDataProvider::fillCache (PMPackagePtr package) const
{
    _rpmdb->queryCache (package, _theCache);
    _cachedPkg = package;
    return;
}

//---------------------------------------------------------
const std::string
PMRpmPackageDataProvider::summary () const
{
    return _attr_SUMMARY;
}

const std::list<std::string>
PMRpmPackageDataProvider::description () const
{
    if (_package != _cachedPkg)
	fillCache (_package);
    return _theCache->_description;
}

const FSize
PMRpmPackageDataProvider::size () const
{
    return _attr_SIZE;
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
    if (_package != _cachedPkg)
	fillCache (_package);
    return _theCache->_buildhost;
}

const Date
PMRpmPackageDataProvider::installtime () const
{
    if (_package != _cachedPkg)
	fillCache (_package);
    return _theCache->_installtime;
}

const std::string
PMRpmPackageDataProvider::distribution () const
{
    if (_package != _cachedPkg)
	fillCache (_package);
    return _theCache->_distribution;
}

const std::string
PMRpmPackageDataProvider::vendor () const
{
    if (_package != _cachedPkg)
	fillCache (_package);
    return _theCache->_vendor;
}

const std::string
PMRpmPackageDataProvider::license () const
{
    if (_package != _cachedPkg)
	fillCache (_package);
    return _theCache->_license;
}

const std::string
PMRpmPackageDataProvider::packager () const
{
    if (_package != _cachedPkg)
	fillCache (_package);
    return _theCache->_packager;
}

const std::string
PMRpmPackageDataProvider::group () const
{
    if (_attr_GROUP == 0)
	return "";
    return Y2PM::packageManager().rpmGroup (_attr_GROUP);
}

const YStringTreeItem *
PMRpmPackageDataProvider::group_ptr () const
{
    return _attr_GROUP;
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
    if (_package != _cachedPkg)
	fillCache (_package);
    return _theCache->_url;
}

const std::string
PMRpmPackageDataProvider::os () const
{
    if (_package != _cachedPkg)
	fillCache (_package);
    return _theCache->_os;
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
    if (_package != _cachedPkg)
	fillCache (_package);
    return _theCache->_sourcerpm;
}

const std::list<std::string>
PMRpmPackageDataProvider::filenames () const
{
    std::list<std::string> value;
    _rpmdb->queryPackage (_package, "[%{FILENAMES}\n]", value);
    return value;
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

