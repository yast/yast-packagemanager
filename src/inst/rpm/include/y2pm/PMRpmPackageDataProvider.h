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

   File:       PMRpmPackageDataProvider.h
   Purpose:    Implements PackageDataProvider for installed rpms
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#ifndef PMRpmPackageDataProvider_h
#define PMRpmPackageDataProvider_h

#include <iosfwd>
#include <map>
#include <vector>

#include <y2util/YRpmGroupsTree.h>
#include <y2util/FSize.h>
#include <y2util/Date.h>
#include <y2util/Vendor.h>

#include <y2pm/RpmDbPtr.h>
#include <y2pm/RpmHeaderPtr.h>

#include <y2pm/PMRpmPackageDataProviderPtr.h>
#include <y2pm/PMPackageDataProvider.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMObject.h>

class PMRpmPackageDataProvider : virtual public Rep, public PMPackageDataProvider
{
    REP_BODY(PMRpmPackageDataProvider);

    friend class RpmDb;

    private:
	// back pointer to RpmDb for on-demand rpm access
	RpmDbPtr _rpmdb;

	// cached values per package
	// !!! RpmDb uses a per Package DataProvider, so it's always the
	//     same PMPackage that calls the inteface. So it makes sense to
	//     store data here.
	Date              _attr_INSTALLTIME; // referenced from rpmdb during setup
	std::string       _attr_SUMMARY;
	FSize             _attr_SIZE;
	YStringTreeItem * _attr_GROUP;
	Vendor            _attr_VENDOR;


	/**
	 * Called from RpmDb to setup cached values.
	 **/
	void loadStaticData( constRpmHeaderPtr h );

	/**
	 * single package cache for _cachedPkg
	 *
	 * will be re-filled if attribute request
	 * for a package != _cachedPkg is issued
	 */
	static PMPackagePtr         _cachedPkg;
	static constRpmHeaderPtr _cachedData;

	constRpmHeaderPtr fillCache( PMPackagePtr package ) const;

  public:

	/**
	 * constructor, destructor
	 */
	PMRpmPackageDataProvider( RpmDbPtr rpmdb );

	virtual ~PMRpmPackageDataProvider();

  public:

    /**
     * Package attributes provided by RpmDb
     **/

    // PMObject attributes
    virtual std::string            summary     ( const PMPackage & pkg_r ) const;
    virtual std::list<std::string> description ( const PMPackage & pkg_r ) const;
    virtual FSize                  size        ( const PMPackage & pkg_r ) const;

    // PMPackage attributes
    virtual Date                   buildtime   ( const PMPackage & pkg_r ) const;
    virtual std::string            buildhost   ( const PMPackage & pkg_r ) const;
    virtual Date                   installtime ( const PMPackage & pkg_r ) const;
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
    virtual std::list<std::string> filenames   ( const PMPackage & pkg_r ) const;

    // dudata is special
    virtual void du( const PMPackage & pkg_r, PkgDu & dudata_r ) const;
};

#endif // PMRpmPackageDataProvider_h

// vim:sw=4
