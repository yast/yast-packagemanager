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
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Realized PackageDataProvider for UnitedLinux packages format

/-*/
#ifndef PMULPackageDataProvider_h
#define PMULPackageDataProvider_h

#include <iosfwd>
#include <string>
#include <fstream>

#include <y2util/Pathname.h>
#include <y2util/TagParser.h>
#include <y2util/TagCacheRetrieval.h>
#include <y2pm/PMULPackageDataProviderPtr.h>
#include <y2pm/PMPackageDataProvider.h>

#include <y2pm/PMPackage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMULPackageDataProvider
/**
 * @short Realizes PackageDataProvider for UnitedLinux packages format
 * @see DataProvider
 **/
class PMULPackageDataProvider : public PMPackageDataProvider  {
    REP_BODY(PMULPackageDataProvider);
    private:

	// save file position and size data for multi line attributes

	TagCacheRetrieval::retrieval_t attr_SUMMARY;

	TagCacheRetrieval::retrieval_t attr_RECOMMENDS;
	TagCacheRetrieval::retrieval_t attr_SUGGESTS;
	TagCacheRetrieval::retrieval_t attr_AUTHORS;
	TagCacheRetrieval::retrieval_t attr_KEYWORDS;

	TagCacheRetrieval::retrieval_t attr_DESCRIPTION;
	TagCacheRetrieval::retrieval_t attr_INSNOTIFY;
	TagCacheRetrieval::retrieval_t attr_DELNOTIFY;

	// save PkgAttributeValue for single line attributes

	PkgAttributeValue attr_LOCATION;
	PkgAttributeValue attr_ARCHIVESIZE;
	PkgAttributeValue attr_SIZE;
	PkgAttributeValue attr_BUILDTIME;
	PkgAttributeValue attr_SOURCERPM;
	PkgAttributeValue attr_GROUP;
	PkgAttributeValue attr_LICENSE;

	// retrieval pointer for packages data
	TagCacheRetrieval *_package_retrieval;

	// retrieval pointer for packages.<lang> data
	TagCacheRetrieval *_language_retrieval;

	// fallback provider (Share entry in packages)
	PMULPackageDataProviderPtr _fallback_provider;

    private:
	// internal attribute value provider
	PkgAttributeValue getValue( constPMObjectPtr obj_r,
				    PMPackage::PMPackageAttribute attr_r );

    public:

	PMULPackageDataProvider (TagCacheRetrieval *package_retrieval = 0, TagCacheRetrieval *language_retrieval = 0);
	virtual ~PMULPackageDataProvider();

	void setShared ( PMULPackageDataProviderPtr provider_r ) { _fallback_provider = provider_r; }

	/**
	 * Object attribute retrieval. (DataProvider interface)
	 * @see PMDataProvider
	 * @see PMObject
	 **/
	PkgAttributeValue getAttributeValue( constPMObjectPtr obj_r,
					     PMObject::PMObjectAttribute attr_r );

	/**
	 * Package attribute retrieval. (PackageDataProvider interface)
	 * @see PMPackageDataProvider
	 * @see PMPackage
	 **/
	PkgAttributeValue getAttributeValue( constPMPackagePtr pkg_r,
					     PMPackage::PMPackageAttribute attr_r );

	/** inject attribute to cache */
	virtual void setAttributeValue(
	    PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	    const PkgAttributeValue& value);

	/** inject offset/size to cache */
	virtual void setAttributeValue(
	    PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	    std::streampos begin, std::streampos end);

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMULPackageDataProvider_h
