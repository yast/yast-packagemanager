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

   File:       Query.h

   Author:     Klaus Kaempf <kkaempf@suse.de>
   Maintainer: Klaus Kaempf <kkaempf@suse.de>

/-*/
#ifndef Query_h
#define Query_h

#include <iosfwd>
#include <string>
#include <list>

#include <y2util/RepDef.h>
#include <y2pm/QueryPtr.h>
#include <y2pm/QueryParser.h>
#include <y2pm/QueryNode.h>
#include <y2pm/QueryError.h>

#include <y2pm/PMSelectablePtr.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMPackageManager.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Query
/**
 * @short Interface class for queries
 **/
class Query : virtual public Rep {
    REP_BODY(Query);

    private:
	/**
	 * bitmask for marking matches in selectable
	 * Bit 0: installed matches
	 * Bit 1..: available n matches
	 */
	typedef unsigned int BitMask;

	struct qnode *_query;

	/**
	 * position of error in query string
	 */
	int _errpos;

	/**
	 * just called queryPackackage, following nextPackage()
	 * is initial
	 */
	bool _initial;

	/**
	 * current iterator for nextPackage()
	 */
	PMManager::PMSelectableVec::const_iterator _current;

	/**
	 * backlog of packages still to be retrieved via nextPackage()
	 */
	BitMask _backlog;	// all bits
	BitMask _logpos;	// mask of last returned package

	/**
	 * check query against selectable
	 */
	BitMask checkSelectable (PMSelectablePtr selectable, struct qnode *query);

	/**
	 * check query against package
	 */
	bool checkPackage (PMPackagePtr package, struct qnode *query);

	BitMask findCandidate (PMSelectablePtr selectable);
	BitMask checkSelectableFlag (PMSelectablePtr selectable, int flag);
	BitMask checkSelectableOpCompare (PMSelectablePtr selectable, struct qnode *query);
	BitMask checkSelectableOpBoolean (PMSelectablePtr selectable, struct qnode *query);
	BitMask checkSelectableValue (PMSelectablePtr selectable, struct qvalue *value);

    public:
	Query();
	~Query();

	/**
	 * parse textual query to internal format
	 */
	PMError queryPackage (const std::string& query);

	/**
	 * failure position if parseQuery returned an error
	 * gives position in query string
	 * -1 if no error
	 */
	int failedPos () { return _errpos; }

	/**
	 * return next match for current query
	 */
	const PMPackagePtr nextPackage (const PMPackageManager& packageManager);
};

///////////////////////////////////////////////////////////////////

#endif // Query_h
