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

  File:       PMULSelectionDataProvider.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Realized PackageDataProvider for UnitedLinux packages format

/-*/
#ifndef PMULSelectionDataProvider_h
#define PMULSelectionDataProvider_h

#include <iosfwd>
#include <string>
#include <fstream>
#include <map>

#include <y2util/FSize.h>
#include <y2util/Pathname.h>
#include <y2util/TagParser.h>
#include <y2util/TagCacheRetrieval.h>
#include <y2util/TagCacheRetrievalPtr.h>
#include <y2pm/PMULSelectionDataProviderPtr.h>
#include <y2pm/PMSelectionDataProvider.h>

#include <y2pm/PMSelection.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMULSelectionDataProvider
/**
 * @short Realizes SelectionDataProvider for UnitedLinux packages format
 * @see DataProvider
 **/
class PMULSelectionDataProvider : public PMSelectionDataProvider  {
    REP_BODY(PMULSelectionDataProvider);

    friend class InstSrcDataUL;

    protected:

	// the data belongs to this selection
	PMSelectionPtr _selection;

	// PMObject

	map <std::string,TagCacheRetrievalPos> _attr_SUMMARY;
	TagCacheRetrievalPos _attr_DESCRIPTION;
	TagCacheRetrievalPos _attr_INSNOTIFY;
	TagCacheRetrievalPos _attr_DELNOTIFY;
	FSize _attr_SIZE;

	std::string _attr_CATEGORY;	// "base", ...
	bool _attr_ISBASE;

	bool _attr_VISIBLE;
	TagCacheRetrievalPos _attr_SUGGESTS;
	std::list<PMSelectionPtr> _ptrs_attr_SUGGESTS;
	TagCacheRetrievalPos _attr_RECOMMENDS;
	std::list<PMSelectionPtr> _ptrs_attr_RECOMMENDS;
	map <std::string,TagCacheRetrievalPos> _attr_INSPACKS;
	map <std::string,std::list<PMPackagePtr> > _ptrs_attr_INSPACKS;
	map <std::string,TagCacheRetrievalPos> _attr_DELPACKS;
	map <std::string,std::list<PMPackagePtr> > _ptrs_attr_DELPACKS;
	FSize _attr_ARCHIVESIZE;
	std::string _attr_ORDER;

	// retrieval pointer for *.sel data
	TagCacheRetrievalPtr _selection_retrieval;

    public:

	PMULSelectionDataProvider (const Pathname& selectionname);
	virtual ~PMULSelectionDataProvider();

	void startRetrieval() const;
	void stopRetrieval() const;

	TagParser& getParser() { return _selection_retrieval->getParser(); }
	void setSelection ( PMSelectionPtr selection) { _selection = selection; }

	/**
	 * access functions for PMObject/PMSelection attributes
	 */

	const std::string summary() const { return summary(""); }
	const std::list<std::string> description() const { return description(""); }
	const std::list<std::string> insnotify() const { return insnotify(""); }
	const std::list<std::string> delnotify() const { return delnotify(""); }
	const FSize size() const;

	const std::string summary(const std::string& lang) const;
	const std::list<std::string> description(const std::string& lang) const;
	const std::list<std::string> insnotify(const std::string& lang) const;
	const std::list<std::string> delnotify(const std::string& lang) const;

	/**
	 * access functions for PMSelection attributes
	 */

	const std::string category () const;
	const bool visible () const;
	const std::list<std::string> suggests() const;
	const std::list<PMSelectionPtr> suggests_ptrs();
	const std::list<std::string> recommends() const;
	const std::list<PMSelectionPtr> recommends_ptrs();
	const std::list<std::string> inspacks(const std::string& lang = "") const;
	const std::list<PMPackagePtr> inspacks_ptrs (const std::string& lang = "");
	const std::list<std::string> delpacks(const std::string& lang = "") const;
	const std::list<PMPackagePtr> delpacks_ptrs (const std::string& lang = "");
	const FSize archivesize() const;
	const std::string order() const;

	/**
	 * helper functions
	 */
	const bool isBase () const;

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMULSelectionDataProvider_h
