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

   File:       PMPLAINPackageDataProvider.cc
   Purpose:    Implements PackageDataProvider for installed rpms
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>

#include <y2pm/PMPLAINPackageDataProvider.h>
#include <y2pm/InstSrcDataPLAIN.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/RpmHeader.h>
#include <y2pm/PMPackageManager.h>

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMPLAINPackageDataProviderPtr
//	CLASS NAME : constPMPLAINPackageDataProviderPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(PMPLAINPackageDataProvider, PMPackageDataProvider );

///////////////////////////////////////////////////////////////////
// static class members
///////////////////////////////////////////////////////////////////

PMPackagePtr         PMPLAINPackageDataProvider::_cachedPkg;
constRpmHeaderPtr PMPLAINPackageDataProvider::_cachedData;

#define TRY_CACHE(fnc) \
  constRpmHeaderPtr h = fillCache( mkPtr(pkg_r) ); \
  if ( !h ) return PMPackageDataProvider::fnc()

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPLAINPackageDataProvider::PMPLAINPackageDataProvider
//	METHOD TYPE : Constructor
//
PMPLAINPackageDataProvider::PMPLAINPackageDataProvider( constInstSrcDataPLAINPtr instSrcData_r,
							unsigned cachepos_r, const Pathname & pkgfile_r )
    : _instSrcData( instSrcData_r )
    , _cachepos( cachepos_r )
    , _pkgfile( pkgfile_r )
    , _attr_GROUP( 0 )
    , _attr_SOURCESIZE( 0 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPLAINPackageDataProvider::~PMPLAINPackageDataProvider
//	METHOD TYPE : Destructor
//
PMPLAINPackageDataProvider::~PMPLAINPackageDataProvider()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPLAINPackageDataProvider::loadStaticData
//	METHOD TYPE : void
//
void PMPLAINPackageDataProvider::loadStaticData( constRpmHeaderPtr h )
{
  if ( !h ) {
    INT << "Got NULL static data from RpmDb!" << endl;
    return;
  }
  _attr_SUMMARY     = h->tag_summary();
  _attr_SIZE        = h->tag_size();
  _attr_GROUP       = Y2PM::packageManager().addRpmGroup( h->tag_group() );
  _attr_VENDOR      = Vendor( h->tag_vendor() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPLAINPackageDataProvider::fillCache
//	METHOD TYPE : constRpmHeaderPtr
//
//      strore packages rpm header in cache
//
inline constRpmHeaderPtr PMPLAINPackageDataProvider::fillCache( PMPackagePtr package ) const
{
  if ( package != _cachedPkg ) {
    _cachedData = _instSrcData->getHeaderAt( _cachepos );
    _cachedPkg = package;
    if ( !_cachedData ) {
      _INT("PLAINPDCache") << "GOT NULL DATA for " << _cachedPkg << " at " << _cachepos << endl;
    }
  }
  return _cachedData;
}

//---------------------------------------------------------
std::string
PMPLAINPackageDataProvider::summary ( const PMPackage & pkg_r ) const
{
  return _attr_SUMMARY;
}

std::list<std::string>
PMPLAINPackageDataProvider::description ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(description);
  return stringutil::splitToLines( h->tag_description() );
}

FSize
PMPLAINPackageDataProvider::size ( const PMPackage & pkg_r ) const
{
  return _attr_SIZE;
}

bool PMPLAINPackageDataProvider::providesSources( const PMPackage & pkg_r ) const
{
  return !_attr_SOURCELOC.empty();
}

std::string
PMPLAINPackageDataProvider::instSrcLabel( const PMPackage & pkg_r ) const
{
  if ( source(pkg_r) && source(pkg_r)->descr() ) {
    return source(pkg_r)->descr()->label();
  }
  return PMPackageDataProvider::instSrcLabel();
}

Vendor
PMPLAINPackageDataProvider::instSrcVendor( const PMPackage & pkg_r ) const
{
  if ( source(pkg_r) && source(pkg_r)->descr() ) {
    return source(pkg_r)->descr()->content_vendor();
  }
  return PMPackageDataProvider::instSrcVendor();
}

unsigned
PMPLAINPackageDataProvider::instSrcRank( const PMPackage & pkg_r ) const
{
  if ( source(pkg_r) && source(pkg_r)->descr() ) {
    return source(pkg_r)->descr()->default_rank();
  }
  return PMPackageDataProvider::instSrcRank();
}

PkgSplitSet PMPLAINPackageDataProvider::splitprovides( const PMPackage & pkg_r ) const
{
  return _attr_SPLITPROVIDES;
}

Date
PMPLAINPackageDataProvider::buildtime ( const PMPackage & pkg_r ) const
{
  // take directly from PkgEdition, RpmHeader sets it
  return Date( pkg_r.edition().buildtime() );
}

std::string
PMPLAINPackageDataProvider::buildhost ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(buildhost);
  return h->tag_buildhost();
}

std::string
PMPLAINPackageDataProvider::distribution ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(distribution);
  return h->tag_distribution();
}

Vendor
PMPLAINPackageDataProvider::vendor ( const PMPackage & pkg_r ) const
{
  return _attr_VENDOR;
}

std::string
PMPLAINPackageDataProvider::license ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(license);
  return h->tag_license();
}

std::string
PMPLAINPackageDataProvider::packager ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(packager);
  return h->tag_packager();
}

std::string
PMPLAINPackageDataProvider::group ( const PMPackage & pkg_r ) const
{
  if (_attr_GROUP == 0)
    return "";
  return Y2PM::packageManager().rpmGroup (_attr_GROUP);
}

YStringTreeItem *
PMPLAINPackageDataProvider::group_ptr ( const PMPackage & pkg_r ) const
{
  return _attr_GROUP;
}

std::list<std::string>
PMPLAINPackageDataProvider::changelog ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(changelog);
  return h->tag_changelog().asStringList();
}

std::string
PMPLAINPackageDataProvider::url ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(url);
  return h->tag_url();
}

std::string
PMPLAINPackageDataProvider::os ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(os);
  return h->tag_os();
}

std::list<std::string>
PMPLAINPackageDataProvider::prein ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(prein);
  return stringutil::splitToLines( h->tag_prein() );
}

std::list<std::string>
PMPLAINPackageDataProvider::postin ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(postin);
  return stringutil::splitToLines( h->tag_postin() );
}

std::list<std::string>
PMPLAINPackageDataProvider::preun ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(preun);
  return stringutil::splitToLines( h->tag_preun() );
}

std::list<std::string>
PMPLAINPackageDataProvider::postun ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(postun);
  return stringutil::splitToLines( h->tag_postun() );
}

std::string PMPLAINPackageDataProvider::sourceloc( const PMPackage & pkg_r ) const
{
  return _attr_SOURCELOC.asString();
}

FSize PMPLAINPackageDataProvider::sourcesize( const PMPackage & pkg_r ) const
{
  return _attr_SOURCESIZE;
}

FSize PMPLAINPackageDataProvider::archivesize( const PMPackage & pkg_r ) const
{
  TRY_CACHE(archivesize);
  return h->tag_archivesize();
}

std::list<std::string>
PMPLAINPackageDataProvider::filenames ( const PMPackage & pkg_r ) const
{
  TRY_CACHE(filenames);
  return h->tag_filenames();
}

std::string PMPLAINPackageDataProvider::location( const PMPackage & pkg_r ) const
{
  return _pkgfile.asString();
}

unsigned int PMPLAINPackageDataProvider::medianr( const PMPackage & pkg_r ) const
{
  return 1; // No multiple media for InstSrcPLAIN
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPLAINPackageDataProvider::providePkgToInstall
//	METHOD TYPE : PMError
//
PMError PMPLAINPackageDataProvider::providePkgToInstall( const PMPackage & pkg_r, Pathname & path_r ) const
{
  return _instSrcData->providePkgToInstall( _pkgfile, path_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPLAINPackageDataProvider::provideSrcPkgToInstall
//	METHOD TYPE : PMError
//
PMError PMPLAINPackageDataProvider::provideSrcPkgToInstall( const PMPackage & pkg_r, Pathname & path_r ) const
{
  return _instSrcData->providePkgToInstall( _attr_SOURCELOC, path_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPLAINPackageDataProvider::source
//	METHOD TYPE : constInstSrcPtr
//
constInstSrcPtr PMPLAINPackageDataProvider::source( const PMPackage & pkg_r ) const
{
  return _instSrcData->attachedToInstSrc();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPLAINPackageDataProvider::isRemote
//	METHOD TYPE : bool
//
//	DESCRIPTION : call InstSrc to check if it is a remote source
//
bool PMPLAINPackageDataProvider::isRemote( const PMPackage & pkg_r ) const
{
  return( source(pkg_r) ? source(pkg_r)->isRemote() : false );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPLAINPackageDataProvider::du
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMPLAINPackageDataProvider::du( const PMPackage & pkg_r, PkgDu & dudata_r ) const
{
  constRpmHeaderPtr h = fillCache (mkPtr(pkg_r));
  if ( !h )
    PMPackageDataProvider::du( dudata_r );
  else
    h->tag_du( dudata_r );
}

