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
#include <y2util/Date.h>
#include <y2util/FSize.h>

#include <y2pm/PMPackage.h>
#include <y2pm/PMSelectable.h>
#include <y2pm/PMSelectablePtr.h>

#include <y2pm/Query.h>
#include <y2pm/querycodes.h>

using namespace std;

#define MASK_NONE	((Query::BitMask)0x00)
#define MASK_INSTALLED	((Query::BitMask)0x01)
#define MASK_AVAILABLE	((Query::BitMask)~0x01)
#define FIRST_AVAILABLE ((Query::BitMask)0x02)

#define QTYPE_NONE	0
#define QTYPE_LIST	1
#define QTYPE_STRING	2
#define QTYPE_DATE	3
#define QTYPE_VERSION	4
#define QTYPE_SIZE	5

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Query
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER( Query );


//-----------------------------------------------------------------
// private

/**
 * findCandidate()
 *
 * find bitmask for candidate
 */
Query::BitMask
Query::findCandidate (PMSelectablePtr selectable)
{
    Query::BitMask ret = MASK_NONE;
MIL << "findCandidate()" << endl;
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



/**
 * findByMask ()
 *
 * I: selectable
 * IO: mask, I: start of search, O: result of search (0 if no package found)
 *
 * find package by bitmask from selectable
 * return package and mask matching the package
 */
PMPackagePtr
Query::findByMask (PMSelectablePtr selectable, Query::BitMask& mask_r)
 {
    if (mask_r == 0)
	return PMPackagePtr();

    if (mask_r == MASK_INSTALLED)
    {
	if (!selectable->installedObj())
	    mask_r = 0;
	return selectable->installedObj();
    }

    Query::BitMask current = FIRST_AVAILABLE;
    for (PMSelectable::PMObjectList::const_iterator it = selectable->av_begin();
	 it != selectable->av_end(); ++it)
    {
	if (mask_r == current)
	    return (*it);
	current <<= 1;
    }
    mask_r = 0;
    return PMPackagePtr();
}



/**
 * determine type of value
 */
int
Query::typeOfValue (struct qvalue *value, int type_hint,
		PMSelectablePtr selectable, Query::BitMask& mask_r,
		std::list<std::string>& vlist, std::string & vstring,
		Date& vdate, PkgEdition& vversion, FSize& vsize)
{
    static void *last_ptr = 0;
    static Query::BitMask last_mask = 0;

    const void *now_ptr = selectable;

    PMPackagePtr package;

    if (now_ptr != last_ptr)
    {
	last_ptr = (void *)now_ptr;
	last_mask = MASK_INSTALLED;
    }
    else
    {
	last_mask <<= 1;
    }

    package = findByMask (selectable, last_mask);
    mask_r = last_mask;

    int type = QTYPE_NONE;

    if (!package)
	return type;

    if (value->type == QTYPE_ATTR)
    {
//MIL << "typeOfValue QTYPE_ATTR" << endl;
	switch (value->v.attr)
	{
	    case QCODE_VERSION:
	    {
		type = QTYPE_VERSION;
		vversion = PkgEdition::toString (package->edition());
	    }
	    break;
	    case QCODE_VENDOR:
	    {
		type = QTYPE_STRING;
		PMPackagePtr package = package;
		vstring = package->vendor();
	    }
	    break;
	    case QCODE_GROUP:
	    {
		type = QTYPE_STRING;
		PMPackagePtr package = package;
		vstring = package->group();
	    }
	    break;
	    case QCODE_NAME:
	    {
		type = QTYPE_STRING;
		vstring = package->name();
	    }
	    break;
	    case QCODE_REQUIRES:
	    {
		type = QTYPE_LIST;
		vlist = PMSolvable::PkgRelList2StringList (package->requires());
	    }
	    break;
	    case QCODE_PROVIDES:
	    {
		type = QTYPE_LIST;
		vlist = PMSolvable::PkgRelList2StringList (package->provides());
	    }
	    break;
	    case QCODE_CONFLICTS:
	    {
		type = QTYPE_LIST;
		vlist = PMSolvable::PkgRelList2StringList (package->conflicts());
	    }
	    break;
	    case QCODE_INSTALLDATE:
	    {
		if (mask_r == MASK_INSTALLED)
		{
		    vdate = package->installtime();
		    type = QTYPE_DATE;
		}
		else
		{
		    package = PMPackagePtr();
		    mask_r = 0;
		}
	    }
	    break;
	    case QCODE_BUILDDATE:
	    {
		type = QTYPE_DATE;
		vdate = package->buildtime();
	    }
	    break;
	    case QCODE_SELECTION:
	    {
		type = QTYPE_STRING;
		vstring = "";
	    }
	    break;
	    case QCODE_PATCH:
	    {
		type = QTYPE_STRING;
		vstring = "";
	    }
	    break;
	    case QCODE_FILES:
	    {
		PMPackagePtr package = selectable->installedObj();
		if (!package) break;
		type = QTYPE_LIST;
		vlist = package->filenames();
	    }
	    break;
	    case QCODE_MIME:
	    {
		type = QTYPE_STRING;
		vstring = "";
	    }
	    break;
	    case QCODE_KEYWORDS:
	    {
		type = QTYPE_LIST;
	    }
	    break;
	    case QCODE_SUPPORTS:
	    {
		type = QTYPE_LIST;
	    }
	    break;
	    case QCODE_NEEDS:
	    {
		type = QTYPE_LIST;
	    }
	    break;
	    case QCODE_SUMMARY:
	    {
		type = QTYPE_STRING;
		vstring = package->summary();
	    }
	    break;
	    case QCODE_DESCRIPTION:
	    {
		type = QTYPE_LIST;
		vlist = package->description();
	    }
	    break;
	    case QCODE_FROM:
	    {
		type = QTYPE_STRING;
		vstring = "";
	    }
	    break;
	    case QCODE_IVERSION:
	    {
		if (mask_r == MASK_INSTALLED)
		{
		    vversion = package->edition();
		    type = QTYPE_VERSION;
		}
		else
		{
		    package = PMPackagePtr();
		    mask_r = 0;
		}
	    }
	    break;
	    case QCODE_AVERSION:
	    {
		if ((mask_r != MASK_INSTALLED)
		    && (mask_r != 0))
		{
		    vversion = package->edition();
		    type = QTYPE_VERSION;
		}
		else
		{
		    package = PMPackagePtr();
		    mask_r = 0;
		}
	    }
	    break;
	    case QCODE_CVERSION:
	    {
		PMPackagePtr candidate = selectable->candidateObj();
		if (!candidate) break;
		type = QTYPE_VERSION;
		vversion = candidate->edition();
		last_mask = mask_r = findCandidate (selectable);
	    }
	    break;
	    case QCODE_EMPTY:
	    {
		type = QTYPE_STRING;
		vstring = "";
	    }
	    break;
	    default:
	    break;
	}
    }
    else if (value->type == QTYPE_CONST)
    {
//MIL << "typeOfValue QTYPE_CONST, hint " << type_hint << endl;
	switch (type_hint)
	{
	    case QTYPE_NONE:
		vstring = value->v.constant;
		vlist.push_back (vstring);
		vdate = Date (vstring);
		vversion = PkgEdition (PkgEdition::fromString(vstring));
		vsize = FSize (atoll(value->v.constant));
	    break;
	    case QTYPE_LIST:
		vlist.push_back (value->v.constant);
		type = type_hint;
	    break;
	    case QTYPE_STRING:
		vstring = value->v.constant;
		type = type_hint;
	    break;
	    case QTYPE_DATE:
		vdate = Date (vstring);
		type = type_hint;
	    break;
	    case QTYPE_VERSION:
		vversion = PkgEdition (PkgEdition::fromString(vstring));
		type = type_hint;
	    break;
	    case QTYPE_SIZE:
		vsize = FSize (atoll(value->v.constant));
		type = type_hint;
	    break;
	    default:
	    break;
	}
    }
//MIL << "typeOfValue " << type << endl;

    return type;
}

/**
 * checkPackage
 *
 * check package against query
 */
bool
Query::checkPackage (PMPackagePtr package, struct qnode *query)
{
    return false;
}


/**
 * checkSelectableFlag
 *
 * check if 'flag' is valid for packages of selectable
 * return BitMask of matching packages
 */
Query::BitMask
Query::checkSelectableFlag (PMSelectablePtr selectable, int flag)
{
    MIL << "Query::checkSelectableFlag (" << flag << endl;

    Query::BitMask ret = MASK_NONE;
    switch (flag)
    {
	case QCODE_ISINSTALLED:
	{
	    if (selectable->installedObj())
	    {
		ret = MASK_INSTALLED;
	    }
	}
	break;
	case QCODE_ISAVAILABLE:
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
	case QCODE_TOINSTALL:
	{
	    if (selectable->to_install())
	    {
		ret = findCandidate (selectable);
	    }
	}
	case QCODE_TODELETE:
	{
	    if (selectable->to_delete()
		&& selectable->has_installed())
	    {
		ret = MASK_INSTALLED;
	    }
	}
	case QCODE_TOUPDATE:
	{
	    if (selectable->to_install()
		&& selectable->has_installed()
		&& selectable->has_candidate())
	    {
		ret = findCandidate (selectable);
	    }
	}
	case QCODE_ISTABOO:
	{
	    if (selectable->is_taboo()
		&& (selectable->has_candidate()))
	    {
		ret = findCandidate (selectable);
	    }
	}
	break;
	case QCODE_ISAUTOMATIC:
	{
	    if (selectable->to_modify()
		&& !selectable->by_user())
	    {
		ret = findCandidate (selectable);
	    }
	}
	case QCODE_ISINCOMPLETE:
	break;
	case QCODE_ISCANDIDATE:
	{
		ret = findCandidate (selectable);
	}
	break;
	default:
	break;
    }
//    MIL << "ret " << ret << endl;
    return ret;
}


/**
 * compareVersion
 *
 * compare versions
 */
bool
Query::compareVersion (const PkgEdition& left, enum operation op, const PkgEdition& right)
{
    bool ret = false;
MIL << "compareVersion(" << left << "," << right << ")" << endl;
    switch (op)
    {
	case OP_EQ:
	    ret = (left == right);
	break;
	case OP_NEQ:
	    ret = (left != right);
	break;
	case OP_GT:
	    ret = (left > right);
	break;
	case OP_GE:
	    ret = (left >= right);
	break;
	case OP_LT:
	    ret = (left < right);
	break;
	case OP_LE:
	    ret = (left <= right);
	default:
	break;
    }
    return ret;
}

/**
 * compareString
 *
 * compare string
 */
bool
Query::compareString (const std::string& left, enum operation op, const std::string& right)
{
    bool ret = false;
//MIL << "compareString(" << left << "," << right << ")" << endl;

    switch (op)
    {
	case OP_EQ:
	    ret = (left == right);
	break;
	case OP_NEQ:
	    ret = (left != right);
	break;
	case OP_GT:
	    ret = (left > right);
	break;
	case OP_GE:
	    ret = (left >= right);
	break;
	case OP_LT:
	    ret = (left < right);
	break;
	case OP_LE:
	    ret = (left <= right);
	default:
	break;
    }
    return ret;
}

/**
 * compareStringList
 *
 * compare string list
 */
bool
Query::compareStringList (const std::list<std::string>& left, enum operation op, const std::string& right)
{
    bool ret = false;
//MIL << "compareStringList(," << right << endl;

    switch (op)
    {
	case OP_EQ:
	{
	    if (left.empty())
	    {
		if (right.empty())
		    ret = true;
	    }
	    else
	    {
		ret = (left.front() == right);
	    }
	}
	break;
	case OP_NEQ:
	{
	    if (left.empty())
	    {
		if (!right.empty())
		    ret = true;
	    }
	    else
	    {
		if (right.empty())
		    ret = true;
		else
		    ret = (left.front() != right);
	    }
	}
	break;
	case OP_CONTAINS:
	{
	    if (left.empty() || right.empty())
		break;
	    for (std::list<std::string>::const_iterator it = left.begin();
	    	 it != left.end(); ++it)
	    {
		if (*it == right)
		{
		    ret = true;
		    break;
		}
	    }
	}
	break;
	default:
	break;
    }
    return ret;
}

/**
 * compareDate
 *
 * compare date
 */
bool
Query::compareDate (const Date& left, enum operation op, const Date& right)
{
    bool ret = false;

//MIL << "compareDate" << left << "," << right << ")" << endl;
    switch (op)
    {
	case OP_EQ:
	    ret = ((time_t)left == (time_t)right);
	break;
	case OP_NEQ:
	    ret = ((time_t)left != (time_t)right);
	break;
	case OP_GT:
	    ret = ((time_t)left > (time_t)right);
	break;
	case OP_GE:
	    ret = ((time_t)left >= (time_t)right);
	break;
	case OP_LT:
	    ret = ((time_t)left < (time_t)right);
	break;
	case OP_LE:
	    ret = ((time_t)left <= (time_t)right);
	default:
	break;
    }
    return ret;
}

/**
 * compareSize
 *
 * compare size
 */
bool
Query::compareSize (const FSize& left, enum operation op, const FSize& right)
{
    bool ret = false;

//MIL << "compareSize" << left << "," << right << ")" << endl;
    switch (op)
    {
	case OP_EQ:
	    ret = ((long long)left == (long long)right);
	break;
	case OP_NEQ:
	    ret = ((long long)left != (long long)right);
	break;
	case OP_GT:
	    ret = ((long long)left > (long long)right);
	break;
	case OP_GE:
	    ret = ((long long)left >= (long long)right);
	break;
	case OP_LT:
	    ret = ((long long)left < (long long)right);
	break;
	case OP_LE:
	    ret = ((long long)left <= (long long)right);
	default:
	break;
    }
    return ret;
}

/**
 * checkSelectableOpCompare
 *
 * check packages of selectable for matching compare
 */
Query::BitMask
Query::checkSelectableOpCompare (PMSelectablePtr selectable, struct qnode *query)
{
    Query::BitMask result = 0;

    for (;;)
    {

    Query::BitMask lmask;
    std::list<std::string> llist;
    std::string lstring;
    Date ldate;
    PkgEdition lversion;
    FSize lsize;
    // find type
    int ltype = typeOfValue (&(query->left), QTYPE_NONE, selectable, lmask, llist, lstring, ldate, lversion, lsize);
    if ((ltype == QTYPE_NONE)
	&& (lmask == 0))
    {
	break;
    }

    Query::BitMask rmask;
    std::list<std::string> rlist;
    std::string rstring;
    Date rdate;
    PkgEdition rversion;
    FSize rsize;
    int rtype = typeOfValue (&(query->right), ltype, selectable, rmask, rlist, rstring, rdate, rversion, rsize);
    if ((rtype == QTYPE_NONE)
	&& (rmask == 0))
    {
	break;
    }

    Query::BitMask ret = MASK_NONE;

    switch (ltype)
    {
	case QTYPE_LIST:
	    if (rtype == QTYPE_STRING)
		ret = compareStringList (llist, query->op, rstring);
	    else
		ERR << "ltype != rtype";
	break;
	case QTYPE_STRING:
	    if (rtype == ltype)
		ret = compareString (lstring, query->op, rstring);
	    else
		ERR << "ltype != rtype";
	break;
	case QTYPE_DATE:
	    if (rtype == ltype)
		ret = compareDate (ldate, query->op, rdate);
	    else
		ERR << "ltype != rtype";
	break;
	case QTYPE_VERSION:
	    if (rtype == ltype)
		ret = compareVersion (lversion, query->op, rversion);
	    else
		ERR << "ltype != rtype";
	break;
	case QTYPE_SIZE:
	    if (rtype == ltype)
		ret = compareSize (lsize, query->op, rsize);
	    else
		ERR << "ltype != rtype";
	break;
	default:
	break;
    }

    result |= ret;
    } // for()
if (result)
MIL << "checkSelectableOpCompare " << (void *)result << endl;
    return result;
}

/**
 * checkSelectableOpBoolean
 *
 * check packages of selectable for matching && or ||
 */
Query::BitMask
Query::checkSelectableOpBoolean (PMSelectablePtr selectable, struct qnode *query)
{
//MIL << "Query::checkSelectableOpBoolean()" << endl;
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


/**
 * checkSelectableValue
 *
 * check packages of selectable for matching qvalue
 */
Query::BitMask
Query::checkSelectableValue (PMSelectablePtr selectable, struct qvalue *value)
{
//    MIL << "Query::checkSelectableValue ()" << endl;
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


/**
 * checkSelectable
 *
 * check packages of selectable for matching query
 */
Query::BitMask
Query::checkSelectable (PMSelectablePtr selectable, struct qnode *query)
{
//MIL << "Query::checkSelectable(" << query << ")" << endl;
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
	if (query->left.type != QTYPE_FLAG)
	{
	    ERR << "op flag but type " << query->left.type << " != flag" << endl;
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
    MIL << "queryPackage (" << query << ")" << endl;

    QueryParser parser;
    PMError err = parser.parseQuery (query, _errpos, &_query);
    if (err == PMError::E_ok)
    {
	MIL << "Ok" << endl;
	_initial = true;
	_backlog = MASK_NONE;
    }
    else
    {
	MIL << "Fail" << endl;
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
	MIL << "initial nextPackage" << endl;
	_current = packageManager.begin();
	_initial = false;
    }
    else if (_current != packageManager.end())
    {
	++_current;
	MIL << "nextPackage" << endl;
    }

    // no bits in backlog -> check selectables
    if (_backlog == MASK_NONE)
    {
	MIL << "no backlog" << endl;
	while (_current != packageManager.end())
	{
	    _backlog = checkSelectable (*_current, _query);

	    if (_backlog != MASK_NONE)			// Hit !
	    {
		MIL << "hit !:" << (void *)_backlog << endl;
		_logpos = MASK_INSTALLED;		// initial mask
		break;
	    }
	    ++_current;
	}
    }

    // anything found ?

    if (_backlog != MASK_NONE)
    {
	MIL << "backlog! " << (void *)_backlog << endl;

	// find next bit

	while ((_logpos != MASK_NONE)
		&& ((_backlog & _logpos) == MASK_NONE))
	{
	    _logpos <<= 1;
	}

	MIL << "_logpos: " << (void *)_logpos << endl;

	// no more found but _backlog still set ?!

	if (_logpos == MASK_NONE)
	{
	    ERR << "_backlog " << (void *)_backlog << " not found" << endl;
	}
	else if (_logpos == MASK_INSTALLED)
	{
	    // return installed
	    package = (*_current)->installedObj();
	    MIL << "installed !" << *_current << ":" << package << endl;
	    _backlog &= ~_logpos;
	}
	else
	{
	    MIL << "available !" << endl;
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
		av_mask <<= 1;
	   }
	}
    }
    MIL << "return " << package << endl;
    return package;
}

