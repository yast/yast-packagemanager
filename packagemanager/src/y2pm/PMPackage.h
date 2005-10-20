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

    File:       PMPackage.h

    Author:     Michael Andres <ma@suse.de>
    Maintainer: Michael Andres <ma@suse.de>

    Purpose: Defines the Package object.

/-*/
#ifndef PMPackage_h
#define PMPackage_h

#include <iosfwd>
#include <string>

#include <y2pm/InstSrcPtr.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMPackageDataProvider.h> // should introduce all attribute data types.

#include <y2pm/PMObject.h>

#include <y2pm/PMPackageDelta.h>

class PkgDuMaster;
class PkgDuSlave;
class YStringTree;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackage
/**
 * The Package.
 **/
class PMPackage : public PMObject {
  REP_BODY(PMPackage);

  private:

    PkgDuSlave & _pkgdu;

  public:

    bool du_add( PkgDuMaster & master_r ) const;
    bool du_sub( PkgDuMaster & master_r ) const;

  private:

    /**
     * Internally used by PMSelectable. InstSrc may overwrite
     * candidate selection rules under certain conditions.
     **/
    virtual bool prefererCandidate() const;

  public:

    /**
     * PMObject attributes that should be realized by each concrete Object.
     * @see PMObject
     **/
    virtual std::string            summary()         const;
    virtual std::list<std::string> description()     const;
    virtual std::list<std::string> insnotify()       const;
    virtual std::list<std::string> delnotify()       const;
    virtual FSize                  size()            const;
    virtual bool                   providesSources() const;

    virtual std::string            instSrcLabel()    const;
    virtual Vendor                 instSrcVendor()   const;
    virtual unsigned               instSrcRank()     const;

    /**
     * PMPackage attributes passed off to PMPackageDataProvider
     * @see PMPackageDataProvider
     **/
    // splitprovides
    PkgSplitSet            splitprovides() const;

    Date                   buildtime()    const;
    std::string            buildhost()    const;
    Date                   installtime()  const;
    std::string            distribution() const;
    Vendor                 vendor()       const;
    std::string            license()      const;
    std::list<std::string> licenseToConfirm() const;
    std::string            packager()     const;
    std::string            group()        const;
    YStringTreeItem *      group_ptr()    const;
    std::list<std::string> changelog()    const;
    std::string            url()          const;
    std::string            os()           const;
    std::list<std::string> prein()        const;
    std::list<std::string> postin()       const;
    std::list<std::string> preun()        const;
    std::list<std::string> postun()       const;
    std::string            sourceloc()    const;
    FSize		   sourcesize()   const;
    FSize                  archivesize()  const;
    std::list<std::string> authors()      const;
    std::list<std::string> filenames()    const;
    // suse packages values
    std::list<std::string> recommends()   const;
    std::list<std::string> suggests()     const;
    std::string            location()     const;
    unsigned int           medianr()      const;
    std::list<std::string> keywords()     const;
    std::string            md5sum()       const;

    // from packages.DU (argument returned for convenience)
    PkgDu & du( PkgDu & dudata_r )        const;

    // YOU attributes
    std::string            externalUrl()  const;
    std::list<PkgEdition>  patchRpmBaseVersions() const;
    FSize                  patchRpmSize() const;
    bool                   forceInstall() const;
    std::string            patchRpmMD5()  const;

    std::list<PMPackageDelta> deltas() const;

    // if the rpm file comes from remote
    // (-> package provide takes some time, use 'provide' callback
    bool isRemote (void) const;

    // physical access to the rpm file.
    PMError providePkgToInstall( Pathname & ) const;
    // physical access to the src.rpm file.
    PMError provideSrcPkgToInstall( Pathname & ) const;
    // who's providing this package ?
    constInstSrcPtr source() const;

  protected:

    /**
     * There's no public acctess to _dataProvider!
     * Just the controlling InstSrcData may access it.
     **/
    friend class InstSrcData;

    PMPackageDataProviderPtr _dataProvider;

  public:

    PMPackage ( const PkgName &    name_r,
		const PkgEdition & edition_r,
		const PkgArch &    arch_r,
		PMPackageDataProviderPtr dataProvider_r );

    virtual ~PMPackage();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMPackage_h
