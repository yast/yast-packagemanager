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
//	DESCRIPTION : open packages stream and keep pointer to tag parser
//		      for later value retrieval on-demand
//
PMULSelectionDataProvider::PMULSelectionDataProvider(TagCacheRetrieval *selection_retrieval)
    : _selection_retrieval (selection_retrieval)
{
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
//	METHOD NAME : PMULSelectionDataProvider::getValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION : common attribute retrieval.
//
PkgAttributeValue
PMULSelectionDataProvider::getValue( constPMObjectPtr obj_r,
				PMSelection::PMSelectionAttribute attr )
{
    TagCacheRetrieval::retrieval_t *pos_ptr;

    switch (attr)
    {
	case PMSelection::ATTR_REQUIRES:
	    pos_ptr = &attr_REQUIRES;
	break;
	case PMSelection::ATTR_PROVIDES:
	    pos_ptr = &attr_PROVIDES;
	break;
	case PMSelection::ATTR_CONFLICTS:
	    pos_ptr = &attr_CONFLICTS;
	break;
	case PMSelection::ATTR_OBSOLETES:
	    pos_ptr = &attr_OBSOLETES;
	break;

	case PMSelection::ATTR_INSTALL:
	    pos_ptr = &attr_INSTALL;
	break;
	case PMSelection::ATTR_DELETE:
	    pos_ptr = &attr_DELETE;
	break;
	case PMSelection::ATTR_NAME:
	    return PkgAttributeValue (obj_r->name());
	break;
	case PMSelection::ATTR_VERSION:
	    return PkgAttributeValue (obj_r->version());
	break;
	case PMSelection::ATTR_RELEASE:
	    return PkgAttributeValue (obj_r->release());
	break;
	case PMSelection::ATTR_ARCH:
	    return PkgAttributeValue (obj_r->arch());
	break;

	case PMSelection::ATTR_SUMMARY:
	    return attr_SUMMARY;
	break;

	case PMSelection::ATTR_CATEGORY:
	    return attr_CATEGORY;
	break;
	case PMSelection::ATTR_ARCHIVESIZE:
	    return attr_ARCHIVESIZE;
	break;
	case PMSelection::ATTR_SIZE:
	    return attr_SIZE;
	break;
	case PMSelection::ATTR_VISIBLE:
	    return attr_VISIBLE;
	break;

	default:
	    return PkgAttributeValue();
    }

    if (pos_ptr->begin >= pos_ptr->end)
	return PkgAttributeValue ();

    std::list<std::string> value;

    if (!_selection_retrieval->retrieveData (*pos_ptr, value))
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
//	METHOD NAME : PMULSelectionDataProvider::getAttributeValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION : Object attribute retrieval.
//
PkgAttributeValue
PMULSelectionDataProvider::getAttributeValue( constPMObjectPtr obj_r,
					PMObject::PMObjectAttribute attr )
{
    return getValue (obj_r, (PMSelection::PMSelectionAttribute)attr);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULSelectionDataProvider::getAttributeValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION :Selection attribute retrieval.
//
PkgAttributeValue
PMULSelectionDataProvider::getAttributeValue( constPMSelectionPtr pkg_r,
					PMSelection::PMSelectionAttribute attr )
{
    return getValue (pkg_r, (PMSelection::PMSelectionAttribute)attr);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULSelectionDataProvider::setAttributeValue
//	METHOD TYPE : attribute set
//
//	DESCRIPTION : inject some object attribute by value
//
void
PMULSelectionDataProvider::setAttributeValue(
	PMSelectionPtr pkg, PMSelection::PMSelectionAttribute attr,
	const PkgAttributeValue& value)
{
    D__ << "PMULSelectionDataProvider::setAttributeValue (" << pkg->getAttributeName(attr) << ")" << endl;

    switch (attr)
    {
	case PMSelection::ATTR_NAME:	  attr_NAME = value; break;
	case PMSelection::ATTR_VERSION:	  attr_VERSION = value; break;
	case PMSelection::ATTR_RELEASE:	  attr_RELEASE = value; break;
	case PMSelection::ATTR_ARCH:	  attr_ARCH = value; break;

	case PMSelection::ATTR_SUMMARY:	    attr_SUMMARY = value; break;
	case PMSelection::ATTR_ARCHIVESIZE: attr_ARCHIVESIZE = value; break;
	case PMSelection::ATTR_SIZE:	    attr_SIZE = value; break;
	case PMSelection::ATTR_CATEGORY:    attr_CATEGORY = value; break;
	case PMSelection::ATTR_VISIBLE:	    attr_VISIBLE = value; break;

	default:
	    break;
    }

    return;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULSelectionDataProvider::setAttributeValue
//	METHOD TYPE : attribute set
//
//	DESCRIPTION :inject some package attribute by file offset
//
void
PMULSelectionDataProvider::setAttributeValue(
	PMSelectionPtr pkg, PMSelection::PMSelectionAttribute attr,
	std::streampos begin, std::streampos end)
{
    D__ << "PMULSelectionDataProvider::setAttributeValue (" << pkg->getAttributeName(attr) << ")" << endl;

#define SET_RETRIEVAL(name,start,stop) \
	do { attr_##name.begin = start; attr_##name.end = stop; } while (0)

    switch (attr)
    {
	case PMSelection::ATTR_REQUIRES:  SET_RETRIEVAL (REQUIRES,begin,end); break;
	case PMSelection::ATTR_PROVIDES:  SET_RETRIEVAL (PROVIDES,begin,end); break;
	case PMSelection::ATTR_CONFLICTS: SET_RETRIEVAL (CONFLICTS,begin,end); break;
	case PMSelection::ATTR_OBSOLETES: SET_RETRIEVAL (OBSOLETES,begin,end); break;
	case PMSelection::ATTR_INSTALL:	  SET_RETRIEVAL (INSTALL,begin,end); break;
	case PMSelection::ATTR_DELETE:	  SET_RETRIEVAL (DELETE,begin,end); break;
	break;

#undef SET_RETRIEVAL
	default:
	    break;
    }

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
