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
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMManager.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Query
/**
 * @short Interface class for queries
 **/
class Query : virtual public Rep {
    REP_BODY(Query);

    private:
	struct qnode *_query;
	int _errpos;

	bool checkSelectable (PMSelectablePtr selectable);
	bool checkPackage (PMPackagePtr package);
	bool checkSelection (PMSelectionPtr selection);
    public:
	Query();
	~Query();
	/**
	 * parse textual query to internal format
	 */
	PMError parseQuery (const std::string& query);
	/**
	 * failure position if parseQuery returned an error
	 * gives position in query string
	 * -1 if no error
	 */
	int failedPos () { return _errpos; }
	const std::list<PMSelectablePtr> querySelectables(const PMManager& packageManager);
	const std::list<PMPackagePtr> queryPackages(const PMManager& packageManager);
	const std::list<PMSelectionPtr> querySelections(const PMManager& selectionManager);
};

///////////////////////////////////////////////////////////////////

#endif // Query_h
