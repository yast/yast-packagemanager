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
    TagCacheRetrieval *retrieval = _package_retrieval;
    TagCacheRetrieval::retrieval_t *pos_ptr;

    switch (attr)
    {
	case PMPackage::ATTR_REQUIRES:
	    return obj_r->PkgRelList2AttributeValue (obj_r->requires());
	break;
	case PMPackage::ATTR_PREREQUIRES:
	    return obj_r->PkgRelList2AttributeValue (obj_r->prerequires());
	break;
	case PMPackage::ATTR_PROVIDES:
	    return obj_r->PkgRelList2AttributeValue (obj_r->provides());
	break;
	case PMPackage::ATTR_CONFLICTS:
	    return obj_r->PkgRelList2AttributeValue (obj_r->conflicts());
	break;
	case PMPackage::ATTR_OBSOLETES:
	    return obj_r->PkgRelList2AttributeValue (obj_r->obsoletes());
	break;

	case PMPackage::ATTR_SUMMARY:
	    pos_ptr = &attr_SUMMARY;
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
	case PMPackage::ATTR_NAME:
	    return PkgAttributeValue (obj_r->name());
	break;
	case PMPackage::ATTR_VERSION:
	    return PkgAttributeValue (obj_r->edition().version());
	break;
	case PMPackage::ATTR_RELEASE:
	    return PkgAttributeValue (obj_r->edition(). release());
	break;
	case PMPackage::ATTR_ARCH:
	    return PkgAttributeValue (obj_r->arch());
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
	    return PkgAttributeValue();
    }

    if (pos_ptr->begin >= pos_ptr->end)
	return PkgAttributeValue ();

    std::list<std::string> value;

    if (!retrieval->retrieveData (*pos_ptr, value))
    {
	return PkgAttributeValue("ERR");
    }
    if (value.empty())
	return PkgAttributeValue ();

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
    PkgAttributeValue value = getValue (obj_r, (PMPackage::PMPackageAttribute)attr);
    if (_fallback_provider != 0
	&& value.empty())
    {
MIL << "Fallback from " << this << " to " << _fallback_provider << endl;
	return _fallback_provider->getAttributeValue (obj_r, attr);
    }
    return value;
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
    PkgAttributeValue value = getValue (pkg_r, (PMPackage::PMPackageAttribute)attr);
    if (_fallback_provider != 0
	&& value.empty())
    {
MIL << "Fallback from " << this << " to " << _fallback_provider << endl;
	return _fallback_provider->getAttributeValue (pkg_r, attr);
    }
    return value;
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
	case PMPackage::ATTR_SUMMARY:	  SET_RETRIEVAL (SUMMARY,begin,end); break;
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
