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

   File:       PMQuery.h

   Author:     Klaus Kaempf <kkaempf@suse.de>
   Maintainer: Klaus Kaempf <kkaempf@suse.de>

/-*/
#ifndef PMQuery_h
#define PMQuery_h

#include <iosfwd>
#include <string>

#include <y2util/RepDef.h>
#include <y2pm/PMQueryPtr.h>
#include <y2pm/PMQueryParser.h>
#include <y2pm/PMQueryNode.h>
#include <y2pm/PMQueryError.h>

#include <y2pm/PMSelectablePtr.h>


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMQuery
/**
 * @short Interface class for queries
 **/
class PMQuery : virtual public Rep {
    REP_BODY(PMQuery);

    private:
	struct qnode *_query;

    public:
	PMQuery();
	~PMQuery();
};

///////////////////////////////////////////////////////////////////

#endif // PMQuery_h
