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
#include <list>
#include <set>
#include <map>

#include <y2util/TaggedFile.h>
#include <y2util/TagCacheRetrieval.h>
#include <y2util/TagCacheRetrievalPtr.h>

#include <y2pm/PMULSelectionDataProviderPtr.h>
#include <y2pm/PMSelectionDataProvider.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMULSelectionDataProvider
/**
 * @short Realizes SelectionDataProvider for UnitedLinux packages format
 *
 * InstSrcDataUL uses a per Selection DataProvider. Thus it's always the same
 * Selection that calls the interface, and it's ok to store Selection related data
 * here.
 *
 * @see PMSelectionDataProvider
 **/
class PMULSelectionDataProvider : public PMSelectionDataProvider {
    REP_BODY(PMULSelectionDataProvider);

    friend class ULSelectionParser;

    private:

	// inspacks and delpacs are cached in two variants. the first
	// (and initial) is the map of <locale, retrieval position>
	// (posmaptype)
	// This map is used when the inspacks/delpacks are needed as a string list
	//
	// But when the inspacks/delpacks are needed as a PMSelectablePtr set,
	// this set is created on demand and stored in the _ptrs* attribute.
	//
	// All further requests for string or pointer set are then fulfilled
	// from the pointer set (which is converted back to a string set on demand,
	// which is faster than a file retrieval anyway)
	//

	// map of <string locale, TagRetrievalPos retrieval position>
	typedef TaggedFile::Tag::posmaptype::const_iterator posmapIT;

	// map of <locale, selectable list>
	typedef map <LangCode,std::set<PMSelectablePtr> > slcmaptype;
	typedef slcmaptype::const_iterator slcmapIT;

	// find retrieval position by locale
	posmapIT posmapFind (const TaggedFile::Tag::posmaptype& theMap, const LangCode& locale, bool with_empty) const;

	// find PMSelectable list by locale
	slcmapIT slcmapFind (const slcmaptype& theMap, const LangCode& locale, bool with_empty) const;

	// find string position by locale
	std::string posmapSLookup (const TaggedFile::Tag::posmaptype& theMap, const LangCode& locale, bool with_empty) const;

	// find string list position by locale
	std::list<std::string> posmapLLookup (const TaggedFile::Tag::posmaptype& theMap, const LangCode& locale, bool with_empty) const;

	// convert selection list to string list
	std::list<std::string> sellist2strlist (const std::list<PMSelectionPtr>& sellist) const;

	// convert selectable set to string list
	std::list<std::string> slclist2strlist (const std::set<PMSelectablePtr>& slclist) const;

	// lookup inspacks/delpacks for locale as string list
	std::list<std::string> pkgsList (const LangCode& locale, bool is_delpacks) const;

	// lookup inspacks/delpacks for locale as selectable set
	std::set<PMSelectablePtr> pkgsPointers (const LangCode & locale, bool is_delpacks) const;

    protected:

	// PMObject
	TaggedFile::Tag::posmaptype 			_attr_SUMMARY;
	TaggedFile::Tag::posmaptype 			_attr_DESCRIPTION;
	TaggedFile::Tag::posmaptype 			_attr_INSNOTIFY;
	TaggedFile::Tag::posmaptype 			_attr_DELNOTIFY;
	FSize 						_attr_SIZE;

	std::string 					_attr_CATEGORY;	// "base", ...
	bool 						_attr_ISBASE;

	bool 						_attr_VISIBLE;

	// see ULSelectionParser. The recommends and suggests fields are filled
	// during parsing to prevent cross-source pointers
	std::list<std::string>			_attr_RECOMMENDS;	// temporary
	std::list<PMSelectionPtr> 		_ptrs_attr_RECOMMENDS;	// final
	std::list<std::string>			_attr_SUGGESTS;		// temporary
	std::list<PMSelectionPtr> 		_ptrs_attr_SUGGESTS;	// final

	// map over locales
	TaggedFile::Tag::posmaptype 		_attr_INSPACKS;
	TaggedFile::Tag::posmaptype 		_attr_DELPACKS;
	mutable slcmaptype 			_ptrs_attr_INSPACKS; // on demand cache
	mutable slcmaptype			_ptrs_attr_DELPACKS; // on demand cache

	FSize 					_attr_ARCHIVESIZE;
	std::string 				_attr_ORDER;

	// retrieval pointer for *.sel data
	TagCacheRetrievalPtr _selection_retrieval;

        TagCacheRetrievalPtr getCacheRetrieval() { return _selection_retrieval; }

    public:

	PMULSelectionDataProvider (const Pathname& selectionname);
	virtual ~PMULSelectionDataProvider();

    public:

	/**
	* Selection attributes InstSrcDataUL is able to provide.
	* @see PMSelectionDataProvider
	**/

         // PMObject attributes
	virtual std::string               summary        ( const PMSelection & sel_r, const LangCode& locale = LangCode("") ) const;
	virtual std::list<std::string>    description    ( const PMSelection & sel_r, const LangCode& locale = LangCode("") ) const;
	virtual std::list<std::string>    insnotify      ( const PMSelection & sel_r, const LangCode& locale = LangCode("") ) const;
	virtual std::list<std::string>    delnotify      ( const PMSelection & sel_r, const LangCode& locale = LangCode("") ) const;
	virtual FSize                     size           ( const PMSelection & sel_r ) const;

	// PMSelection attributes
	virtual std::string               category       ( const PMSelection & sel_r ) const;
	virtual bool                      visible        ( const PMSelection & sel_r ) const;
	// selection dependencies
	virtual std::list<std::string>    suggests       ( const PMSelection & sel_r ) const;
	virtual std::list<PMSelectionPtr> suggests_ptrs  ( const PMSelection & sel_r ) const;
	virtual std::list<std::string>    recommends     ( const PMSelection & sel_r ) const;
	virtual std::list<PMSelectionPtr> recommends_ptrs( const PMSelection & sel_r ) const;
	// package dependencies
	virtual std::list<std::string>    inspacks       ( const PMSelection & sel_r, const LangCode & locale = LangCode("") ) const;
	virtual std::list<std::string>    delpacks       ( const PMSelection & sel_r, const LangCode & locale = LangCode("") ) const;

        // the per locale entry ( no default lang argument! )
	virtual PM::LocaleSet             supportedLocales( const PMSelection & sel_r ) const;
	virtual std::set<PMSelectablePtr> inspacks_ptrs  ( const PMSelection & sel_r, const LangCode & locale ) const;
	virtual std::set<PMSelectablePtr> delpacks_ptrs  ( const PMSelection & sel_r, const LangCode & locale ) const;

	virtual FSize                     archivesize    ( const PMSelection & sel_r ) const;
	virtual std::string               order          ( const PMSelection & sel_r ) const;

	virtual bool                      isBase         ( const PMSelection & sel_r ) const;

	// physical access to the sel file.
	virtual PMError provideSelToInstall( const PMSelection & sel_r, Pathname & path_r ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMULSelectionDataProvider_h
