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
//	DESCRIPTION : store selection Pathname
//		      for later value retrieval on-demand
//
PMULSelectionDataProvider::PMULSelectionDataProvider(const Pathname &selectionname)
{
    _selection_retrieval = new TagCacheRetrieval (selectionname);
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
    delete _selection_retrieval;
}

const std::string
PMULSelectionDataProvider::summary(const std::string& lang) const
{
    std::string value;
    _selection_retrieval->retrieveData (_attr_SUMMARY, value);
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

const std::list<std::string>
PMULSelectionDataProvider::inspacks(const std::string& lang) const
{
    std::list<std::string> value;
    _selection_retrieval->retrieveData (_attr_INSPACKS, value);
    return value;
}

const std::list<std::string>
PMULSelectionDataProvider::delpacks(const std::string& lang) const
{
    std::list<std::string> value;
    _selection_retrieval->retrieveData (_attr_DELPACKS, value);
    return value;
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
    switch (attr)
    {
	case PMSelection::ATTR_CATEGORY:	return PkgAttributeValue (category());
	break;
	case PMSelection::ATTR_VISIBLE:		return PkgAttributeValue (_attr_VISIBLE?"true":"false");
	break;
	case PMSelection::ATTR_SUGGESTS:	return PkgAttributeValue (suggests());
	break;
	case PMSelection::ATTR_INSPACKS:	return PkgAttributeValue (inspacks());
	break;
	case PMSelection::ATTR_DELPACKS:	return PkgAttributeValue (delpacks());
	break;
	case PMSelection::ATTR_ARCHIVESIZE:	return PkgAttributeValue (_attr_ARCHIVESIZE.asString());
	break;
	case PMSelection::PMSEL_NUM_ATTRIBUTES:
	break;
    }

    return PkgAttributeValue ();
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
#if 0
	case PMSelection::ATTR_NAME:	  attr_NAME = value; break;
	case PMSelection::ATTR_VERSION:	  attr_VERSION = value; break;
	case PMSelection::ATTR_RELEASE:	  attr_RELEASE = value; break;
	case PMSelection::ATTR_ARCH:	  attr_ARCH = value; break;

	case PMSelection::ATTR_SUMMARY:	    attr_SUMMARY = value; break;
	case PMSelection::ATTR_ARCHIVESIZE: attr_ARCHIVESIZE = value; break;
	case PMSelection::ATTR_SIZE:	    attr_SIZE = value; break;
	case PMSelection::ATTR_CATEGORY:    attr_CATEGORY = value; break;
	case PMSelection::ATTR_VISIBLE:	    attr_VISIBLE = value; break;
#endif
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
#if 0
	case PMSelection::ATTR_REQUIRES:  SET_RETRIEVAL (REQUIRES,begin,end); break;
	case PMSelection::ATTR_PROVIDES:  SET_RETRIEVAL (PROVIDES,begin,end); break;
	case PMSelection::ATTR_CONFLICTS: SET_RETRIEVAL (CONFLICTS,begin,end); break;
	case PMSelection::ATTR_OBSOLETES: SET_RETRIEVAL (OBSOLETES,begin,end); break;
	case PMSelection::ATTR_INSTALL:	  SET_RETRIEVAL (INSTALL,begin,end); break;
	case PMSelection::ATTR_DELETE:	  SET_RETRIEVAL (DELETE,begin,end); break;
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
