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

  File:       PMYouPatchManager.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMYouPatchManager_h
#define PMYouPatchManager_h

#include <iosfwd>
#include <list>

#include <y2pm/PMError.h>
#include <y2pm/PMManager.h>
#include <y2pm/PMYouPatch.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchManager
/**
 *
 **/
class PMYouPatchManager : public PMManager {

  friend std::ostream & operator<<( std::ostream & str, const PMYouPatchManager & obj );

  PMYouPatchManager & operator=( const PMYouPatchManager & );
  PMYouPatchManager            ( const PMYouPatchManager & );

  private:

    friend class Y2PM;
    PMYouPatchManager();
    virtual ~PMYouPatchManager();

  private:

    /**
     * Make sure the passed PMObjectPtr actually references a PMYouPatch. If not,
     * report error and return NULL.
     **/
    virtual PMObjectPtr assertObjectType( const PMObjectPtr & object_r ) const;

  public:

};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchManager_h
