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
#include <y2pm/InstSrcDescr.h>
#include <y2pm/PMPackageManager.h>
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
PMULPackageDataProvider::PMULPackageDataProvider (constInstSrcPtr source,
			TagCacheRetrievalPtr package_retrieval,
			TagCacheRetrievalPtr locale_retrieval,
			TagCacheRetrievalPtr du_retrieval)
    : _attr_GROUP(0)
    , _source (source)
    , _package_retrieval (package_retrieval)
    , _locale_retrieval (locale_retrieval)
    , _du_retrieval (du_retrieval)
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

bool
PMULPackageDataProvider::providesSources( const PMPackage & pkg_r ) const
{
  if ( _attr_SOURCELOC.empty() ) {
    if ( _fallback_provider != 0 )
      return _fallback_provider->providesSources(pkg_r);
    return false;
  }
  return true;
}

std::string
PMULPackageDataProvider::instSrcLabel( const PMPackage & pkg_r ) const
{
  if ( _source && _source->descr() ) {
    return _source->descr()->label();
  }
  return PMPackageDataProvider::instSrcLabel();
}

Vendor
PMULPackageDataProvider::instSrcVendor( const PMPackage & pkg_r ) const
{
  if ( _source && _source->descr() ) {
    return _source->descr()->content_vendor();
  }
  return PMPackageDataProvider::instSrcVendor();
}

unsigned
PMULPackageDataProvider::instSrcRank( const PMPackage & pkg_r ) const
{
  if ( _source && _source->descr() ) {
    return _source->descr()->default_rank();
  }
  return PMPackageDataProvider::instSrcRank();
}

PkgSplitSet
PMULPackageDataProvider::splitprovides ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_SPLITPROVIDES,splitprovides);
    return _attr_SPLITPROVIDES;
}


Date
PMULPackageDataProvider::buildtime ( const PMPackage & pkg_r ) const
{
    return _attr_BUILDTIME;
}

Vendor
PMULPackageDataProvider::vendor( const PMPackage & pkg_r ) const
{
  return instSrcVendor( pkg_r );
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
PMULPackageDataProvider::sourceloc ( const PMPackage & pkg_r ) const
{
    FALLBACK(_attr_SOURCELOC,sourceloc);
    std::string value;
    _package_retrieval->retrieveData (_attr_SOURCELOC, value);
    return value;
}

FSize
PMULPackageDataProvider::sourcesize ( const PMPackage & pkg_r ) const
{
    if ((_attr_SOURCESIZE == 0LL)
	&& (_fallback_provider != 0)) return _fallback_provider->sourcesize(pkg_r);
    return _attr_SOURCESIZE;
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

unsigned int
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::du
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMULPackageDataProvider::du( const PMPackage & pkg_r, PkgDu & dudata_r ) const
{
  dudata_r.clear();
  if ( _attr_DU.empty() )
    return;
  std::list<std::string> value;
  _du_retrieval->retrieveData (_attr_DU, value);
  dudata_r.setFrom( value );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::isRemote
//	METHOD TYPE : bool
//
//	DESCRIPTION : call InstSrc to check if it is a remote source
//
bool PMULPackageDataProvider::isRemote (const PMPackage & pkg_r) const
{
    if (_source) return _source->isRemote();
    return false;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::providePkgToInstall
//	METHOD TYPE : PMError
//
//	DESCRIPTION : call InstSrc to provide package, return local path to package in path_r
//
PMError PMULPackageDataProvider::providePkgToInstall( const PMPackage & pkg_r, Pathname& path_r ) const
{
    // determine directory and rpm name
    Pathname rpmname = pkg_r.location();
    Pathname dir;
    string::size_type dirpos = rpmname.asString().find_first_of (" ");
    if (dirpos == string::npos)
    {
	// directory == architecture
	dir = Pathname ((const std::string &)(pkg_r.arch()));
    }
    else
    {
	// directory in location
	dir = Pathname (rpmname.asString().substr (dirpos+1));
	rpmname = Pathname (rpmname.asString().substr (0, dirpos));
    }

    if (!_source)
    {
	ERR << "No source for '" << dir << "/" << rpmname << "'" << endl;
	path_r = Pathname();
	return InstSrcError::E_no_source;
    }

    return _source->providePackage (pkg_r.medianr(), rpmname, dir, path_r);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::provideSrcPkgToInstall
//	METHOD TYPE : PMError
//
//	DESCRIPTION : call InstSrc to provide source package, return local path to package in path_r
//
PMError PMULPackageDataProvider::provideSrcPkgToInstall( const PMPackage & pkg_r, Pathname& path_r ) const
{
    MIL << "provideSrcPkgToInstall " << pkg_r.sourceloc() << endl;

    std::vector<std::string> locsplit;
    stringutil::split (pkg_r.sourceloc(), locsplit, " ", false);

    if (locsplit.size () < 2)
    {
	ERR << "bad location '" << pkg_r.sourceloc() << "'" << endl;
	return InstSrcError::E_no_source;
    }

    // determine media, directory and rpm name
    int medianr = atoi (locsplit[0].c_str());
    Pathname rpmname (locsplit[1]);
    Pathname dir;
    if (locsplit.size() > 3)
	dir = Pathname (locsplit[2]);

    if (!_source)
    {
	ERR << "No source for '" << dir << "/" << rpmname << "'" << endl;
	path_r = Pathname();
	return InstSrcError::E_no_source;
    }

    return _source->providePackage (medianr, rpmname, dir, path_r);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::prefererCandidate
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMULPackageDataProvider::prefererCandidate( const PMPackage & pkg_r ) const
{
  return( source( pkg_r ) && source( pkg_r )->specialCache() );
}

