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
#include <y2pm/RpmDbPtr.h>
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

	// package this provider belongs to
	PMPackagePtr _package;

	// cached values per package
	std::string _attr_SUMMARY;
	FSize _attr_SIZE;
	YStringTreeItem *_attr_GROUP;

    public:

	/**
	 * constructor, destructor
	 */
	PMRpmPackageDataProvider (RpmDbPtr rpmdb);
	virtual ~PMRpmPackageDataProvider();

	/**
	 * backlink to package
	 */
	void setPackage (PMPackagePtr package) { _package = package; }

	/**
	 * hint before accessing multiple attributes
	 */
	void startRetrieval() const;

	/**
	 * hint after accessing multiple attributes
	 */
	void stopRetrieval() const;

	/**
	 * access functions for PMObject attributes
	 */

	const std::string summary () const;
	const std::list<std::string> description () const;
	const std::list<std::string> insnotify () const { return PMPackageDataProvider::insnotify(); }
	const std::list<std::string> delnotify () const { return PMPackageDataProvider::delnotify(); }
	const FSize size () const;

	/**
	 * access functions for PMPackage attributes
	 */

	const Date buildtime () const;
	const std::string buildhost () const;
	const Date installtime () const;
	const std::string distribution () const;
	const std::string vendor () const;
	const std::string license () const;
	const std::string packager () const;
	const std::string group () const;
	const YStringTreeItem *group_ptr () const;
	const std::list<std::string> changelog () const;
	const std::string url () const;
	const std::string os () const;
	const std::list<std::string> prein () const;
	const std::list<std::string> postin () const;
	const std::list<std::string> preun () const;
	const std::list<std::string> postun () const;
	const std::string sourcerpm () const;
	const FSize archivesize () const { return PMPackageDataProvider::archivesize(); }
	const std::list<std::string> authors () const { return PMPackageDataProvider::authors(); }
	const std::list<std::string> filenames () const;
	// suse packages values
	const std::list<std::string> recommends () const { return PMPackageDataProvider::recommends(); }
	const std::list<std::string> suggests () const { return PMPackageDataProvider::suggests(); }
	const std::string location () const { return PMPackageDataProvider::location(); }
	const int medianr () const { return 0; }
	const std::list<std::string> keywords () const { return PMPackageDataProvider::keywords(); }

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

#endif // PMRpmPackageDataProvider_h

// vim:sw=4
