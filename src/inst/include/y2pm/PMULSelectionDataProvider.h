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
#include <y2util/LangCode.h>
#include <y2util/Pathname.h>
#include <y2util/TaggedFile.h>
#include <y2util/TagCacheRetrieval.h>
#include <y2util/TagCacheRetrievalPtr.h>
#include <y2pm/PMULSelectionDataProviderPtr.h>
#include <y2pm/PMSelectionDataProvider.h>

#include <y2pm/PMSelection.h>
#include <y2pm/PMSelectable.h>

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

	// the list of inspacks and delpacs are cached in two variants
	// the first (and initial) is the map of <locale, retrieval position>
	// (posmaptype)
	// This map is used when the inspacks/delpacks are needed as a string list
	//
	// But when the inspacks/delpacks are needed as a PMSelectablePtr list,
	// this list is created on demand and stored in the _ptrs* attribute.
	//
	// All further requests for string or pointer list are then fulfilled
	// from the pointer list (which is converted back to a string list on demand,
	// which is faster than a file retrieval anyway)
	//

	// map of <string locale, TagRetrievalPos retrieval position>
	typedef TaggedFile::Tag::posmaptype::const_iterator posmapIT;

	// map of <locale, selectable list>
	typedef map <LangCode,std::list<PMSelectablePtr> > slcmaptype;
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

	// convert selectable list to string list
	std::list<std::string> slclist2strlist (const std::list<PMSelectablePtr>& slclist) const;

	// lookup inspacks/delpacks for locale as string list
	std::list<std::string> pkgsList (const LangCode& locale, bool is_delpacks) const;

	// lookup inspacks/delpacks for locale as selectable list
	//   this needs a non-const data provider in order to store the selectable list in this
	//   provider if it didn't exits before.
	std::list<PMSelectablePtr> pkgsPointers (PMULSelectionDataProviderPtr prv, const LangCode & locale, bool is_delpacks) const;

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
	slcmaptype 				_ptrs_attr_INSPACKS;
	TaggedFile::Tag::posmaptype 		_attr_DELPACKS;
	slcmaptype				_ptrs_attr_DELPACKS;

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
	virtual std::list<PMSelectablePtr>inspacks_ptrs  ( const PMSelection & sel_r, const LangCode & locale = LangCode("") ) const;
	virtual std::list<std::string>    delpacks       ( const PMSelection & sel_r, const LangCode & locale = LangCode("") ) const;
	virtual std::list<PMSelectablePtr>delpacks_ptrs  ( const PMSelection & sel_r, const LangCode & locale = LangCode("") ) const;
	virtual FSize                     archivesize    ( const PMSelection & sel_r ) const;
	virtual std::string               order          ( const PMSelection & sel_r ) const;

	virtual bool                      isBase         ( const PMSelection & sel_r ) const;

};

///////////////////////////////////////////////////////////////////

#endif // PMULSelectionDataProvider_h
