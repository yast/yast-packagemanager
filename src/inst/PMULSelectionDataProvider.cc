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

  File:       PMULSelectionDataProvider.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Realize SelectionDataProvider for UnitedLinux packages format

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>

#include <y2pm/PMULSelectionDataProvider.h>
#include <y2pm/InstData.h>
#include <y2pm/PMSelection.h>
#include <y2pm/PMSelectable.h>
#include <y2pm/PMPackageManager.h>

using namespace std;

typedef map <std::string,std::list<PMPackagePtr> >::const_iterator pkgsmapIT;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMULSelectionDataProviderPtr
//	CLASS NAME : constPMULSelectionDataProviderPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(PMULSelectionDataProvider,PMSelectionDataProvider,PMSelectionDataProvider);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULSelectionDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION : store selection Pathname
//		      for later value retrieval on-demand
//
PMULSelectionDataProvider::PMULSelectionDataProvider(const Pathname &selectionname)
{
    _selection_retrieval = TagCacheRetrievalPtr (new TagCacheRetrieval (selectionname));
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : ~PMULSelectionDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMULSelectionDataProvider::~PMULSelectionDataProvider()
{
}

///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : posmapFind
//	METHOD TYPE : posmapIT
//
//	DESCRIPTION : lookup locale in locale retrieval map
//
PMULSelectionDataProvider::posmapIT
PMULSelectionDataProvider::posmapFind (const TaggedFile::Tag::posmaptype& theMap, const LangCode& locale, bool with_empty) const
{
    posmapIT it = theMap.find (locale.asString());	// try full locale
    if (it == theMap.end())
    {
	it = theMap.find (locale.languageOnly().asString());	// try 2-char locale
	if (it == theMap.end())
	{
	    it = theMap.find ("default");	// try "default" locale
	}
	if (it == theMap.end() && with_empty)
	{
	    it = theMap.find ("");		// try empty locale
	}
    }
    return it;
}

///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : slcmapFind
//	METHOD TYPE : slcmapIT
//
//	DESCRIPTION : lookup locale in locale selection list map
//
PMULSelectionDataProvider::slcmapIT
PMULSelectionDataProvider::slcmapFind (const slcmaptype& theMap, const LangCode& locale, bool with_empty) const
{
    slcmapIT it = theMap.find (locale);
    if (it == theMap.end())
    {
	it = theMap.find (locale.languageOnly());
	if (it == theMap.end())
	{
	    it = theMap.find (LangCode ("default"));	// try "default" locale
	}
	if (it == theMap.end() && with_empty)
	{
	    it = theMap.find (LangCode (""));		// try empty locale
	}
    }
    return it;
}

///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : posmapSLookup
//	METHOD TYPE : std::string
//
//	DESCRIPTION : lookup string in locale retrieval map
//
std::string
PMULSelectionDataProvider::posmapSLookup (const TaggedFile::Tag::posmaptype& theMap, const LangCode& locale, bool with_empty) const
{
    std::string value;
    posmapIT it = posmapFind (theMap, locale, with_empty);
    if (it != theMap.end())
    {
	_selection_retrieval->retrieveData (it->second, value);
    }
    return value;
}

///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : posmasLLookup
//	METHOD TYPE : std::list<std::string>
//
//	DESCRIPTION : lookup string list in locale retrieval map
//
std::list<std::string>
PMULSelectionDataProvider::posmapLLookup (const TaggedFile::Tag::posmaptype& theMap, const LangCode& locale, bool with_empty) const
{
    std::list<std::string> value;
    posmapIT it = posmapFind (theMap, locale, with_empty);
    if (it != theMap.end())
    {
	_selection_retrieval->retrieveData (it->second, value);
    }
    return value;
}


///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : sellist2strlist
//	METHOD TYPE : std::list<std::string>
//
//	DESCRIPTION : convert list of selection pointers to list of
//		selection names
//
std::list<std::string>
PMULSelectionDataProvider::sellist2strlist (const std::list<PMSelectionPtr>& sellist) const
{
    std::list<std::string> strlist;
    for (std::list<PMSelectionPtr>::const_iterator it = sellist.begin(); it != sellist.end(); ++it)
    {
	strlist.push_back ((const std::string &)((*it)->name()));
    }
    return strlist;
}


///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : slclist2strlist
//	METHOD TYPE : std::list<std::string>
//
//	DESCRIPTION : convert set of selectable pointers to list of
//		selection names
//
std::list<std::string>
PMULSelectionDataProvider::slclist2strlist (const std::set<PMSelectablePtr>& slclist) const
{
    std::list<std::string> strlist;
    for (std::set<PMSelectablePtr>::const_iterator it = slclist.begin(); it != slclist.end(); ++it)
    {
	strlist.push_back ((*it)->name().asString());
    }
    return strlist;
}


///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : pkgsList
//	METHOD TYPE : std::list<std::string>
//
//	DESCRIPTION : lookup INSPACKS or DELPACKS string list
//		if corresponding pointer set is non-empty,
//		create string list on-demand from pointer set
//		else retrieve string list
//
std::list<std::string>
PMULSelectionDataProvider::pkgsList (const LangCode& locale, bool is_delpacks) const
{
    std::set<PMSelectablePtr> slclist;

    // prefer cached pointer list over string list retrieval

    if (is_delpacks)
    {
	slcmapIT it = slcmapFind (_ptrs_attr_DELPACKS, locale, false);
	if (it == _ptrs_attr_DELPACKS.end())
	{
	    return posmapLLookup (_attr_DELPACKS, locale, false);
	}
	slclist = it->second;
    }
    else
    {
	slcmapIT it = slcmapFind (_ptrs_attr_INSPACKS, locale, false);
	if (it == _ptrs_attr_INSPACKS.end())
	{
	    return posmapLLookup (_attr_INSPACKS, locale, false);
	}
	slclist = it->second;
    }
    return slclist2strlist (slclist);
}

///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : pkgsPointers
//	METHOD TYPE : std::set<PMSelectablePtr>
//
//	DESCRIPTION : lookup INSPACKS or DELPACKS pointer list
//		if pointer list empty, create on-demand from string list
//		and delete string list
//
std::set<PMSelectablePtr>
PMULSelectionDataProvider::pkgsPointers (const LangCode & locale, bool is_delpacks) const
{
    slcmaptype slcmap;
    if (is_delpacks)
	slcmap = _ptrs_attr_DELPACKS;
    else
	slcmap = _ptrs_attr_INSPACKS;

    slcmapIT slcpos = slcmapFind (slcmap, locale, false);		// already set ?

    if (slcpos != slcmap.end())			// Yes
    {
	return slcpos->second;
    }

    std::list<std::string> strlist;

    posmapIT it;			// find retrieval pointer
    if (is_delpacks)
    {
	it = posmapFind (_attr_DELPACKS, locale, false);
	if (it == _attr_DELPACKS.end())		// no retrieval pointer
	{
	    return std::set<PMSelectablePtr>();
	}
    }
    else
    {
	it = posmapFind (_attr_INSPACKS, locale, false);
	if (it == _attr_INSPACKS.end())		// no retrieval pointer
	{
	    return std::set<PMSelectablePtr>();
	}
    }
    _selection_retrieval->retrieveData (it->second, strlist);

    //-----------------------------------------------------
    // now we have the string list and it's position
    // convert the string list to a PMSelectable set

    std::set<PMSelectablePtr> slclist;
    for (std::list<std::string>::const_iterator strpos = strlist.begin();
	 strpos != strlist.end(); ++strpos)
    {
	PMSelectablePtr selectable = Y2PM::packageManager().getItem( *strpos );

	if ( !selectable && strpos->find_first_of( " (,)" ) != string::npos ) {
	  // Scan for alternates: name(alternative [,alternative ...])
	  // Additional catches names surrounded by whitespace
	  vector<string> names;
	  stringutil::split( *strpos, names, " (,)" );
	  for ( unsigned i = 0; i < names.size(); ++i ) {
	    selectable = Y2PM::packageManager().getItem( names[i] );
	    if ( selectable )
	      break;
	  }
	}

	if (selectable)
	    slclist.insert(selectable);
    }

    // cache list for next time

    if (is_delpacks)
    {
	_ptrs_attr_DELPACKS[locale] = slclist;
    }
    else
    {
	_ptrs_attr_INSPACKS[locale] = slclist;
    }

    return slclist;
}


//---------------------------------------------------------------------------------------
// public access functions
//---------------------------------------------------------------------------------------

std::string
PMULSelectionDataProvider::summary(const PMSelection & sel_r, const LangCode& locale) const
{
    return posmapSLookup (_attr_SUMMARY, locale, true);
}

std::list<std::string>
PMULSelectionDataProvider::description(const PMSelection & sel_r, const LangCode& locale) const
{
    return posmapLLookup (_attr_DESCRIPTION, locale, true);
}

std::list<std::string>
PMULSelectionDataProvider::insnotify(const PMSelection & sel_r, const LangCode& locale) const
{
    return posmapLLookup (_attr_INSNOTIFY, locale, true);
}

std::list<std::string>
PMULSelectionDataProvider::delnotify(const PMSelection & sel_r, const LangCode& locale) const
{
    return posmapLLookup (_attr_DELNOTIFY, locale, true);
}

FSize
PMULSelectionDataProvider::size( const PMSelection & sel_r ) const
{
    return _attr_SIZE;
}


std::string
PMULSelectionDataProvider::category ( const PMSelection & sel_r ) const
{
    return _attr_CATEGORY;
}

bool
PMULSelectionDataProvider::visible ( const PMSelection & sel_r ) const
{
    return _attr_VISIBLE;
}

std::list<std::string>
PMULSelectionDataProvider::suggests( const PMSelection & sel_r ) const
{
    return _attr_SUGGESTS;
}

std::list<PMSelectionPtr>
PMULSelectionDataProvider::suggests_ptrs( const PMSelection & sel_r ) const
{
    return _ptrs_attr_SUGGESTS;
}


std::list<std::string>
PMULSelectionDataProvider::recommends( const PMSelection & sel_r ) const
{
    return _attr_RECOMMENDS;
}

std::list<PMSelectionPtr>
PMULSelectionDataProvider::recommends_ptrs( const PMSelection & sel_r ) const
{
    return _ptrs_attr_RECOMMENDS;
}

std::list<std::string>
PMULSelectionDataProvider::inspacks(const PMSelection & sel_r, const LangCode& locale) const
{
    return pkgsList (locale, false);
}

std::list<std::string>
PMULSelectionDataProvider::delpacks(const PMSelection & sel_r, const LangCode& locale) const
{
    return pkgsList (locale, true);
}

std::set<PMSelectablePtr>
PMULSelectionDataProvider::inspacks_ptrs(const PMSelection & sel_r, const LangCode& locale) const
{
    return pkgsPointers (locale, false);
}

std::set<PMSelectablePtr>
PMULSelectionDataProvider::delpacks_ptrs(const PMSelection & sel_r, const LangCode& locale) const
{
    return pkgsPointers (locale, true);
}

FSize
PMULSelectionDataProvider::archivesize ( const PMSelection & sel_r ) const
{
    return _attr_ARCHIVESIZE;
}

std::string
PMULSelectionDataProvider::order ( const PMSelection & sel_r ) const
{
    return _attr_ORDER;
}

bool
PMULSelectionDataProvider::isBase ( const PMSelection & sel_r ) const
{
    return _attr_ISBASE;
}

PMError
PMULSelectionDataProvider::provideSelToInstall( const PMSelection & sel_r, Pathname & path_r ) const
{
#warning PROVIDE THE FILE!!! Not just return it's expected path.
  path_r = _selection_retrieval->getName();
  return PMError::E_ok;
}
