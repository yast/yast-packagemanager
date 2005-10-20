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

 File:       YUMPackageDataProvider.h
 Purpose:    Implements PackageDataProvider for YUM
 Author:     Michael Andres <ma@suse.de>
 Maintainer: Michael Andres <ma@suse.de>

 /-*/

#ifndef YUMPackageDataProvider_h
#define YUMPackageDataProvider_h

#include <iosfwd>
#include <vector>
#include <map>

#include <y2util/YRpmGroupsTree.h>
#include <y2util/Vendor.h>
#include <y2util/FSize.h>
#include <y2util/Date.h>

#include <y2pm/PMPackageDataProvider.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMPackage.h>
#include <y2pm/InstSrcDataYUM.h>
#include <y2pm/InstSrcDescr.h>
#include <Y2PM.h>

#include "YUMPackageDataProviderPtr.h"
#include "y2pm/InstSrcDataYUM.h"
#include "YUMImpl.h"


///////////////////////////////////////////////////////////////////
namespace YUM
{ /////////////////////////////////////////////////////////////////

  class PackageDataProvider : public PMPackageDataProvider
  {
    REP_BODY(PackageDataProvider);

    friend class Impl;

  public:

    /**
     * constructor, destructor
     **/
    PackageDataProvider( const VarPtr<Impl> impl_r );

    virtual ~PackageDataProvider();

  public:

    // PMObject attributes
    virtual std::string            summary     ( const PMPackage & pkg_r ) const
    { return _attr_SUMMARY; }
    virtual std::list<std::string> description ( const PMPackage & pkg_r ) const
    { return _attr_DESCRIPTION; }
    virtual FSize                  size        ( const PMPackage & pkg_r ) const
    { return _attr_SIZE; }
    virtual bool                   providesSources( const PMPackage & pkg_r ) const
    { return !_sourcefile.empty(); }
    virtual std::string            instSrcLabel   ( const PMPackage & pkg_r ) const
    {
      if ( source(pkg_r) && source(pkg_r)->descr() )
        return source(pkg_r)->descr()->label();
      return PMPackageDataProvider::instSrcLabel();
    }
    virtual Vendor                 instSrcVendor  ( const PMPackage & pkg_r ) const
    {
      if ( source(pkg_r) && source(pkg_r)->descr() )
        return source(pkg_r)->descr()->content_vendor();
      return PMPackageDataProvider::instSrcVendor();
    }
    virtual unsigned               instSrcRank    ( const PMPackage & pkg_r ) const
    {
      if ( source(pkg_r) && source(pkg_r)->descr() )
        return source(pkg_r)->descr()->default_rank();
      return PMPackageDataProvider::instSrcRank();
    }

    // PMPackage attributes
    virtual PkgSplitSet            splitprovides( const PMPackage & pkg_r ) const
    { return _attr_SPLITPROVIDES; }
    virtual Date                   buildtime   ( const PMPackage & pkg_r ) const
    { return Date( pkg_r.edition().buildtime() ); }
    //virtual std::string            buildhost   ( const PMPackage & pkg_r ) const;
    //virtual std::string            distribution( const PMPackage & pkg_r ) const;
    virtual Vendor                 vendor      ( const PMPackage & pkg_r ) const
    { return _attr_VENDOR; }
    virtual std::string            license     ( const PMPackage & pkg_r ) const
    { return _attr_LICENSE; }
    //virtual std::string            packager    ( const PMPackage & pkg_r ) const;
    virtual std::string            group       ( const PMPackage & pkg_r ) const
    { return _attr_GROUP ? Y2PM::packageManager().rpmGroup( _attr_GROUP ) : std::string(); }
    virtual YStringTreeItem *      group_ptr   ( const PMPackage & pkg_r ) const
    { return _attr_GROUP; }
    //virtual std::list<std::string> changelog   ( const PMPackage & pkg_r ) const;
    //virtual std::string            url         ( const PMPackage & pkg_r ) const;
    //virtual std::string            os          ( const PMPackage & pkg_r ) const;
    //virtual std::list<std::string> prein       ( const PMPackage & pkg_r ) const;
    //virtual std::list<std::string> postin      ( const PMPackage & pkg_r ) const;
    //virtual std::list<std::string> preun       ( const PMPackage & pkg_r ) const;
    //virtual std::list<std::string> postun      ( const PMPackage & pkg_r ) const;
    virtual std::string            sourceloc   ( const PMPackage & pkg_r ) const
    { return _sourcefile.asString(); }
    virtual FSize                  sourcesize  ( const PMPackage & pkg_r ) const
    { return _attr_SOURCESIZE; }
    virtual FSize                  archivesize ( const PMPackage & pkg_r ) const
    { return _attr_ARCHIVESIZE; }
    //virtual std::list<std::string> authors     ( const PMPackage & pkg_r ) const;
    //virtual std::list<std::string> filenames   ( const PMPackage & pkg_r ) const;

    // suse packages values
    //virtual std::list<std::string> recommends  ( const PMPackage & pkg_r ) const;
    //virtual std::list<std::string> suggests    ( const PMPackage & pkg_r ) const;
    virtual std::string            location    ( const PMPackage & pkg_r ) const
    { return _pkgfile.asString(); }
    virtual unsigned int           medianr     ( const PMPackage & pkg_r ) const
    { return 1; }
    //virtual std::list<std::string> keywords    ( const PMPackage & pkg_r ) const;
    //virtual std::string            externalUrl ( const PMPackage & pkg_r ) const;
    //virtual std::list<PkgEdition>  patchRpmBaseVersions( const PMPackage & pkg_r ) const;
    //virtual FSize                  patchRpmSize( const PMPackage & pkg_r ) const;

    // physical access to the rpm file.
    virtual PMError providePkgToInstall( const PMPackage & pkg_r, Pathname& path_r ) const
    { return _impl->providePkgToInstall( _pkgfile, path_r ); }
    // physical access to the src.rpm file.
    virtual PMError provideSrcPkgToInstall( const PMPackage & pkg_r, Pathname& path_r ) const
    { return _impl->providePkgToInstall( _sourcefile, path_r );}

    // source for this package, needed for callbacks
    virtual constInstSrcPtr source   ( const PMPackage & pkg_r ) const
    { return _impl->instSrc(); }
    virtual bool	    isRemote ( const PMPackage & pkg_r ) const
    { return( source(pkg_r) ? source(pkg_r)->isRemote() : false ); }
    // dudata is special
    //virtual void du( const PMPackage & pkg_r, PkgDu & dudata_r ) const;

  private:

    // the source
    const VarPtr<Impl>     _impl;

    Pathname               _pkgfile;
    Pathname               _sourcefile;

    std::string            _attr_SUMMARY;
    std::list<std::string> _attr_DESCRIPTION;
    FSize                  _attr_SIZE;

    PkgSplitSet	           _attr_SPLITPROVIDES; // provided by YUMImpl
    Vendor                 _attr_VENDOR;
    std::string            _attr_LICENSE;
    YStringTreeItem *      _attr_GROUP;
    FSize                  _attr_SOURCESIZE;
    FSize                  _attr_ARCHIVESIZE;

  private:

    void loadAttr( YUMPrimaryDataPtr pdata_r )
    {
      _pkgfile              = pdata_r->location;

      _attr_SUMMARY         = pdata_r->summary;
      _attr_DESCRIPTION     = stringutil::splitToLines( pdata_r->description );
      _attr_SIZE            = FSize(pdata_r->sizeInstalled);

      _attr_VENDOR          = Vendor(pdata_r->vendor);
      _attr_LICENSE         = pdata_r->license;
      _attr_GROUP           = Y2PM::packageManager().addRpmGroup( pdata_r->group );
      _attr_ARCHIVESIZE     = FSize(pdata_r->sizePackage);
    }

  };

  /////////////////////////////////////////////////////////////////
} // namespace YUM
///////////////////////////////////////////////////////////////////

#endif // YUMPackageDataProvider_h
