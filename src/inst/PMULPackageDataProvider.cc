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

  File:       PMULPackageDataProvider.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Realize PackageDataProvider for UnitedLinux packages format

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMULPackageDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMULPackageDataProviderPtr
//	CLASS NAME : constPMULPackageDataProviderPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(PMULPackageDataProvider,PMPackageDataProvider,PMDataProvider);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::PMULPackageDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION : open packages stream and keep pointer to tag parser
//		      for later value retrieval on-demand
//
PMULPackageDataProvider::PMULPackageDataProvider(TagCacheRetrievalPtr package_retrieval)
    : _package_retrieval (package_retrieval)
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::~PMULPackageDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMULPackageDataProvider::~PMULPackageDataProvider()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::startRetrieval
//	METHOD TYPE : void
//
//	DESCRIPTION : hint to keep file stream open
//
void
PMULPackageDataProvider::startRetrieval() const
{
#if 0
    _package_retrieval->startRetrieval();
    _language_retrieval->startRetrieval();
#endif
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::stopRetrieval
//	METHOD TYPE : void
//
//	DESCRIPTION : hint to close stream
//
void
PMULPackageDataProvider::stopRetrieval() const
{
#if 0
    _package_retrieval->stopRetrieval();
    _language_retrieval->stopRetrieval();
#endif
}

#define FALLBACK(attr,func) \
  do { if (attr.empty() && (_fallback_provider != 0)) return _fallback_provider->func(); } while (0);

const std::string
PMULPackageDataProvider::summary () const
{
    FALLBACK(_attr_SUMMARY,summary);
    std::string value;
    _language_retrieval->retrieveData (_attr_SUMMARY, value);
    return value;
}

const std::list<std::string>
PMULPackageDataProvider::description () const
{
    FALLBACK(_attr_DESCRIPTION,description);
    std::list<std::string> value;
    _language_retrieval->retrieveData (_attr_DESCRIPTION, value);
    return value;
}

const std::list<std::string>
PMULPackageDataProvider::insnotify () const
{
    FALLBACK(_attr_INSNOTIFY,insnotify);
    std::list<std::string> value;
    _language_retrieval->retrieveData (_attr_INSNOTIFY, value);
    return value;
}

const std::list<std::string>
PMULPackageDataProvider::delnotify () const
{
    FALLBACK(_attr_DELNOTIFY,delnotify);
    std::list<std::string> value;
    _language_retrieval->retrieveData (_attr_DELNOTIFY, value);
    return value;
}

const FSize
PMULPackageDataProvider::size () const
{
    return _attr_SIZE;
}


const Date
PMULPackageDataProvider::buildtime () const
{
    return _attr_BUILDTIME;
}

const std::string
PMULPackageDataProvider::license () const
{
    FALLBACK(_attr_LICENSE,license);
    std::string value;
    _package_retrieval->retrieveData (_attr_LICENSE, value);
    return value;
}

const std::string
PMULPackageDataProvider::group () const
{
    FALLBACK(_attr_GROUP,group);
    std::string value;
    _package_retrieval->retrieveData (_attr_GROUP, value);
    return value;
}

const std::string
PMULPackageDataProvider::sourcerpm () const
{
    FALLBACK(_attr_SOURCERPM,sourcerpm);
    std::string value;
    _package_retrieval->retrieveData (_attr_SOURCERPM, value);
    return value;
}

const FSize
PMULPackageDataProvider::archivesize () const
{
  return _attr_ARCHIVESIZE;
}

const std::list<std::string>
PMULPackageDataProvider::authors () const
{
    FALLBACK(_attr_AUTHORS,authors);
    std::list<std::string> value;
    _package_retrieval->retrieveData (_attr_AUTHORS, value);
    return value;
}

// suse packages values
const std::list<std::string>
PMULPackageDataProvider::recommends () const
{
    FALLBACK(_attr_RECOMMENDS,recommends);
    std::list<std::string> value;
    _package_retrieval->retrieveData (_attr_RECOMMENDS, value);
    return value;
}

const std::list<std::string>
PMULPackageDataProvider::suggests () const
{
    FALLBACK(_attr_SUGGESTS,suggests);
    std::list<std::string> value;
    _package_retrieval->retrieveData (_attr_SUGGESTS, value);
    return value;
}

const std::string
PMULPackageDataProvider::location () const
{
    FALLBACK(_attr_LOCATION,location);
    std::string value;
    _package_retrieval->retrieveData (_attr_LOCATION, value);
    return value;
}

const int
PMULPackageDataProvider::medianr () const
{
    return _attr_MEDIANR;
}

const std::list<std::string>
PMULPackageDataProvider::keywords () const
{
    FALLBACK(_attr_KEYWORDS,keywords);
    std::list<std::string> value;
    _package_retrieval->retrieveData (_attr_KEYWORDS, value);
    return value;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
PMULPackageDataProvider::dumpOn( ostream & str ) const
{
    Rep::dumpOn( str );
    return str;
}
