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
#include <y2util/TagCacheRetrievalPtr.h>
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

    friend class InstSrcDataUL;
    protected:

	// the data belongs to this package
	PMPackagePtr _package;

	// PMObject

	TagCacheRetrievalPos _attr_SUMMARY;
	TagCacheRetrievalPos _attr_DESCRIPTION;
	TagCacheRetrievalPos _attr_INSNOTIFY;
	TagCacheRetrievalPos _attr_DELNOTIFY;
	FSize _attr_SIZE;

	// PMPackage
	Date _attr_BUILDTIME;
	// BUILDHOST
	// INSTALLTIME
	// DISTRIBUTION
	// VENDOR
	TagCacheRetrievalPos _attr_LICENSE;
	// PACKAGER
	TagCacheRetrievalPos _attr_GROUP;
	// CHANGELOG
	// URL
	// OS
	// PREIN
	// POSTIN
	// PREUN
	// POSTUN
	TagCacheRetrievalPos _attr_SOURCERPM;
	FSize _attr_ARCHIVESIZE;
	TagCacheRetrievalPos _attr_AUTHORS;
	// FILENAMES
	TagCacheRetrievalPos _attr_RECOMMENDS;
	TagCacheRetrievalPos _attr_SUGGESTS;
	TagCacheRetrievalPos _attr_LOCATION;
	TagCacheRetrievalPos _attr_KEYWORDS;

	// retrieval pointer for packages data
	TagCacheRetrievalPtr _package_retrieval;

	// retrieval pointer for packages.<lang> data
	TagCacheRetrievalPtr _language_retrieval;

	// fallback provider (Share entry in packages)
	PMULPackageDataProviderPtr _fallback_provider;

    public:
	void setValue( PMPackage::PMPackageAttribute attr_r, std::string& value_r );
	void setValue( PMPackage::PMPackageAttribute attr_r, std::streampos begin, std::streampos end);

    public:

	PMULPackageDataProvider (TagCacheRetrievalPtr package_retrieval);
	virtual ~PMULPackageDataProvider();

	void setPackage ( PMPackagePtr package ) { _package = package; }
	void setLangCache ( TagCacheRetrievalPtr language_retrieval ) { _language_retrieval = language_retrieval; }
	void setShared ( PMULPackageDataProviderPtr provider_r ) { _fallback_provider = provider_r; }

	void startRetrieval() const;
	void stopRetrieval() const;

	/**
	 * access functions for PMObject attributes
	 */

	const std::string summary () const;
	const std::list<std::string> description () const;
	const std::list<std::string> insnotify () const;
	const std::list<std::string> delnotify () const;
	const FSize size () const;

	/**
	 * access functions for PMPackage attributes
	 */

	const Date buildtime () const;
	const std::string buildhost () const { return PMPackageDataProvider::buildhost(); }
	const Date installtime () const { return PMPackageDataProvider::installtime(); }
	const std::string distribution () const { return PMPackageDataProvider::distribution(); }
	const std::string vendor () const { return PMPackageDataProvider::vendor(); }
	const std::string license () const;
	const std::string packager () const { return PMPackageDataProvider::packager(); }
	const std::string group () const;
	const std::list<std::string> changelog () const { return PMPackageDataProvider::changelog(); }
	const std::string url () const { return PMPackageDataProvider::url(); }
	const std::string os () const { return PMPackageDataProvider::os(); }
	const std::list<std::string> prein () const { return PMPackageDataProvider::prein(); }
	const std::list<std::string> postin () const { return PMPackageDataProvider::postin(); }
	const std::list<std::string> preun () const { return PMPackageDataProvider::preun(); }
	const std::list<std::string> postun () const { return PMPackageDataProvider::postun(); }
	const std::string sourcerpm () const;
	const FSize archivesize () const;
	const std::list<std::string> authors () const;
	const std::list<std::string> filenames () const { return PMPackageDataProvider::filenames(); }
	// suse packages values
	const std::list<std::string> recommends () const;
	const std::list<std::string> suggests () const;
	const std::string location () const;
	const std::list<std::string> keywords () const;

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMULPackageDataProvider_h
