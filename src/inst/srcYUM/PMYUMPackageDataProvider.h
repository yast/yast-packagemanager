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

   File:       PMYUMPackageDataProvider.h
   Purpose:    Implements PackageDataProvider for installed rpms
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#ifndef PMYUMPackageDataProvider_h
#define PMYUMPackageDataProvider_h

#include <iosfwd>
#include <map>
#include <vector>

#include <y2util/YRpmGroupsTree.h>
#include <y2util/FSize.h>
#include <y2util/Date.h>
#include <y2util/Vendor.h>

#include <y2pm/InstSrcDataYUMPtr.h>

#include "PMYUMPackageDataProviderPtr.h"
#include <y2pm/PMPackageDataProvider.h>
#include <y2pm/PMPackage.h>

#include "y2pm/InstSrcDataYUM.h"
#include "YUMImpl.h"

class PMYUMPackageDataProvider : public PMPackageDataProvider
{
  REP_BODY(PMYUMPackageDataProvider);

  friend class InstSrcDataYUM;
  friend class YUM::Impl;

  public:

    /**
     * constructor, destructor
    */
    PMYUMPackageDataProvider( constInstSrcDataYUMPtr instSrcData_r );

    virtual ~PMYUMPackageDataProvider();

  public:

    // PMObject attributes
    virtual std::string            summary     ( const PMPackage & pkg_r ) const
    { return _attr_SUMMARY; }
    virtual std::list<std::string> description ( const PMPackage & pkg_r ) const
    { return _attr_DESCRIPTION; }
    /**
     * Package attributes provided by RpmDb
     **/
#if 0
    // PMObject attributes
    virtual std::string            summary     ( const PMPackage & pkg_r ) const;
    virtual std::list<std::string> description ( const PMPackage & pkg_r ) const;
    virtual FSize                  size        ( const PMPackage & pkg_r ) const;
    virtual bool                   providesSources( const PMPackage & pkg_r ) const;
    virtual std::string            instSrcLabel   ( const PMPackage & pkg_r ) const;
    virtual Vendor                 instSrcVendor  ( const PMPackage & pkg_r ) const;
    virtual unsigned               instSrcRank    ( const PMPackage & pkg_r ) const;

    // PMPackage attributes
    virtual PkgSplitSet            splitprovides( const PMPackage & pkg_r ) const;
    virtual Date                   buildtime   ( const PMPackage & pkg_r ) const;
    virtual std::string            buildhost   ( const PMPackage & pkg_r ) const;
    virtual std::string            distribution( const PMPackage & pkg_r ) const;
    virtual Vendor                 vendor      ( const PMPackage & pkg_r ) const;
    virtual std::string            license     ( const PMPackage & pkg_r ) const;
    virtual std::string            packager    ( const PMPackage & pkg_r ) const;
    virtual std::string            group       ( const PMPackage & pkg_r ) const;
    virtual YStringTreeItem *      group_ptr   ( const PMPackage & pkg_r ) const;
    virtual std::list<std::string> changelog   ( const PMPackage & pkg_r ) const;
    virtual std::string            url         ( const PMPackage & pkg_r ) const;
    virtual std::string            os          ( const PMPackage & pkg_r ) const;
    virtual std::list<std::string> prein       ( const PMPackage & pkg_r ) const;
    virtual std::list<std::string> postin      ( const PMPackage & pkg_r ) const;
    virtual std::list<std::string> preun       ( const PMPackage & pkg_r ) const;
    virtual std::list<std::string> postun      ( const PMPackage & pkg_r ) const;
    virtual std::string            sourceloc   ( const PMPackage & pkg_r ) const;
    virtual FSize                  sourcesize  ( const PMPackage & pkg_r ) const;
    virtual FSize                  archivesize ( const PMPackage & pkg_r ) const;
    //virtual std::list<std::string> authors     ( const PMPackage & pkg_r ) const;
    virtual std::list<std::string> filenames   ( const PMPackage & pkg_r ) const;

    // suse packages values
    //virtual std::list<std::string> recommends  ( const PMPackage & pkg_r ) const;
    //virtual std::list<std::string> suggests    ( const PMPackage & pkg_r ) const;
    virtual std::string            location    ( const PMPackage & pkg_r ) const;
    virtual unsigned int           medianr     ( const PMPackage & pkg_r ) const;
    //virtual std::list<std::string> keywords    ( const PMPackage & pkg_r ) const;
    //virtual std::string            externalUrl ( const PMPackage & pkg_r ) const;
    //virtual std::list<PkgEdition>  patchRpmBaseVersions( const PMPackage & pkg_r ) const;
    //virtual FSize                  patchRpmSize( const PMPackage & pkg_r ) const;

    // physical access to the rpm file.
    virtual PMError providePkgToInstall( const PMPackage & pkg_r, Pathname& path_r ) const;
    // physical access to the src.rpm file.
    virtual PMError provideSrcPkgToInstall( const PMPackage & pkg_r, Pathname& path_r ) const;

    // source for this package, needed for callbacks
    virtual constInstSrcPtr source   ( const PMPackage & pkg_r ) const;
    virtual bool	    isRemote ( const PMPackage & pkg_r ) const;

    // dudata is special
    virtual void du( const PMPackage & pkg_r, PkgDu & dudata_r ) const;
#endif

  private:

    // the source
    constInstSrcDataYUMPtr _instSrcData;

    PkgSplitSet	_attr_SPLITPROVIDES;

    std::string _attr_SUMMARY;

    std::list<std::string> _attr_DESCRIPTION;
};

#endif // PMYUMPackageDataProvider_h

// vim:sw=4
