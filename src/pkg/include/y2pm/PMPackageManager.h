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

  File:       PMPackageManager.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMPackageManager_h
#define PMPackageManager_h

#include <iosfwd>
#include <list>

#include <y2pm/PMError.h>
#include <y2pm/PMManager.h>
#include <y2pm/PMPackage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageManager
/**
 *
 **/
class PMPackageManager : public PMManager {

  friend std::ostream & operator<<( std::ostream & str, const PMPackageManager & obj );

  PMPackageManager & operator=( const PMPackageManager & );
  PMPackageManager            ( const PMPackageManager & );

  private:

    friend class Y2PM;
    PMPackageManager();
    virtual ~PMPackageManager();

  public:

};

///////////////////////////////////////////////////////////////////

#endif // PMPackageManager_h
