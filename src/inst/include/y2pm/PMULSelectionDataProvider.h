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
#include <y2util/TaggedFile.h>
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
 *
 * InstSrcDataUL uses a per Selection DataProvider. Thus it's always the same
 * Selection that calls the interface, and it's ok to store Selection related data
 * here.
 *
 * @see PMSelectionDataProvider
 **/
class PMULSelectionDataProvider : public PMSelectionDataProvider {
    REP_BODY(PMULSelectionDataProvider);

    friend class InstSrcDataUL;

    private:

	std::string posmapSLookup (TaggedFile::Tag::posmaptype theMap, const std::string& locale) const;
	std::list<std::string> posmapLLookup (TaggedFile::Tag::posmaptype theMap, const std::string& locale) const;

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
	TagRetrievalPos 				_attr_RECOMMENDS;
	std::list<PMSelectionPtr> 			_ptrs_attr_RECOMMENDS;
	TagRetrievalPos 				_attr_SUGGESTS;
	std::list<PMSelectionPtr> 			_ptrs_attr_SUGGESTS;

	// map over locales
	TaggedFile::Tag::posmaptype 			_attr_INSPACKS;
	map <std::string,std::list<PMPackagePtr> > 	_ptrs_attr_INSPACKS;
	TaggedFile::Tag::posmaptype 			_attr_DELPACKS;
	map <std::string,std::list<PMPackagePtr> > 	_ptrs_attr_DELPACKS;

	FSize 						_attr_ARCHIVESIZE;
	std::string 					_attr_ORDER;

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
         virtual std::string               summary        ( const PMSelection & sel_r, const std::string & lang = "" ) const;
	 virtual std::list<std::string>    description    ( const PMSelection & sel_r, const std::string & lang = "" ) const;
	 virtual std::list<std::string>    insnotify      ( const PMSelection & sel_r, const std::string & lang = "" ) const;
	 virtual std::list<std::string>    delnotify      ( const PMSelection & sel_r, const std::string & lang = "" ) const;
	 virtual FSize                     size           ( const PMSelection & sel_r ) const;

	 // PMSelection attributes
	 virtual std::string               category       ( const PMSelection & sel_r ) const;
	 virtual bool                      visible        ( const PMSelection & sel_r ) const;
	 virtual std::list<std::string>    suggests       ( const PMSelection & sel_r ) const;
	 virtual std::list<PMSelectionPtr> suggests_ptrs  ( const PMSelection & sel_r ) const;
	 virtual std::list<std::string>    recommends     ( const PMSelection & sel_r ) const;
	 virtual std::list<PMSelectionPtr> recommends_ptrs( const PMSelection & sel_r ) const;
	 virtual std::list<std::string>    inspacks       ( const PMSelection & sel_r, const std::string & lang = "" ) const;
	 virtual std::list<PMPackagePtr>   inspacks_ptrs  ( const PMSelection & sel_r, const std::string & lang = "" ) const;
	 virtual std::list<std::string>    delpacks       ( const PMSelection & sel_r, const std::string & lang = "" ) const;
	 virtual std::list<PMPackagePtr>   delpacks_ptrs  ( const PMSelection & sel_r, const std::string & lang = "" ) const;
	 virtual FSize                     archivesize    ( const PMSelection & sel_r ) const;
	 virtual std::string               order          ( const PMSelection & sel_r ) const;

	 virtual bool                      isBase         ( const PMSelection & sel_r ) const;

};

///////////////////////////////////////////////////////////////////

#endif // PMULSelectionDataProvider_h
