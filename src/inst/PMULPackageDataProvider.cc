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
#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMULPackageDataProviderPtr
//	CLASS NAME : constPMULPackageDataProviderPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(PMULPackageDataProvider,PMPackageDataProvider,PMPackageDataProvider);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::PMULPackageDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION : open packages stream and keep pointer to tag parser
//		      for later value retrieval on-demand
//
PMULPackageDataProvider::PMULPackageDataProvider(TagCacheRetrievalPtr package_retrieval,
			TagCacheRetrievalPtr locale_retrieval)
    : _attr_GROUP(0)
    , _package_retrieval (package_retrieval)
    , _locale_retrieval (locale_retrieval)
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

// NOTE:
// The 'const PMPackage & pkg_r' argument is passed to the _fallback_provider->func.
// This is ok, as InstSrcDataUL uses a per package DataProvider. The pkg_r argument
// is not actually evaluated. (and if it would be, it would point to the Package that
// initiated the request, which is in fact what we want).

#define FALLBACK(attr,func) \
  do { if (attr.empty() && (_fallback_provider != 0)) return _fallback_provider->func(pkg_r); } while (0);

std::string
PMULPackageDataProvider::summary ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_SUMMARY,summary);
    std::string value;
    _locale_retrieval->retrieveData (_attr_SUMMARY, value);
    return value;
}

std::list<std::string>
PMULPackageDataProvider::description ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_DESCRIPTION,description);
    std::list<std::string> value;
    _locale_retrieval->retrieveData (_attr_DESCRIPTION, value);
    return value;
}

std::list<std::string>
PMULPackageDataProvider::insnotify ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_INSNOTIFY,insnotify);
    std::list<std::string> value;
    _locale_retrieval->retrieveData (_attr_INSNOTIFY, value);
    return value;
}

std::list<std::string>
PMULPackageDataProvider::delnotify ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_DELNOTIFY,delnotify);
    std::list<std::string> value;
    _locale_retrieval->retrieveData (_attr_DELNOTIFY, value);
    return value;
}

FSize
PMULPackageDataProvider::size ( const PMPackage & pkg_r ) const
{
    return _attr_SIZE;
}


Date
PMULPackageDataProvider::buildtime ( const PMPackage & pkg_r ) const
{
    return _attr_BUILDTIME;
}

std::string
PMULPackageDataProvider::license ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_LICENSE,license);
    std::string value;
    _package_retrieval->retrieveData (_attr_LICENSE, value);
    return value;
}

std::string
PMULPackageDataProvider::group ( const PMPackage & pkg_r ) const
{
    // FALLBACK
    if (_attr_GROUP == 0)
    {
	if (_fallback_provider != 0)
	{
	    return _fallback_provider->group( pkg_r );
	}
	return "";
    }
    return Y2PM::packageManager().rpmGroup (_attr_GROUP);
}

YStringTreeItem *
PMULPackageDataProvider::group_ptr ( const PMPackage & pkg_r ) const
{
    // FALLBACK
    if (_attr_GROUP == 0)
    {
	if (_fallback_provider != 0)
	{
	    return _fallback_provider->group_ptr( pkg_r );
	}
	return 0;
    }
    return _attr_GROUP;
}


std::string
PMULPackageDataProvider::sourcerpm ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_SOURCERPM,sourcerpm);
    std::string value;
    _package_retrieval->retrieveData (_attr_SOURCERPM, value);
    return value;
}

FSize
PMULPackageDataProvider::archivesize ( const PMPackage & pkg_r ) const
{
  return _attr_ARCHIVESIZE;
}

std::list<std::string>
PMULPackageDataProvider::authors ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_AUTHORS,authors);
    std::list<std::string> value;
    _package_retrieval->retrieveData (_attr_AUTHORS, value);
    return value;
}

// suse packages values
std::list<std::string>
PMULPackageDataProvider::recommends ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_RECOMMENDS,recommends);
    std::list<std::string> value;
    _package_retrieval->retrieveData (_attr_RECOMMENDS, value);
    return value;
}

std::list<std::string>
PMULPackageDataProvider::suggests ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_SUGGESTS,suggests);
    std::list<std::string> value;
    _package_retrieval->retrieveData (_attr_SUGGESTS, value);
    return value;
}

std::string
PMULPackageDataProvider::location ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_LOCATION,location);
    std::string value;
    _package_retrieval->retrieveData (_attr_LOCATION, value);
    return value;
}

int
PMULPackageDataProvider::medianr ( const PMPackage & pkg_r ) const
{
    return _attr_MEDIANR;
}

std::list<std::string>
PMULPackageDataProvider::keywords ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_KEYWORDS,keywords);
    std::list<std::string> value;
    _package_retrieval->retrieveData (_attr_KEYWORDS, value);
    return value;
}

