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

   File:       PMObject.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/PMObject.h>
#include <y2pm/PMSelectable.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMObject
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(PMObject);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::PMObject
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMObject::PMObject( const PkgName &    name_r,
		    const PkgEdition & edition_r )
    : PMSolvable(name_r, edition_r)
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::~PMObject
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMObject::~PMObject()
{
}

string PMObject::getAttributeName(PMSolvableAttribute attr)
{
    const char* str = NULL;
    switch(attr)
    {
	case ATTR_NAME:
	    str = "NAME";
	    break;
	case ATTR_VERSION:
	    str = "VERSION";
	    break;
	case ATTR_RELEASE:
	    str = "RELEASE";
	    break;
	case ATTR_REQUIRES:
	    str = "REQUIRES";
	    break;
	case ATTR_PREREQUIRES:
	    str = "PREREQUIRES";
	    break;
	case ATTR_PROVIDES:
	    str = "PROVIDES";
	    break;
	case ATTR_OBSOLETES:
	    str = "OBSOLETES";
	    break;
	case ATTR_CONFLICTS:
	    str = "CONFLICTS";
	    break;
	case PMSLV_NUM_ATTRIBUTES:
	    // invalid
	    return "invalid query";
    }

    if(!str)
    {
	ERR << "invalid enum value" << endl;
	str = "invalid query";
    }

    return str;
}

string PMObject::getAttributeName(PMObjectAttribute attr)
{
    const char* str = NULL;
    switch(attr)
    {
	case ATTR_DESCRIPTION:
	    str = "DESCRIPTION";
	    break;
	case ATTR_SUMMARY:
	    str = "SUMMARY";
	    break;
	case ATTR_SIZE:
	    str = "SIZE";
	    break;
	case PMOBJ_NUM_ATTRIBUTES:
	    // invalid
	    return "invalid query";
    }

    if(!str)
    {
	ERR << "invalid enum value" << endl;
	str = "invalid query";
    }

    return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMObject::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str )
    << '(' << (_selectable ? "MANAGED" : "unmanaged" ) << ')'
    << '(' << _name << '-' << _edition.version() << '-' << _edition.release() << ')';
  return str;
}

///////////////////////////////////////////////////////////////////
//
// Shortcuts for UI
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMObject::isInstalledObj
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMObject::isInstalledObj() const
{
  return( hasSelectable() && _selectable->installedObj() == this );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : PMObject::isCandidateObj
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMObject::isCandidateObj() const
{
  return( hasSelectable() && _selectable->candidateObj() == this );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : PMObject::isAvailableOnly
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMObject::isAvailableOnly() const
{
  return( hasSelectable()
	  && _selectable->installedObj() != this
	  && _selectable->candidateObj() != this );
}






