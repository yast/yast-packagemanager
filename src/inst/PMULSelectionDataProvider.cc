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

#include <y2pm/PMULSelectionDataProvider.h>
#include <y2pm/InstData.h>

using namespace std;

typedef TaggedFile::Tag::posmaptype::const_iterator posmapIT;
typedef map <std::string,std::list<PMPackagePtr> >::const_iterator pkgsmapIT;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMULSelectionDataProviderPtr
//	CLASS NAME : constPMULSelectionDataProviderPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(PMULSelectionDataProvider,PMSelectionDataProvider,PMSelectionDataProvider);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULSelectionDataProvider::PMULSelectionDataProvider
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
//	METHOD NAME : PMULSelectionDataProvider::~PMULSelectionDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMULSelectionDataProvider::~PMULSelectionDataProvider()
{
}

std::string
PMULSelectionDataProvider::posmapSLookup (TaggedFile::Tag::posmaptype theMap, const std::string& locale) const
{
    std::string value;
    posmapIT it = theMap.find (locale);
    if ((it == theMap.end())
	&& (locale.size() > 2))
    {
	it = theMap.find (locale.substr (0, 2));
    }
    if (it != theMap.end())
    {
	_selection_retrieval->retrieveData (it->second, value);
    }
    return value;
}

std::list<std::string>
PMULSelectionDataProvider::posmapLLookup (TaggedFile::Tag::posmaptype theMap, const std::string& locale) const
{
    std::list<std::string> value;
    posmapIT it = theMap.find (locale);

    if ((it == theMap.end())
	&& (locale.size() > 2))
    {
	it = theMap.find (locale.substr (0, 2));
    }
    if (it != theMap.end())
    {
	_selection_retrieval->retrieveData (it->second, value);
    }
    return value;
}

//-------------------------------------------------------------------
// public access functions
//-------------------------------------------------------------------

std::string
PMULSelectionDataProvider::summary(const PMSelection & sel_r, const std::string& locale) const
{
    return posmapSLookup (_attr_SUMMARY, locale);
}

std::list<std::string>
PMULSelectionDataProvider::description(const PMSelection & sel_r, const std::string& locale) const
{
    return posmapLLookup (_attr_DESCRIPTION, locale);
}

std::list<std::string>
PMULSelectionDataProvider::insnotify(const PMSelection & sel_r, const std::string& locale) const
{
    return posmapLLookup (_attr_INSNOTIFY, locale);
}

std::list<std::string>
PMULSelectionDataProvider::delnotify(const PMSelection & sel_r, const std::string& locale) const
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
    std::list<std::string> value;
    _selection_retrieval->retrieveData (_attr_SUGGESTS, value);
    return value;
}

std::list<PMSelectionPtr>
PMULSelectionDataProvider::suggests_ptrs( const PMSelection & sel_r ) const
{
    return _ptrs_attr_SUGGESTS;
}


std::list<std::string>
PMULSelectionDataProvider::recommends( const PMSelection & sel_r ) const
{
    std::list<std::string> value;
    _selection_retrieval->retrieveData (_attr_RECOMMENDS, value);
    return value;
}

std::list<PMSelectionPtr>
PMULSelectionDataProvider::recommends_ptrs( const PMSelection & sel_r ) const
{
    return _ptrs_attr_RECOMMENDS;
}

std::list<std::string>
PMULSelectionDataProvider::inspacks(const PMSelection & sel_r, const std::string& locale) const
{
    return posmapLLookup (_attr_INSPACKS, locale);
}

std::list<PMPackagePtr>
PMULSelectionDataProvider::inspacks_ptrs(const PMSelection & sel_r, const std::string& locale) const
{
    // already set ?
    pkgsmapIT it = _ptrs_attr_INSPACKS.find(locale);
    if (it == _ptrs_attr_INSPACKS.end())
    {
	if (locale.size() > 2)
	{
	    it = _ptrs_attr_INSPACKS.find(locale.substr(0,2));
	}
	return std::list<PMPackagePtr>();
    }
    return it->second;
}

std::list<std::string>
PMULSelectionDataProvider::delpacks(const PMSelection & sel_r, const std::string& locale) const
{
    return posmapLLookup (_attr_DELPACKS, locale);
}

std::list<PMPackagePtr>
PMULSelectionDataProvider::delpacks_ptrs(const PMSelection & sel_r, const std::string& locale) const
{
    // already set ?
    pkgsmapIT it = _ptrs_attr_DELPACKS.find(locale);
    if (it == _ptrs_attr_DELPACKS.end())
    {
	if (locale.size() > 2)
	{
	    it = _ptrs_attr_DELPACKS.find(locale.substr(0,2));
	}
	if (it == _ptrs_attr_DELPACKS.end())
	    return std::list<PMPackagePtr>();
    }
    return it->second;
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
