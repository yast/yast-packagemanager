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

#include <y2pm/PMPackage.h>
#include <y2pm/PMSelectable.h>

#include <y2pm/Query.h>

using namespace std;

#define MASK_NONE	((Query::BitMask)0x00)
#define MASK_INSTALLED	((Query::BitMask)0x01)
#define MASK_AVAILABLE	((Query::BitMask)~0x01)
#define FIRST_AVAILABLE ((Query::BitMask)0x02)

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Query
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER( Query );


//-----------------------------------------------------------------
// private

bool
Query::checkPackage (PMPackagePtr package, struct qnode *query)
{
    return false;
}

Query::BitMask
Query::findCandidate (PMSelectablePtr selectable)
{
    Query::BitMask ret = MASK_NONE;

    if (!selectable->has_candidate())
	return ret;

    Query::BitMask current = FIRST_AVAILABLE;
    PMPackagePtr candidate = selectable->candidateObj();
    for (PMSelectable::PMObjectList::const_iterator it = selectable->av_begin();
	 it != selectable->av_end(); ++it)
    {
	if (candidate == *it)
	{
	    ret = current;
	    break;
	}
	current <<= 1;
    }
    return ret;
}

Query::BitMask
Query::checkSelectableFlag (PMSelectablePtr selectable, int flag)
{
    MIL << "Query::checkSelectableFlag (" << flag << endl;

    Query::BitMask ret = MASK_NONE;
    switch (flag)
    {
	case 1:			//    { "isInstalled",	1 },
	{
	    if (selectable->installedObj())
	    {
		ret = MASK_INSTALLED;
	    }
	}
	break;
	case 2:			//    { "isAvailable",	2 },
	{
	    Query::BitMask current = FIRST_AVAILABLE;
	    for (PMSelectable::PMObjectList::const_iterator it = selectable->av_begin();
		 it != selectable->av_end(); ++it)
	    {
		ret |= current;
		current <<= 1;
	    }
	}
	break;
	case 3:			//    { "isSelected",	3 },
	{
	    if (selectable->to_install())
	    {
		ret = findCandidate (selectable);
	    }
	}
	case 4:			//    { "isDeleted",	4 },
	{
	    if (selectable->to_delete()
		&& selectable->has_installed())
	    {
		ret = MASK_INSTALLED;
	    }
	}
	case 5:			//    { "isUpdated",	5 },
	{
	    if (selectable->to_install()
		&& selectable->has_installed()
		&& selectable->has_candidate())
	    {
		ret = findCandidate (selectable);
	    }
	}
	case 6:			//    { "isTaboo",	6 },
	{
	    if (selectable->is_taboo()
		&& (selectable->has_candidate()))
	    {
		ret = findCandidate (selectable);
	    }
	}
	break;
	case 7:			//    { "isAutomatic",	7 },
	{
	    if (selectable->status() == PMSelectable::S_Auto
		&& selectable->has_candidate())
	    {
		ret = findCandidate (selectable);
	    }
	}
	case 8:			//    { "isIncomplete",	8 },
	break;
	default:
	break;
    }
    MIL << "ret " << ret << endl;
    return ret;
}


Query::BitMask
Query::checkSelectableOpCompare (PMSelectablePtr selectable, struct qnode *query)
{
    Query::BitMask ret = MASK_NONE;

    return ret;
}

Query::BitMask
Query::checkSelectableOpBoolean (PMSelectablePtr selectable, struct qnode *query)
{
    Query::BitMask left = checkSelectableValue (selectable, &(query->left));

    if ((query->op == OP_AND)
	&& (left == MASK_NONE))
    {
	return left;
    }

    Query::BitMask right = checkSelectableValue (selectable, &(query->right));
    if (query->op == OP_AND)
    {
	return left & right;
    }
    if (query->op == OP_OR)
    {
	return left | right;
    }
    ERR << "Query::checkSelectableOpBoolean non boolean " << query->op << endl;
    return MASK_NONE;
}


Query::BitMask
Query::checkSelectableValue (PMSelectablePtr selectable, struct qvalue *value)
{
    MIL << "Query::checkSelectableValue ()" << endl;
    Query::BitMask ret = MASK_NONE;
    switch (value->type)
    {
	case QTYPE_FLAG:
	    ret = checkSelectableFlag (selectable, value->v.attr);
	break;
	case QTYPE_ATTR:
	    MIL << "QTYPE_ATTR ?!" << endl;
	break;
	case QTYPE_CONST:
	    MIL << "QTYPE_CONST ?!" << endl;
	break;
	case QTYPE_NODE:
	    ret = checkSelectable (selectable, value->v.node);
	break;
	case QTYPE_NONE:
	    MIL << "QTYPE_NONE ?!" << endl;
	break;
    }
    MIL << "ret " << ret << endl;
    return ret;
}


Query::BitMask
Query::checkSelectable (PMSelectablePtr selectable, struct qnode *query)
{
    Query::BitMask ret = MASK_NONE;
    if (query->op > OP_BOOLEAN)
    {
	ret = checkSelectableOpCompare (selectable, query);
    }
    else if (query->op > OP_UNARY)
    {
	ret = checkSelectableOpBoolean (selectable, query);
    }
    else if (query->op == OP_NOT)
    {
	ret = ~checkSelectableValue (selectable, &(query->left));
    }
    else if (query->op == OP_FLAG)
    {
	if (query->left.type != QTYPE_ATTR)
	{
	    ERR << "op flag but type " << query->left.type << " != attr" << endl;
	}
	else
	{
	    ret = checkSelectableFlag (selectable, query->left.v.attr);
	}
    }
    return ret;
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
    , _initial (true)
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
Query::queryPackage (const std::string& query)
{
    QueryParser parser;
    PMError err = parser.parseQuery (query, _errpos, &_query);
    if (err == PMError::E_ok)
    {
	_initial = true;
	_backlog = MASK_NONE;
    }
    else
    {
	_query = 0;
	_backlog = MASK_NONE;
    }
    return err;
}


const PMPackagePtr
Query::nextPackage (const PMPackageManager& packageManager)
{
    PMPackagePtr package;

    // no query -> exit
    if (_query == 0)
    {
	return package;
    }

    // first call to nextPackage(), initialize
    if (_initial)
    {
	_current = packageManager.begin();
	_initial = false;
    }

    // no bits in backlog -> check selectables
    if (_backlog == MASK_NONE)
    {
	while (_current != packageManager.end())
	{
	    _backlog = checkSelectable (*_current, _query);

	    if (_backlog != MASK_NONE)			// Hit !
	    {
		++_current;
		_logpos = MASK_INSTALLED;
		break;
	    }
	}
    }

    // anything found ?

    if (_backlog != MASK_NONE)
    {
	// find next bit

	while ((_logpos != MASK_NONE)
		&& ((_backlog & _logpos) == MASK_NONE))
	{
	    _logpos <<= 1;
	}

	// no more found but _backlog still set ?!

	if (_logpos == MASK_NONE)
	{
	    ERR << "_backlog " << _backlog << " not found" << endl;
	}
	else if (_logpos == MASK_INSTALLED)
	{
	    // return installed
	    package = (*_current)->installedObj();
	    _backlog &= ~_logpos;
	}
	else
	{
	    // an available
	    Query::BitMask av_mask = FIRST_AVAILABLE;
	    for (PMSelectable::PMObjectList::const_iterator it = (*_current)->av_begin();
		 it != (*_current)->av_end(); ++it)
	    {
		if (_backlog & av_mask)
		{
		    _logpos = av_mask;
		    _backlog &= ~_logpos;
		    package = *it;
		    break;
		}
	   }
	}
    }
    return package;
}

