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

  File:       PMULPackageDataProvider.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Klaus Kaempf <kkaempf@suse.de>

  Purpose: Realized PackageDataProvider for UnitedLinux packages format

/-*/
#ifndef PMULPackageDataProvider_h
#define PMULPackageDataProvider_h

#include <iosfwd>
#include <string>
#include <fstream>

#include <y2util/Pathname.h>
#include <y2util/YRpmGroupsTree.h>
#include <y2util/TagCacheRetrieval.h>
#include <y2util/TagCacheRetrievalPtr.h>
#include <y2pm/PMULPackageDataProviderPtr.h>
#include <y2pm/PMPackageDataProvider.h>

#include <y2pm/PMPackage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMULPackageDataProvider
/**
 * @short Realizes PackageDataProvider for UnitedLinux packages format
 *
 * InstSrcDataUL uses a per Package DataProvider. Thus it's always the same
 * Package that calls the interface, and it's ok to store Package related data
 * here.
 *
 * @see PMPackageDataProvider
 **/
class PMULPackageDataProvider : public PMPackageDataProvider  {
    REP_BODY(PMULPackageDataProvider);

    friend class ULPackagesParser;

    protected:

	// PMObject
	TagRetrievalPos		_attr_SUMMARY;
	TagRetrievalPos		_attr_DESCRIPTION;
	TagRetrievalPos		_attr_INSNOTIFY;
	TagRetrievalPos		_attr_DELNOTIFY;
	FSize			_attr_SIZE;

	// PMPackage
	std::list<std::string>	_attr_SPLITPROVIDES;
	Date			_attr_BUILDTIME;
	// BUILDHOST
	// INSTALLTIME
	// DISTRIBUTION
	// VENDOR
	TagRetrievalPos		_attr_LICENSE;
	// PACKAGER
	YStringTreeItem *	_attr_GROUP;
	// CHANGELOG
	// URL
	// OS
	// PREIN
	// POSTIN
	// PREUN
	// POSTUN
	TagRetrievalPos		_attr_SOURCERPM;
	FSize			_attr_ARCHIVESIZE;
	TagRetrievalPos		_attr_AUTHORS;
	// FILENAMES
	TagRetrievalPos		_attr_RECOMMENDS;
	TagRetrievalPos		_attr_SUGGESTS;
	TagRetrievalPos		_attr_LOCATION;
	unsigned int		_attr_MEDIANR;
	TagRetrievalPos		_attr_KEYWORDS;
	TagRetrievalPos		_attr_DU;

	// the source
	const InstSrcPtr _source;

	// retrieval pointer for packages data
	const TagCacheRetrievalPtr _package_retrieval;

	// retrieval pointer for packages.<locale> data
	const TagCacheRetrievalPtr _locale_retrieval;

	// retrieval pointer for packages.DU data
	const TagCacheRetrievalPtr _du_retrieval;

	// fallback provider (Share entry in packages)
	PMULPackageDataProviderPtr _fallback_provider;

    public:

	PMULPackageDataProvider (InstSrcPtr source,
				 TagCacheRetrievalPtr package_retrieval,
				 TagCacheRetrievalPtr locale_retrieval,
				 TagCacheRetrievalPtr du_retrieval);
	virtual ~PMULPackageDataProvider();

    public:

	void setShared ( PMULPackageDataProviderPtr provider ) { _fallback_provider = provider; }

    public:

	/**
	 * Package attributes InstSrcDataUL is able to provide.
	 * @see PMPackageDataProvider
	 **/

	// PMObject attributes
	virtual std::string            summary     ( const PMPackage & pkg_r ) const;
	virtual std::list<std::string> description ( const PMPackage & pkg_r ) const;
	virtual std::list<std::string> insnotify   ( const PMPackage & pkg_r ) const;
	virtual std::list<std::string> delnotify   ( const PMPackage & pkg_r ) const;
	virtual FSize                  size        ( const PMPackage & pkg_r ) const;

	// PMPackage attributes
	virtual std::list<std::string> splitprovides ( const PMPackage & pkg_r ) const;
	virtual Date                   buildtime     ( const PMPackage & pkg_r ) const;
	//virtual std::string            buildhost   ( const PMPackage & pkg_r ) const;
	//virtual Date                   installtime ( const PMPackage & pkg_r ) const;
	//virtual std::string            distribution( const PMPackage & pkg_r ) const;
	//virtual std::string            vendor      ( const PMPackage & pkg_r ) const;
	virtual std::string            license     ( const PMPackage & pkg_r ) const;
	//virtual std::string            packager    ( const PMPackage & pkg_r ) const;
	virtual std::string            group       ( const PMPackage & pkg_r ) const;
	virtual YStringTreeItem *      group_ptr   ( const PMPackage & pkg_r ) const;
	//virtual std::list<std::string> changelog   ( const PMPackage & pkg_r ) const;
	//virtual std::string            url         ( const PMPackage & pkg_r ) const;
	//virtual std::string            os          ( const PMPackage & pkg_r ) const;
	//virtual std::list<std::string> prein       ( const PMPackage & pkg_r ) const;
	//virtual std::list<std::string> postin      ( const PMPackage & pkg_r ) const;
	//virtual std::list<std::string> preun       ( const PMPackage & pkg_r ) const;
	//virtual std::list<std::string> postun      ( const PMPackage & pkg_r ) const;
	virtual std::string            sourcerpm   ( const PMPackage & pkg_r ) const;
	virtual FSize                  archivesize ( const PMPackage & pkg_r ) const;
	virtual std::list<std::string> authors     ( const PMPackage & pkg_r ) const;
	//virtual std::list<std::string> filenames   ( const PMPackage & pkg_r ) const;
	// suse packages values
	virtual std::list<std::string> recommends  ( const PMPackage & pkg_r ) const;
	virtual std::list<std::string> suggests    ( const PMPackage & pkg_r ) const;
	virtual std::string            location    ( const PMPackage & pkg_r ) const;
	virtual unsigned int           medianr     ( const PMPackage & pkg_r ) const;
	virtual std::list<std::string> keywords    ( const PMPackage & pkg_r ) const;
	virtual std::list<std::string> du	   ( const PMPackage & pkg_r ) const;

	virtual bool			isRemote   ( const PMPackage & pkg_r ) const;
	// physical access to the rpm file.
	virtual PMError providePkgToInstall( const PMPackage & pkg_r, Pathname& path_r ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMULPackageDataProvider_h
