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
PMULSelectionDataProvider::posmapFind (const TaggedFile::Tag::posmaptype& theMap, const LangCode& locale) const
{
    posmapIT it = theMap.find (locale);
    if (it == theMap.end())
    {
	const std::string lang = locale;
	if (lang.size() > 2)
	{
	    it = theMap.find (LangCode (lang.substr (0, 2)));
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
PMULSelectionDataProvider::slcmapFind (const slcmaptype& theMap, const LangCode& locale) const
{
    slcmapIT it = theMap.find (locale);
    if (it == theMap.end())
    {
	const std::string lang = locale;
	if (lang.size() > 2)
	{
	    it = theMap.find (LangCode (lang.substr (0, 2)));
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
PMULSelectionDataProvider::posmapSLookup (const TaggedFile::Tag::posmaptype& theMap, const LangCode& locale) const
{
    std::string value;
    posmapIT it = posmapFind (theMap, locale);
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
PMULSelectionDataProvider::posmapLLookup (const TaggedFile::Tag::posmaptype& theMap, const LangCode& locale) const
{
    std::list<std::string> value;
    posmapIT it = posmapFind (theMap, locale);
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
//	DESCRIPTION : convert list of selectable pointers to list of
//		selection names
//
std::list<std::string>
PMULSelectionDataProvider::slclist2strlist (const std::list<PMSelectablePtr>& slclist) const
{
    std::list<std::string> strlist;
    for (std::list<PMSelectablePtr>::const_iterator it = slclist.begin(); it != slclist.end(); ++it)
    {
	strlist.push_back ((const std::string &)((*it)->name()));
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
//		if corresponding pointer list is non-empty,
//		create string list on-demand from pointer list
//		else retrieve string list
//
std::list<std::string>
PMULSelectionDataProvider::pkgsList (const LangCode& locale, bool is_delpacks) const
{
    std::list<PMSelectablePtr> slclist;

    // prefer cached pointer list over string list retrieval

    if (is_delpacks)
    {
	slcmapIT it = slcmapFind (_ptrs_attr_DELPACKS, locale);
	if (it == _ptrs_attr_DELPACKS.end())
	{
	    return posmapLLookup (_attr_DELPACKS, locale);
	}
	slclist = it->second;
    }
    else
    {
	slcmapIT it = slcmapFind (_ptrs_attr_INSPACKS, locale);
	if (it == _ptrs_attr_INSPACKS.end())
	{
	    return posmapLLookup (_attr_INSPACKS, locale);
	}
	slclist = it->second;
    }
    return slclist2strlist (slclist);
}

///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : pkgsPointers
//	METHOD TYPE : std::list<PMSelectablePtr>
//
//	DESCRIPTION : lookup INSPACKS or DELPACKS pointer list
//		if pointer list empty, create on-demand from string list
//		and delete string list
//		This on-demand functionality requires a non-const
//		pointer to 'this' as an argument
//

std::list<PMSelectablePtr>
PMULSelectionDataProvider::pkgsPointers (PMULSelectionDataProviderPtr prv, const LangCode & locale, bool is_delpacks) const
{
    slcmaptype slcmap;
    if (is_delpacks)
	slcmap = prv->_ptrs_attr_DELPACKS;
    else
	slcmap = prv->_ptrs_attr_INSPACKS;

    slcmapIT slcpos = slcmapFind (slcmap, locale);		// already set ?

    if (slcpos != slcmap.end())			// Yes
    {
	return slcpos->second;
    }

    std::list<std::string> strlist;

    posmapIT it;			// find retrieval pointer
    if (is_delpacks)
    {
	it = posmapFind (prv->_attr_DELPACKS, locale);
	if (it == prv->_attr_DELPACKS.end())		// no retrieval pointer
	{
	    return std::list<PMSelectablePtr>();
	}
    }
    else
    {
	it = posmapFind (prv->_attr_INSPACKS, locale);
	if (it == prv->_attr_INSPACKS.end())		// no retrieval pointer
	{
	    return std::list<PMSelectablePtr>();
	}
    }
    _selection_retrieval->retrieveData (it->second, strlist);

    //-----------------------------------------------------
    // now we have the string list and it's position
    // convert the string list to a PMSelectable list

    std::list<PMSelectablePtr> slclist;
    for (std::list<std::string>::const_iterator strpos = strlist.begin();
	 strpos != strlist.end(); ++strpos)
    {
	PMSelectablePtr selectable = Y2PM::packageManager().getItem (*strpos);
	if (selectable)
	    slclist.push_back (selectable);
    }

    // cache list for next time

    if (is_delpacks)
    {
	prv->_ptrs_attr_DELPACKS[locale] = slclist;
    }
    else
    {
	prv->_ptrs_attr_INSPACKS[locale] = slclist;
    }

    return slclist;
}


//---------------------------------------------------------------------------------------
// public access functions
//---------------------------------------------------------------------------------------

std::string
PMULSelectionDataProvider::summary(const PMSelection & sel_r, const LangCode& locale) const
{
    return posmapSLookup (_attr_SUMMARY, locale);
}

std::list<std::string>
PMULSelectionDataProvider::description(const PMSelection & sel_r, const LangCode& locale) const
{
    return posmapLLookup (_attr_DESCRIPTION, locale);
}

std::list<std::string>
PMULSelectionDataProvider::insnotify(const PMSelection & sel_r, const LangCode& locale) const
{
    return posmapLLookup (_attr_INSNOTIFY, locale);
}

std::list<std::string>
PMULSelectionDataProvider::delnotify(const PMSelection & sel_r, const LangCode& locale) const
{
    return posmapLLookup (_attr_DELNOTIFY, locale);
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
    return sellist2strlist (_ptrs_attr_SUGGESTS);
}

std::list<PMSelectionPtr>
PMULSelectionDataProvider::suggests_ptrs( const PMSelection & sel_r ) const
{
    return _ptrs_attr_SUGGESTS;
}


std::list<std::string>
PMULSelectionDataProvider::recommends( const PMSelection & sel_r ) const
{
    return sellist2strlist (_ptrs_attr_RECOMMENDS);
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

std::list<PMSelectablePtr>
PMULSelectionDataProvider::inspacks_ptrs(const PMSelection & sel_r, const LangCode& locale) const
{
    return pkgsPointers (PMULSelectionDataProviderPtr::cast_away_const(this), locale, false);
}

std::list<std::string>
PMULSelectionDataProvider::delpacks(const PMSelection & sel_r, const LangCode& locale) const
{
    return pkgsList (locale, true);
}

std::list<PMSelectablePtr>
PMULSelectionDataProvider::delpacks_ptrs(const PMSelection & sel_r, const LangCode& locale) const
{
    return pkgsPointers (PMULSelectionDataProviderPtr::cast_away_const(this), locale, true);
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

#if 0

//-------------------------------------------------------------------
// fill selections with caching data
// set up lists of PMSelectionPtr and PMPackagePtr
// for suggests, inspacks, delpacks

PMError
ULSelectionParser::fillSelections (std::list<PMSelectionPtr>& all_selections, std::list<PMPackagePtr>& all_packages)
{
    PMError err;

    for (std::list<PMSelectionPtr>::iterator selIt = all_selections.begin();
	 selIt != all_selections.end(); ++selIt)
    {
//	MIL << "fillSelection (" << (*selIt)->name() << ")" << endl;
        PMULSelectionDataProviderPtr selDp = getDataProvider( *selIt );
	if ( !selDp ) {
	  INT << "SUSPICIOUS: got NULL SelectionDataProviderPtr to fill" << endl;
	  continue;
	}
	selDp->_ptrs_attr_SUGGESTS = lookupSelections (all_selections, (*selIt)->suggests());
	selDp->_ptrs_attr_RECOMMENDS = lookupSelections (all_selections, (*selIt)->recommends());

	for (TaggedFile::Tag::posmaptype::iterator tagIt = selDp->_attr_INSPACKS.begin();
	     tagIt != selDp->_attr_INSPACKS.end(); ++tagIt)
	{
	    // get language packages
	    std::list<std::string> inspackages = (*selIt)->inspacks (tagIt->first);
	    if (!inspackages.empty())
	    {
		selDp->_ptrs_attr_INSPACKS[tagIt->first] = lookupPackages (all_packages, inspackages);
	    }
	}
	for (TaggedFile::Tag::posmaptype::iterator tagIt = selDp->_attr_DELPACKS.begin();
	     tagIt != selDp->_attr_DELPACKS.end(); ++tagIt)
	{
	    // get language packages
	    std::list<std::string> delpackages = (*selIt)->delpacks (tagIt->first);
	    if (!delpackages.empty())
	    {
		selDp->_ptrs_attr_DELPACKS[tagIt->first] = lookupPackages (all_packages, delpackages);
	    }
	}
    }
    return err;
}



///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : lookupSelectionPtrs
//	METHOD TYPE : std::list<PMPackagePtr>
//
//	DESCRIPTION : lookup selection names to PMSelectionPtr
//
std::list<PMSelectionPtr>
PMULSelectionDataProvider::lookupSelectionPtrs (const std::list<PMSelectionPtr> all_selections, const std::list<std::string>& selections)
{
    std::list<PMSelectionPtr> selection_ptrs;

    for (std::list<std::string>::const_iterator selIt = selections.begin();
	 selIt != selections.end(); ++selIt)
    {
	std::list<PMSelectionPtr> matches = InstData::findSelections (all_selections, *selIt);
	// silently ignore selections not found
	if (matches.size() > 0)
	{
	    selection_ptrs.push_back (matches.front());
	}
    }

    return selection_ptrs;
}


///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : PMULSelectionDataProvider::lookupPackagePtrs
//	METHOD TYPE : std::list<PMPackagePtr>
//
//	DESCRIPTION : lookup package names to PMPackagePtr
//
std::list<PMPackagePtr>
PMULSelectionDataProvider::lookupPackagePtrs (const std::list<std::string>& packages)
{
    std::list<PMPackagePtr> package_ptrs;

    for (std::list<std::string>::const_iterator pkgIt = packages.begin();
	 pkgIt != packages.end(); ++pkgIt)
    {
	string name = *pkgIt;
	std::list<PMPackagePtr> matches;

	// check for alternative package
	string::size_type spacepos = name.find_first_of (" ");
	if (spacepos != string::npos)
	{
	    string wantedname = name.substr (0, spacepos);
	    matches = InstData::findPackages (all_packages, wantedname);
	    if (matches.size() == 0)
	    {
		string::size_type startpos = name.find_first_of ("(", spacepos);
		if (startpos != string::npos)
		{
		    string::size_type endpos = name.find_first_of (")", startpos);
		    if (endpos != string::npos)
		    {
			string alternative = name.substr (startpos+1, endpos-startpos-1);
			matches = InstData::findPackages (all_packages, alternative);
		    }
		}
	    }
	}
	else
	{
#warning lookup in arch order
	    matches = InstData::findPackages (all_packages, name);
	}

	// silently ignore packages not found
	if (matches.size() > 0)
	{
	    package_ptrs.push_back (matches.front());
	}
    }
    return package_ptrs;
}
#endif


