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
PMULPackageDataProvider::PMULPackageDataProvider(TagCacheRetrieval *package_retrieval, TagCacheRetrieval *language_retrieval)
    : _package_retrieval (package_retrieval)
    , _language_retrieval (language_retrieval)
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
//	METHOD NAME : PMULPackageDataProvider::getValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION : common attribute retrieval.
//
PkgAttributeValue
PMULPackageDataProvider::getValue( constPMObjectPtr obj_r,
				PMPackage::PMPackageAttribute attr )
{
    if (obj_r != _package)
    {
	ERR << "Not my package " << endl;
	return PkgAttributeValue();
    }

    switch (attr)
    {
#if 0
	//---------------------
	// PMSolvable

	case PMPackage::ATTR_NAME:	return PkgAttributeValue (_package->name());
	break;
	case PMPackage::ATTR_VERSION:	return PkgAttributeValue (_package->edition().version());
	break;
	case PMPackage::ATTR_RELEASE:	return PkgAttributeValue (_package->edition(). release());
	break;
	case PMPackage::ATTR_ARCH:	return PkgAttributeValue (_package->arch());
	break;

	case PMPackage::ATTR_REQUIRES:	return _package->PkgRelList2AttributeValue (_package->requires());
	break;
	case PMPackage::ATTR_PREREQUIRES: return _package->PkgRelList2AttributeValue (_package->prerequires());
	break;
	case PMPackage::ATTR_PROVIDES:	return _package->PkgRelList2AttributeValue (_package->provides());
	break;
	case PMPackage::ATTR_OBSOLETES:	return _package->PkgRelList2AttributeValue (_package->obsoletes());
	break;
	case PMPackage::ATTR_CONFLICTS: return _package->PkgRelList2AttributeValue (_package->conflicts());
	break;

	//---------------------
	// PMObject

	case PMPackage::ATTR_SUMMARY:	return PkgAttributeValue (summary());
	break;
	case PMPackage::ATTR_DESCRIPTION: return PkgAttributeValue (description());
	break;
	case PMPackage::ATTR_INSNOTIFY:	return PkgAttributeValue (insnotify());
	break;
	case PMPackage::ATTR_DELNOTIFY: return PkgAttributeValue (delnotify());
	break;
	case PMPackage::ATTR_SIZE:	return PkgAttributeValue (size().asString());
	break;
#endif
	//---------------------
	// PMPackage

	case PMPackage::ATTR_BUILDTIME:	return PkgAttributeValue (buildtime().asString());
	break;
	case PMPackage::ATTR_BUILDHOST:
	case PMPackage::ATTR_INSTALLTIME:
	case PMPackage::ATTR_DISTRIBUTION:
	case PMPackage::ATTR_VENDOR:
	break;
	case PMPackage::ATTR_LICENSE:	return PkgAttributeValue(license());
	break;
	case PMPackage::ATTR_PACKAGER:
	case PMPackage::ATTR_GROUP:	return PkgAttributeValue(group());
	break;
	case PMPackage::ATTR_CHANGELOG:
	case PMPackage::ATTR_URL:
	case PMPackage::ATTR_OS:
	case PMPackage::ATTR_PREIN:
	case PMPackage::ATTR_POSTIN:
	case PMPackage::ATTR_PREUN:
	case PMPackage::ATTR_POSTUN:
	break;
	case PMPackage::ATTR_SOURCERPM:	return PkgAttributeValue(sourcerpm());
	break;
	case PMPackage::ATTR_ARCHIVESIZE: return PkgAttributeValue (archivesize().asString());
	break;
	case PMPackage::ATTR_AUTHORS:	return PkgAttributeValue (authors());
	case PMPackage::ATTR_FILENAMES:
	break;
	case PMPackage::ATTR_RECOMMENDS:return PkgAttributeValue (recommends());
	break;
	case PMPackage::ATTR_SUGGESTS:	return PkgAttributeValue (suggests());
	break;
	case PMPackage::ATTR_LOCATION:	return PkgAttributeValue (location());
	break;
	case PMPackage::ATTR_KEYWORDS:	return PkgAttributeValue (keywords());
	break;
	case PMPackage::PMPKG_NUM_ATTRIBUTES:
	break;
    }

    // data unknown
    return PkgAttributeValue ();
}


void
PMULPackageDataProvider::setValue( PMPackage::PMPackageAttribute attr_r, std::string& value_r )
{
}

void
PMULPackageDataProvider::setValue( PMPackage::PMPackageAttribute attr_r, std::streampos begin, std::streampos end)
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::getAttributeValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION : Object attribute retrieval.
//
PkgAttributeValue
PMULPackageDataProvider::getAttributeValue( constPMObjectPtr obj_r,
					PMObject::PMObjectAttribute attr )
{
    return getValue (obj_r, (PMPackage::PMPackageAttribute)attr);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::getAttributeValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION : Package attribute retrieval.
//
PkgAttributeValue
PMULPackageDataProvider::getAttributeValue( constPMPackagePtr pkg_r,
					PMPackage::PMPackageAttribute attr )
{
    return getValue (pkg_r, (PMPackage::PMPackageAttribute)attr);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::setAttributeValue
//	METHOD TYPE : attribute set
//
//	DESCRIPTION : inject some object attribute by value
//
void
PMULPackageDataProvider::setAttributeValue(
	PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	const PkgAttributeValue& value)
{
    ERR << "Not supported: PMULPackageDataProvider::setAttributeValue (" << pkg->getAttributeName(attr) << ")" << endl;

    return;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::setAttributeValue
//	METHOD TYPE : attribute set
//
//	DESCRIPTION :inject some package attribute by file offset
//
void
PMULPackageDataProvider::setAttributeValue(
	PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	std::streampos begin, std::streampos end)
{
    ERR << "Not supported: PMULPackageDataProvider::setAttributeValue (" << pkg->getAttributeName(attr) << ")" << endl;
    D__ << "PMULPackageDataProvider::setAttributeValue (" << pkg->getAttributeName(attr) << ")" << endl;

#define SET_RETRIEVAL(name,start,stop) \
	do { _attr_##name.begin = start; _attr_##name.end = stop; } while (0)

    switch (attr)
    {
#if 0
	case PMPackage::ATTR_SUMMARY:	  SET_RETRIEVAL (SUMMARY,begin,end); break;
	case PMPackage::ATTR_RECOMMENDS:  SET_RETRIEVAL (RECOMMENDS,begin,end); break;
	case PMPackage::ATTR_SUGGESTS:	  SET_RETRIEVAL (SUGGESTS,begin,end); break;
	case PMPackage::ATTR_AUTHORS:	  SET_RETRIEVAL (AUTHORS,begin,end); break;
	case PMPackage::ATTR_KEYWORDS:	  SET_RETRIEVAL (KEYWORDS,begin,end); break;
	case PMPackage::ATTR_DESCRIPTION: SET_RETRIEVAL (DESCRIPTION,begin,end); break;
	case PMPackage::ATTR_INSNOTIFY:	  SET_RETRIEVAL (INSNOTIFY,begin,end); break;
	case PMPackage::ATTR_DELNOTIFY:	  SET_RETRIEVAL (DELNOTIFY,begin,end); break;
	break;
#endif
#undef SET_RETRIEVAL
	default:
	    break;
    }

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
