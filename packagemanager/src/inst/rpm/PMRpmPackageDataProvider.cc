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
#include <y2util/stringutil.h>

#include <y2pm/PMRpmPackageDataProvider.h>
#include <y2pm/RpmDb.h>
#include <y2pm/RpmHeader.h>
#include <y2pm/PMPackageManager.h>

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMRpmPackageDataProviderPtr
//	CLASS NAME : constPMRpmPackageDataProviderPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(PMRpmPackageDataProvider, PMPackageDataProvider );

//-----------------------------------------------------------------
// static class members

PMPackagePtr         PMRpmPackageDataProvider::_cachedPkg;
constRpmHeaderPtr PMRpmPackageDataProvider::_cachedData;

#define TRY_CACHE(fnc) \
  constRpmHeaderPtr h = fillCache( mkPtr(pkg_r) ); \
  if ( !h ) return PMPackageDataProvider::fnc()

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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMRpmPackageDataProvider::loadStaticData
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMRpmPackageDataProvider::loadStaticData( constRpmHeaderPtr h )
{
  if ( !h ) {
    INT << "Got NULL static data from RpmDb!" << endl;
    return;
  }
  _attr_INSTALLTIME = h->tag_installtime();
  _attr_SUMMARY     = h->tag_summary();
  _attr_SIZE        = h->tag_size();
  _attr_GROUP       = Y2PM::packageManager().addRpmGroup( h->tag_group() );
  _attr_VENDOR      = Vendor( h->tag_vendor() );
}

//---------------------------------------------------------

/**
 * fill _theCache with data from package
 */
inline constRpmHeaderPtr
PMRpmPackageDataProvider::fillCache( PMPackagePtr package ) const
{
  if ( package != _cachedPkg ) {
    PMError err = _rpmdb->getData( package->name(), package->edition(), _cachedData );
    if ( err ) {
      _ERR("RpmPDCache") << "GOT NULL DATA for " << package << " -> " << err << endl;
    }
    _cachedPkg = package;
  }
  return _cachedData;
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
  TRY_CACHE(description);
  return stringutil::splitToLines( h->tag_description() );
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
  return Date( pkg_r.edition().buildtime() );
}

std::string
PMRpmPackageDataProvider::buildhost ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(buildhost);
  return h->tag_buildhost();
}

Date
PMRpmPackageDataProvider::installtime ( const PMPackage & pkg_r ) const
{
  return _attr_INSTALLTIME;
}

std::string
PMRpmPackageDataProvider::distribution ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(distribution);
  return h->tag_distribution();
}

Vendor
PMRpmPackageDataProvider::vendor ( const PMPackage & pkg_r ) const
{
  return _attr_VENDOR;
}

std::string
PMRpmPackageDataProvider::license ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(license);
  return h->tag_license();
}

std::string
PMRpmPackageDataProvider::packager ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(packager);
  return h->tag_packager();
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
  TRY_CACHE(changelog);
  return h->tag_changelog().asStringList();
}

std::string
PMRpmPackageDataProvider::url ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(url);
  return h->tag_url();
}

std::string
PMRpmPackageDataProvider::os ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(os);
  return h->tag_os();
}

std::list<std::string>
PMRpmPackageDataProvider::prein ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(prein);
  return stringutil::splitToLines( h->tag_prein() );
}

std::list<std::string>
PMRpmPackageDataProvider::postin ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(postin);
  return stringutil::splitToLines( h->tag_postin() );
}

std::list<std::string>
PMRpmPackageDataProvider::preun ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(preun);
  return stringutil::splitToLines( h->tag_preun() );
}

std::list<std::string>
PMRpmPackageDataProvider::postun ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(postun);
  return stringutil::splitToLines( h->tag_postun() );
}

std::list<std::string>
PMRpmPackageDataProvider::filenames ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(filenames);
  return h->tag_filenames();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMRpmPackageDataProvider::du
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMRpmPackageDataProvider::du( const PMPackage & pkg_r, PkgDu & dudata_r ) const
{
  constRpmHeaderPtr h = fillCache (mkPtr(pkg_r));
  if ( !h )
    PMPackageDataProvider::du( dudata_r );
  else
    h->tag_du( dudata_r );
}
