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

  File:       PMPackageDataProvider.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Common interface to be realized by all PackageDataProvider.

/-*/
#ifndef PMPackageDataProvider_h
#define PMPackageDataProvider_h

#include <iosfwd>
#include <string>

#include <y2util/Date.h>
#include <y2util/FSize.h>
#include <y2util/Vendor.h>
#include <y2util/Pathname.h>
#include <y2util/YRpmGroupsTree.h>

#include <y2pm/PkgSplit.h>

#include <y2pm/PMError.h>
#include <y2pm/PMPackageDataProviderPtr.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/InstSrcPtr.h>

class PkgDu;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProvider
/**
 * @short Common interface to be realized by all PackageDataProvider.
 *
 * PMPackage attribute retrieval. The PMPackage this pointer is passed
 * in case the concrete DataProvider has do some lookup.
 *
 * Default values provided by static members
 *
 * @see DataProvider
 **/
class PMPackageDataProvider : public CountedRep {
  REP_BODY(PMPackageDataProvider);

  public:

    ///////////////////////////////////////////////////////////////////
    // default for unprovided values
    ///////////////////////////////////////////////////////////////////

    // PMObject attributes
    static std::string            summary()     { return std::string(); }
    static std::list<std::string> description() { return std::list<std::string>(); }
    static std::list<std::string> insnotify()   { return std::list<std::string>(); }
    static std::list<std::string> delnotify()   { return std::list<std::string>(); }
    static FSize                  size()        { return FSize(0); }
    static bool                   providesSources() { return false; }
    static std::string            instSrcLabel()    { return std::string(); }
    static Vendor                 instSrcVendor()   { return Vendor(); }
    static unsigned               instSrcRank()     { return unsigned(-1); }

    // PMPackage attributes
    static PkgSplitSet            splitprovides() { return PkgSplitSet(); }
    static Date                   buildtime()   { return Date(0); }
    static std::string            buildhost()   { return std::string(); }
    static Date                   installtime() { return Date(0); }
    static std::string            distribution(){ return std::string(); }
    static Vendor                 vendor()      { return Vendor(); }
    static std::string            license()     { return std::string(); }
    static std::list<std::string> licenseToConfirm() { return std::list<std::string>(); }
    static std::string            packager()    { return std::string(); }
    static std::string            group()       { return std::string(); }
    static YStringTreeItem *      group_ptr()   { return 0; }
    static std::list<std::string> changelog()   { return std::list<std::string>(); }
    static std::string            url()         { return std::string(); }
    static std::string            os()          { return std::string(); }
    static std::list<std::string> prein()       { return std::list<std::string>(); }
    static std::list<std::string> postin()      { return std::list<std::string>(); }
    static std::list<std::string> preun()       { return std::list<std::string>(); }
    static std::list<std::string> postun()      { return std::list<std::string>(); }
    static std::string            sourceloc()   { return std::string(); }
    static FSize	          sourcesize()  { return FSize(0); }
    static FSize                  archivesize() { return FSize(0); }
    static std::list<std::string> authors()     { return std::list<std::string>(); }
    static std::list<std::string> filenames()   { return std::list<std::string>(); }
    // suse packages values
    static std::list<std::string> recommends()  { return std::list<std::string>(); }
    static std::list<std::string> suggests()    { return std::list<std::string>(); }
    static std::string            location()    { return std::string(); }
    static unsigned int           medianr()     { return 0; }
    static std::list<std::string> keywords()    { return std::list<std::string>(); }
    static std::string            externalUrl() { return std::string(); }
    static std::list<PkgEdition>  patchRpmBaseVersions() { return std::list<PkgEdition>(); }
    static FSize                  patchRpmSize() { return FSize( 0 ); }
    static bool                   forceInstall() { return false; }

    static bool isRemote()			{ return false; }
    // physical access to the rpm file.
    static PMError providePkgToInstall(Pathname& path_r) { path_r = Pathname(); return PMError::E_error; }
    // physical access to the src.rpm file.
    static PMError provideSrcPkgToInstall(Pathname& path_r) { path_r = Pathname(); return PMError::E_error; }
    // source for this package, needed for callbacks
    static constInstSrcPtr source()		{ return constInstSrcPtr(0); }

    // Internally used by PMSelectable.
    static bool prefererCandidate() { return false; }

    // dudata is special
    static void du( PkgDu & dudata_r );

  protected:

    PMPackageDataProvider();

    virtual ~PMPackageDataProvider();

    /**
     * In case concrete PackageDataProvider wants PMPackagePtr for lookup.
     **/
    PMPackagePtr mkPtr( const PMPackage & pkg_r ) const { return const_cast<PMPackage*>(&pkg_r); }

  public:

    ///////////////////////////////////////////////////////////////////
    // Overload values you can provide
    ///////////////////////////////////////////////////////////////////

    // PMObject attributes
    virtual std::string            summary     ( const PMPackage & pkg_r ) const { return summary(); }
    virtual std::list<std::string> description ( const PMPackage & pkg_r ) const { return description(); }
    virtual std::list<std::string> insnotify   ( const PMPackage & pkg_r ) const { return insnotify(); }
    virtual std::list<std::string> delnotify   ( const PMPackage & pkg_r ) const { return delnotify(); }
    virtual FSize                  size        ( const PMPackage & pkg_r ) const { return size(); }
    virtual bool                   providesSources( const PMPackage & pkg_r ) const { return providesSources(); }
    virtual std::string            instSrcLabel   ( const PMPackage & pkg_r ) const { return instSrcLabel(); }
    virtual Vendor                 instSrcVendor  ( const PMPackage & pkg_r ) const { return instSrcVendor(); }
    virtual unsigned               instSrcRank    ( const PMPackage & pkg_r ) const { return instSrcRank(); }

    // PMPackage attributes
    virtual PkgSplitSet            splitprovides( const PMPackage & pkg_r ) const { return splitprovides(); }
    virtual Date                   buildtime   ( const PMPackage & pkg_r ) const { return buildtime(); }
    virtual std::string            buildhost   ( const PMPackage & pkg_r ) const { return buildhost(); }
    virtual Date                   installtime ( const PMPackage & pkg_r ) const { return installtime(); }
    virtual std::string            distribution( const PMPackage & pkg_r ) const { return distribution(); }
    virtual Vendor                 vendor      ( const PMPackage & pkg_r ) const { return vendor(); }
    virtual std::string            license     ( const PMPackage & pkg_r ) const { return license(); }
    virtual std::list<std::string> licenseToConfirm( const PMPackage & pkg_r ) const { return licenseToConfirm(); }
    virtual std::string            packager    ( const PMPackage & pkg_r ) const { return packager(); }
    virtual std::string            group       ( const PMPackage & pkg_r ) const { return group(); }
    virtual YStringTreeItem *      group_ptr   ( const PMPackage & pkg_r ) const { return group_ptr(); }
    virtual std::list<std::string> changelog   ( const PMPackage & pkg_r ) const { return changelog(); }
    virtual std::string            url         ( const PMPackage & pkg_r ) const { return url(); }
    virtual std::string            os          ( const PMPackage & pkg_r ) const { return os(); }
    virtual std::list<std::string> prein       ( const PMPackage & pkg_r ) const { return prein(); }
    virtual std::list<std::string> postin      ( const PMPackage & pkg_r ) const { return postin(); }
    virtual std::list<std::string> preun       ( const PMPackage & pkg_r ) const { return preun(); }
    virtual std::list<std::string> postun      ( const PMPackage & pkg_r ) const { return postun(); }
    virtual std::string            sourceloc   ( const PMPackage & pkg_r ) const { return sourceloc(); }
    virtual FSize                  sourcesize  ( const PMPackage & pkg_r ) const { return sourcesize(); }
    virtual FSize                  archivesize ( const PMPackage & pkg_r ) const { return archivesize(); }
    virtual std::list<std::string> authors     ( const PMPackage & pkg_r ) const { return authors(); }
    virtual std::list<std::string> filenames   ( const PMPackage & pkg_r ) const { return filenames(); }
    // suse packages values
    virtual std::list<std::string> recommends  ( const PMPackage & pkg_r ) const { return recommends(); }
    virtual std::list<std::string> suggests    ( const PMPackage & pkg_r ) const { return suggests(); }
    virtual std::string            location    ( const PMPackage & pkg_r ) const { return location(); }
    virtual unsigned int           medianr     ( const PMPackage & pkg_r ) const { return medianr(); }
    virtual std::list<std::string> keywords    ( const PMPackage & pkg_r ) const { return keywords(); }
    virtual std::string            externalUrl ( const PMPackage & pkg_r ) const { return externalUrl(); }
    virtual std::list<PkgEdition>  patchRpmBaseVersions( const PMPackage & pkg_r ) const { return patchRpmBaseVersions(); }
    virtual FSize                  patchRpmSize( const PMPackage & pkg_r ) const { return patchRpmSize(); }
    virtual bool                   forceInstall( const PMPackage & pkg_r ) const { return forceInstall(); }

    virtual bool		   isRemote    ( const PMPackage & pkg_r ) const { return isRemote(); }
    // physical access to the rpm file.
    virtual PMError providePkgToInstall( const PMPackage & pkg_r, Pathname& path_r ) const { return providePkgToInstall(path_r); }
    // physical access to the src.rpm file.
    virtual PMError provideSrcPkgToInstall( const PMPackage & pkg_r, Pathname& path_r ) const { return provideSrcPkgToInstall(path_r); }
    // source for this package, needed for callbacks
    virtual constInstSrcPtr source( const PMPackage & pkg_r ) const { return source(); }

    // Internally used by PMSelectable.
    virtual bool prefererCandidate( const PMPackage & pkg_r ) const { return prefererCandidate(); }

    // dudata is special
    virtual void du( const PMPackage & pkg_r, PkgDu & dudata_r ) const { return du( dudata_r ); }
};

///////////////////////////////////////////////////////////////////

#endif // PMPackageDataProvider_h
