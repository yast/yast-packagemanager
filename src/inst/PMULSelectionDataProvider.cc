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

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMULSelectionDataProviderPtr
//	CLASS NAME : constPMULSelectionDataProviderPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(PMULSelectionDataProvider,PMSelectionDataProvider,PMDataProvider);

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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULSelectionDataProvider::startRetrieval
//	METHOD TYPE : bool
//
//	DESCRIPTION : hint to keep file stream open
//
void
PMULSelectionDataProvider::startRetrieval() const
{
#if 0
    _selection_retrieval->startRetrieval();
#endif
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULSelectionDataProvider::stopRetrieval
//	METHOD TYPE : void
//
//	DESCRIPTION : hint to close stream
//
void
PMULSelectionDataProvider::stopRetrieval() const
{
#if 0
    _selection_retrieval->stopRetrieval();
#endif
}


//-------------------------------------------------------------------
// public access functions
//-------------------------------------------------------------------

typedef map <std::string,TagCacheRetrievalPos>::const_iterator tagmapIT;
typedef map <std::string,std::list<PMPackagePtr> >::const_iterator pkgsmapIT;

const std::string
PMULSelectionDataProvider::summary(const std::string& lang) const
{
    MIL << "PMULSelectionDataProvider::summary()" << endl;
    std::string value;
    tagmapIT it = _attr_SUMMARY.find(lang);
    if (it == _attr_SUMMARY.end())
    {
	MIL << "No summary for language '" << lang << "' available" << endl;
    }
    else
    {
	_selection_retrieval->retrieveData (it->second, value);
    }
    return value;
}

const std::list<std::string>
PMULSelectionDataProvider::description(const std::string& lang) const
{
    std::list<std::string> value;
    _selection_retrieval->retrieveData (_attr_DESCRIPTION, value);
    return value;
}

const std::list<std::string>
PMULSelectionDataProvider::insnotify(const std::string& lang) const
{
    std::list<std::string> value;
    _selection_retrieval->retrieveData (_attr_INSNOTIFY, value);
    return value;
}

const std::list<std::string>
PMULSelectionDataProvider::delnotify(const std::string& lang) const
{
    std::list<std::string> value;
    _selection_retrieval->retrieveData (_attr_DELNOTIFY, value);
    return value;
}

const FSize
PMULSelectionDataProvider::size() const
{
    return _attr_SIZE;
}


const std::string
PMULSelectionDataProvider::category () const
{
    return _attr_CATEGORY;
}

const bool
PMULSelectionDataProvider::visible () const
{
    return _attr_VISIBLE;
}

const std::list<std::string>
PMULSelectionDataProvider::suggests() const
{
    std::list<std::string> value;
    _selection_retrieval->retrieveData (_attr_SUGGESTS, value);
    return value;
}

const std::list<PMSelectionPtr>
PMULSelectionDataProvider::suggests_ptrs()
{
    return _ptrs_attr_SUGGESTS;
}


const std::list<std::string>
PMULSelectionDataProvider::recommends() const
{
    std::list<std::string> value;
    _selection_retrieval->retrieveData (_attr_RECOMMENDS, value);
    return value;
}

const std::list<PMSelectionPtr>
PMULSelectionDataProvider::recommends_ptrs()
{
    return _ptrs_attr_RECOMMENDS;
}

const std::list<std::string>
PMULSelectionDataProvider::inspacks(const std::string& lang) const
{
    std::list<std::string> value;
    tagmapIT it = _attr_INSPACKS.find(lang);
    if (it != _attr_INSPACKS.end())
	_selection_retrieval->retrieveData (it->second, value);
    return value;
}

const std::list<PMPackagePtr>
PMULSelectionDataProvider::inspacks_ptrs(const std::string& lang)
{
    // already set ?
    pkgsmapIT it = _ptrs_attr_INSPACKS.find(lang);
    if (it == _ptrs_attr_INSPACKS.end())
	return std::list<PMPackagePtr>();
    return it->second;
}

const std::list<std::string>
PMULSelectionDataProvider::delpacks(const std::string& lang) const
{
    std::list<std::string> value;
    tagmapIT it = _attr_DELPACKS.find(lang);
    if (it != _attr_DELPACKS.end())
	_selection_retrieval->retrieveData (it->second, value);
    return value;
}

const std::list<PMPackagePtr>
PMULSelectionDataProvider::delpacks_ptrs(const std::string& lang)
{
    // already set ?
    pkgsmapIT it = _ptrs_attr_DELPACKS.find(lang);
    if (it != _ptrs_attr_DELPACKS.end())
	return std::list<PMPackagePtr>();
    return it->second;
}

const FSize
PMULSelectionDataProvider::archivesize () const
{
    return _attr_ARCHIVESIZE;
}

const std::string
PMULSelectionDataProvider::order () const
{
    return _attr_ORDER;
}

const bool
PMULSelectionDataProvider::isBase () const
{
    return _attr_ISBASE;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULSelectionDataProvider::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
PMULSelectionDataProvider::dumpOn( ostream & str ) const
{
    Rep::dumpOn( str );
    return str;
}
