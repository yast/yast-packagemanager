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

   File:       Query.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/Query.h>
#include <y2pm/PMManager.h> 
using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Query
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER( Query );


//-----------------------------------------------------------------
// private

bool
Query::checkSelectable (PMSelectablePtr selectable)
{
    return false;
}

bool
Query::checkPackage (PMPackagePtr package)
{
    return false;
}

bool
Query::checkSelection (PMSelectionPtr selection)
{
    return false;
}

//-----------------------------------------------------------------
// public


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Query::Query
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
Query::Query()
    : _query (0)
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Query::~Query
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
Query::~Query()
{
    if (_query)
	QueryParser::free_node (_query);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Query::parseQuery (const std::string& query)
//	METHOD TYPE : PMError
//
//	DESCRIPTION : parse textual query to filter
//
PMError
Query::parseQuery (const std::string& query)
{
    QueryParser parser;
    return parser.parseQuery (query, _errpos, &_query);
}


const std::list<PMSelectablePtr>
Query::querySelectables (const PMManager& packageManager)
{
    std::list<PMSelectablePtr> matches;
    for (PMManager::PMSelectableVec::const_iterator it = packageManager.begin();
	 it != packageManager.end(); ++it)
    {
	if (checkSelectable (*it))
	    matches.push_back (*it);
    }
    return matches;
}

const std::list<PMPackagePtr>
Query::queryPackages (const PMManager& packageManager)
{
    return std::list<PMPackagePtr>();
}

const std::list<PMSelectionPtr>
Query::querySelections (const PMManager& selectionManager)
{
    return std::list<PMSelectionPtr>();
}

