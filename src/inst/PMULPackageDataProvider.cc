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
    std::list<std::string> value;
    TagCacheRetrieval *retrieval = _package_retrieval;
    TagCacheRetrieval::retrieval_t *pos_ptr;

    switch (attr)
    {
	case PMPackage::ATTR_REQUIRES:
	    pos_ptr = &attr_REQUIRES;
	break;
	case PMPackage::ATTR_PREREQUIRES:
	    pos_ptr = &attr_PREREQUIRES;
	break;
	case PMPackage::ATTR_PROVIDES:
	    pos_ptr = &attr_PROVIDES;
	break;
	case PMPackage::ATTR_CONFLICTS:
	    pos_ptr = &attr_CONFLICTS;
	break;
	case PMPackage::ATTR_OBSOLETES:
	    pos_ptr = &attr_OBSOLETES;
	break;

	case PMPackage::ATTR_RECOMMENDS:
	    pos_ptr = &attr_RECOMMENDS;
	break;
	case PMPackage::ATTR_SUGGESTS:
	    pos_ptr = &attr_SUGGESTS;
	break;
	case PMPackage::ATTR_AUTHOR:
	    pos_ptr = &attr_AUTHOR;
	break;
	case PMPackage::ATTR_KEYWORDS:
	    pos_ptr = &attr_KEYWORDS;
	break;

	case PMPackage::ATTR_DESCRIPTION:
	    retrieval = _language_retrieval;
	    pos_ptr = &attr_DESCRIPTION;
	break;
	case PMPackage::ATTR_INSNOTIFY:
	    retrieval = _language_retrieval;
	    pos_ptr = &attr_INSNOTIFY;
	break;
	case PMPackage::ATTR_DELNOTIFY:
	    retrieval = _language_retrieval;
	    pos_ptr = &attr_DELNOTIFY;
	break;
#warning ATTR_NAME should come from underlying PMObject
	case PMPackage::ATTR_NAME:
	    return attr_NAME;
	break;
#warning ATTR_VERSION should come from underlying PMObject
	case PMPackage::ATTR_VERSION:
	    return attr_VERSION;
	break;
#warning ATTR_RELEASE should come from underlying PMObject
	case PMPackage::ATTR_RELEASE:
	    return attr_RELEASE;
	break;
#warning ATTR_ARCH should come from underlying PMObject
	case PMPackage::ATTR_ARCH:
	    return attr_ARCH;
	break;

	case PMPackage::ATTR_SUMMARY:
	    return attr_SUMMARY;
	break;

	case PMPackage::ATTR_LOCATION:
	    return attr_LOCATION;
	break;
	case PMPackage::ATTR_ARCHIVESIZE:
	    return attr_ARCHIVESIZE;
	break;
	case PMPackage::ATTR_SIZE:
	    return attr_SIZE;
	break;
	case PMPackage::ATTR_BUILDTIME:
	    return attr_BUILDTIME;
	break;
	case PMPackage::ATTR_SOURCERPM:
	    return attr_SOURCERPM;
	break;
	case PMPackage::ATTR_GROUP:
	    return attr_GROUP;
	break;
	case PMPackage::ATTR_LICENSE:
	    return attr_LICENSE;
	break;

	default:
	    return PkgAttributeValue("?");
    }

    if (!retrieval->retrieveData (*pos_ptr, value))
    {
	    return PkgAttributeValue("ERR");
    }	
    return PkgAttributeValue (value);
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
//	DESCRIPTION :Package attribute retrieval.
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
    D__ << "PMULPackageDataProvider::setAttributeValue (" << pkg->getAttributeName(attr) << ")" << endl;

    switch (attr)
    {
	case PMPackage::ATTR_NAME:	  attr_NAME = value; break;
	case PMPackage::ATTR_VERSION:	  attr_VERSION = value; break;
	case PMPackage::ATTR_RELEASE:	  attr_RELEASE = value; break;
	case PMPackage::ATTR_ARCH:	  attr_ARCH = value; break;

	case PMPackage::ATTR_SUMMARY:	  attr_SUMMARY = value; break;
	case PMPackage::ATTR_LOCATION:	  attr_LOCATION = value; break;
	case PMPackage::ATTR_ARCHIVESIZE: attr_ARCHIVESIZE = value; break;
	case PMPackage::ATTR_SIZE:	  attr_SIZE = value; break;
	case PMPackage::ATTR_BUILDTIME:   attr_BUILDTIME = value; break;
	case PMPackage::ATTR_SOURCERPM:	  attr_SOURCERPM = value; break;
	case PMPackage::ATTR_GROUP:	  attr_GROUP = value; break;
	case PMPackage::ATTR_LICENSE:	  attr_LICENSE = value;	break;

	default:
	    break;
    }

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
    D__ << "PMULPackageDataProvider::setAttributeValue (" << pkg->getAttributeName(attr) << ")" << endl;

#define SET_RETRIEVAL(name,start,stop) \
	do { attr_##name.begin = start; attr_##name.end = stop; } while (0)

    switch (attr)
    {
	case PMPackage::ATTR_REQUIRES:	  SET_RETRIEVAL (REQUIRES,begin,end); break;
	case PMPackage::ATTR_PREREQUIRES: SET_RETRIEVAL (PREREQUIRES,begin,end); break;
	case PMPackage::ATTR_PROVIDES:	  SET_RETRIEVAL (PROVIDES,begin,end); break;
	case PMPackage::ATTR_CONFLICTS:	  SET_RETRIEVAL (CONFLICTS,begin,end); break;
	case PMPackage::ATTR_OBSOLETES:	  SET_RETRIEVAL (OBSOLETES,begin,end); break;
	case PMPackage::ATTR_RECOMMENDS:  SET_RETRIEVAL (RECOMMENDS,begin,end); break;
	case PMPackage::ATTR_SUGGESTS:	  SET_RETRIEVAL (SUGGESTS,begin,end); break;
	case PMPackage::ATTR_AUTHOR:	  SET_RETRIEVAL (AUTHOR,begin,end); break;
	case PMPackage::ATTR_KEYWORDS:	  SET_RETRIEVAL (KEYWORDS,begin,end); break;
	case PMPackage::ATTR_DESCRIPTION: SET_RETRIEVAL (DESCRIPTION,begin,end); break;
	case PMPackage::ATTR_INSNOTIFY:	  SET_RETRIEVAL (INSNOTIFY,begin,end); break;
	case PMPackage::ATTR_DELNOTIFY:	  SET_RETRIEVAL (DELNOTIFY,begin,end); break;
	break;

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
