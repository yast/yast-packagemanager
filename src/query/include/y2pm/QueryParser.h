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

   File:       QueryParser.h

   Author:     Klaus Kaempf <kkaempf@suse.de>
   Maintainer: Klaus Kaempf <kkaempf@suse.de>

/-*/
#ifndef QueryParser_h
#define QueryParser_h

#include <iosfwd>
#include <string>

#include <y2pm/QueryError.h>
#include <y2pm/QueryNode.h>
#include <y2pm/QueryScanner.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : QueryParser
/**
 * @short parser query from string to struct qnode
 **/
class QueryParser {

    private:
	void free_node (struct qnode *node);
	int op_binding (enum operation op);
	enum operation parse_op (const char **query);
	PMError check_node (struct qnode *node);
	int parse_value (struct qvalue *value, const char **query, PMError *error);
	struct qnode *parse_expr (const char **query, PMError *error);
    public:
	QueryParser () {}
	~QueryParser () {}
	PMError parseQuery (const std::string& query, int& errpos, struct qnode **node);
};

///////////////////////////////////////////////////////////////////

#endif // Query_h
