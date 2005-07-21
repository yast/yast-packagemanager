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

   File:       YUMPackageDataProvider.cc
   Purpose:    Implements PackageDataProvider for YUM
   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>

#include "YUMPackageDataProvider.h"
#include <y2pm/InstSrcDataYUM.h>
#include <y2pm/InstSrcDescr.h>

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
namespace YUM
{ /////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  //	CLASS NAME : PackageDataProviderPtr
  //	CLASS NAME : constPackageDataProviderPtr
  ///////////////////////////////////////////////////////////////////
  IMPL_DERIVED_POINTER(PackageDataProvider, PMPackageDataProvider );

  ///////////////////////////////////////////////////////////////////
  //
  //
  //	METHOD NAME : PackageDataProvider::PackageDataProvider
  //	METHOD TYPE : Constructor
  //
  PackageDataProvider::PackageDataProvider( constInstSrcDataYUMPtr instSrcData_r )
  : _instSrcData( instSrcData_r )
  {
  }

  ///////////////////////////////////////////////////////////////////
  //
  //
  //	METHOD NAME : PackageDataProvider::~PackageDataProvider
  //	METHOD TYPE : Destructor
  //
  PackageDataProvider::~PackageDataProvider()
  {
  }

#if 0
  ///////////////////////////////////////////////////////////////////
  //
  //
  //	METHOD NAME : PackageDataProvider::fillCache
  //	METHOD TYPE : constRpmHeaderPtr
  //
  //      strore packages rpm header in cache
  //
  inline constRpmHeaderPtr PackageDataProvider::fillCache( PMPackagePtr package ) const
  {
    if ( package != _cachedPkg ) {
      _cachedData = _instSrcData->getHeaderAt( _cachepos );
      _cachedPkg = package;
      if ( !_cachedData ) {
        _INT("YUMPDCache") << "GOT NULL DATA for " << _cachedPkg << " at " << _cachepos << endl;
      }
    }
    return _cachedData;
  }

  //---------------------------------------------------------
  std::string
  PackageDataProvider::summary ( const PMPackage & pkg_r ) const
  {
    return _attr_SUMMARY;
  }

  std::list<std::string>
  PackageDataProvider::description ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(description);
    return stringutil::splitToLines( h->tag_description() );
  }

  FSize
  PackageDataProvider::size ( const PMPackage & pkg_r ) const
  {
    return _attr_SIZE;
  }

  bool PackageDataProvider::providesSources( const PMPackage & pkg_r ) const
  {
    return !_attr_SOURCELOC.empty();
  }

  std::string
  PackageDataProvider::instSrcLabel( const PMPackage & pkg_r ) const
  {
    if ( source(pkg_r) && source(pkg_r)->descr() ) {
      return source(pkg_r)->descr()->label();
    }
    return PMPackageDataProvider::instSrcLabel();
  }

  Vendor
  PackageDataProvider::instSrcVendor( const PMPackage & pkg_r ) const
  {
    if ( source(pkg_r) && source(pkg_r)->descr() ) {
      return source(pkg_r)->descr()->content_vendor();
    }
    return PMPackageDataProvider::instSrcVendor();
  }

  unsigned
  PackageDataProvider::instSrcRank( const PMPackage & pkg_r ) const
  {
    if ( source(pkg_r) && source(pkg_r)->descr() ) {
      return source(pkg_r)->descr()->default_rank();
    }
    return PMPackageDataProvider::instSrcRank();
  }

  PkgSplitSet PackageDataProvider::splitprovides( const PMPackage & pkg_r ) const
  {
    return _attr_SPLITPROVIDES;
  }

  Date
  PackageDataProvider::buildtime ( const PMPackage & pkg_r ) const
  {
    // take directly from PkgEdition, RpmHeader sets it
    return Date( pkg_r.edition().buildtime() );
  }

  std::string
  PackageDataProvider::buildhost ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(buildhost);
    return h->tag_buildhost();
  }

  std::string
  PackageDataProvider::distribution ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(distribution);
    return h->tag_distribution();
  }

  Vendor
  PackageDataProvider::vendor ( const PMPackage & pkg_r ) const
  {
    return _attr_VENDOR;
  }

  std::string
  PackageDataProvider::license ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(license);
    return h->tag_license();
  }

  std::string
  PackageDataProvider::packager ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(packager);
    return h->tag_packager();
  }

  std::string
  PackageDataProvider::group ( const PMPackage & pkg_r ) const
  {
    if (_attr_GROUP == 0)
      return "";
    return Y2PM::packageManager().rpmGroup (_attr_GROUP);
  }

  YStringTreeItem *
  PackageDataProvider::group_ptr ( const PMPackage & pkg_r ) const
  {
    return _attr_GROUP;
  }

  std::list<std::string>
  PackageDataProvider::changelog ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(changelog);
    return h->tag_changelog().asStringList();
  }

  std::string
  PackageDataProvider::url ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(url);
    return h->tag_url();
  }

  std::string
  PackageDataProvider::os ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(os);
    return h->tag_os();
  }

  std::list<std::string>
  PackageDataProvider::prein ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(prein);
    return stringutil::splitToLines( h->tag_prein() );
  }

  std::list<std::string>
  PackageDataProvider::postin ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(postin);
    return stringutil::splitToLines( h->tag_postin() );
  }

  std::list<std::string>
  PackageDataProvider::preun ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(preun);
    return stringutil::splitToLines( h->tag_preun() );
  }

  std::list<std::string>
  PackageDataProvider::postun ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(postun);
    return stringutil::splitToLines( h->tag_postun() );
  }

  std::string PackageDataProvider::sourceloc( const PMPackage & pkg_r ) const
  {
    return _attr_SOURCELOC.asString();
  }

  FSize PackageDataProvider::sourcesize( const PMPackage & pkg_r ) const
  {
    return _attr_SOURCESIZE;
  }

  FSize PackageDataProvider::archivesize( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(archivesize);
    return h->tag_archivesize();
  }

  std::list<std::string>
  PackageDataProvider::filenames ( const PMPackage & pkg_r ) const
  {
    TRY_CACHE(filenames);
    return h->tag_filenames();
  }

  std::string PackageDataProvider::location( const PMPackage & pkg_r ) const
  {
    return _pkgfile.asString();
  }

  unsigned int PackageDataProvider::medianr( const PMPackage & pkg_r ) const
  {
    return 1; // No multiple media for InstSrcYUM
  }

  ///////////////////////////////////////////////////////////////////
  //
  //
  //	METHOD NAME : PackageDataProvider::providePkgToInstall
  //	METHOD TYPE : PMError
  //
  PMError PackageDataProvider::providePkgToInstall( const PMPackage & pkg_r, Pathname & path_r ) const
  {
    return _instSrcData->providePkgToInstall( _pkgfile, path_r );
  }

  ///////////////////////////////////////////////////////////////////
  //
  //
  //	METHOD NAME : PackageDataProvider::provideSrcPkgToInstall
  //	METHOD TYPE : PMError
  //
  PMError PackageDataProvider::provideSrcPkgToInstall( const PMPackage & pkg_r, Pathname & path_r ) const
  {
    return _instSrcData->providePkgToInstall( _attr_SOURCELOC, path_r );
  }

  ///////////////////////////////////////////////////////////////////
  //
  //
  //	METHOD NAME : PackageDataProvider::source
  //	METHOD TYPE : constInstSrcPtr
  //
  constInstSrcPtr PackageDataProvider::source( const PMPackage & pkg_r ) const
  {
    return _instSrcData->attachedToInstSrc();
  }

  ///////////////////////////////////////////////////////////////////
  //
  //
  //	METHOD NAME : PackageDataProvider::isRemote
  //	METHOD TYPE : bool
  //
  //	DESCRIPTION : call InstSrc to check if it is a remote source
  //
  bool PackageDataProvider::isRemote( const PMPackage & pkg_r ) const
  {
    return( source(pkg_r) ? source(pkg_r)->isRemote() : false );
  }

  ///////////////////////////////////////////////////////////////////
  //
  //
  //	METHOD NAME : PackageDataProvider::du
  //	METHOD TYPE : void
  //
  //	DESCRIPTION :
  //
  void PackageDataProvider::du( const PMPackage & pkg_r, PkgDu & dudata_r ) const
  {
    constRpmHeaderPtr h = fillCache (mkPtr(pkg_r));
    if ( !h )
      PMPackageDataProvider::du( dudata_r );
    else
      h->tag_du( dudata_r );
  }
#endif

  /////////////////////////////////////////////////////////////////
} // namespace YUM
///////////////////////////////////////////////////////////////////

