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

  File:       PMSelectionManager.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMSelectionManager_h
#define PMSelectionManager_h

#include <iosfwd>
#include <list>

#include <y2pm/PMError.h>
#include <y2pm/PMManager.h>
#include <y2pm/PMSelection.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectionManager
/**
 *
 **/
class PMSelectionManager : public PMManager {

  friend std::ostream & operator<<( std::ostream & str, const PMSelectionManager & obj );

  PMSelectionManager & operator=( const PMSelectionManager & );
  PMSelectionManager            ( const PMSelectionManager & );

  private:

    friend class Y2PM;
    PMSelectionManager();
    virtual ~PMSelectionManager();

  private:

    /**
     * Make shure the passed PMObjectPtr actually references a PMSelection. If not,
     * rerport error and return NULL.
     **/
    virtual PMObjectPtr assertObjectType( const PMObjectPtr & object_r ) const;

  public:

};

///////////////////////////////////////////////////////////////////

#endif // PMSelectionManager_h
