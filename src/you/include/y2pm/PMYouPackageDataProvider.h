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

   File:       PMYouPackageDataProvider.h
   Purpose:    Implements PackageDataProvider for YOU patches.
   Author:     Cornelius Schumacher <cschum@suse.de>
   Maintainer: Cornelius Schumacher <cschum@suse.de>

/-*/

#ifndef PMYouPackageDataProvider_h
#define PMYouPackageDataProvider_h

#include <y2pm/PMYouPackageDataProviderPtr.h>
#include <y2pm/PMPackageDataProvider.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMObject.h>
#include <y2pm/PMYouPatchInfo.h>

#include <y2util/YRpmGroupsTree.h>

class PMYouPackageDataProvider : virtual public Rep, public PMPackageDataProvider {
	REP_BODY(PMYouPackageDataProvider);

        PMYouPatchInfoPtr _patchInfo;
        PMPackagePtr _pkg;

    public:

	PMYouPackageDataProvider( const PMYouPatchInfoPtr & );

	virtual ~PMYouPackageDataProvider();

        void setPackage( const PMPackagePtr &pkg ) { _pkg = pkg; }

	/**
	 * hint before accessing multiple attributes
	 */
	void startRetrieval() const { return PMDataProvider::startRetrieval(); }

	/**
	 * hint after accessing multiple attributes
	 */
	void stopRetrieval() const { return PMDataProvider::stopRetrieval(); }

	/**
	 * access functions for PMObject attributes
	 */

	const std::string summary() const;
	const std::list<std::string> description() const;
	const std::list<std::string> insnotify() const;
	const std::list<std::string> delnotify() const;
	const FSize size () const;

	/**
	 * access functions for PMPackage attributes
	 */

	const Date buildtime () const { return PMPackageDataProvider::buildtime(); }
	const std::string buildhost () const { return PMPackageDataProvider::buildhost(); }
	const Date installtime () const { return PMPackageDataProvider::installtime(); }
	const std::string distribution () const { return PMPackageDataProvider::distribution(); }
	const std::string vendor () const { return PMPackageDataProvider::vendor(); }
	const std::string license () const { return PMPackageDataProvider::license(); }
	const std::string packager () const { return PMPackageDataProvider::packager(); }
	const std::string group () const { return PMPackageDataProvider::group(); }
	const YStringTreeItem * group_ptr () const { return PMPackageDataProvider::group_ptr(); }
	const std::list<std::string> changelog () const { return PMPackageDataProvider::changelog(); }
	const std::string url () const { return PMPackageDataProvider::url(); }
	const std::string os () const { return PMPackageDataProvider::os(); }
	const std::list<std::string> prein () const { return PMPackageDataProvider::prein(); }
	const std::list<std::string> postin () const { return PMPackageDataProvider::postin(); }
	const std::list<std::string> preun () const { return PMPackageDataProvider::preun(); }
	const std::list<std::string> postun () const { return PMPackageDataProvider::postun(); }
	const std::string sourcerpm () const { return PMPackageDataProvider::sourcerpm(); }
	const FSize archivesize () const { return PMPackageDataProvider::archivesize(); }
	const std::list<std::string> authors () const { return PMPackageDataProvider::authors(); }
	const std::list<std::string> filenames () const { return PMPackageDataProvider::filenames(); }
	// suse packages values
	const std::list<std::string> recommends () const { return PMPackageDataProvider::recommends(); }
	const std::list<std::string> suggests () const { return PMPackageDataProvider::suggests(); }
	const std::string location () const;
#warning YouPackage Media Number default used
	const int medianr () const { return PMPackageDataProvider::medianr(); }
	const std::list<std::string> keywords () const { return PMPackageDataProvider::keywords(); }

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

#endif // PMYouPackageDataProvider_h
