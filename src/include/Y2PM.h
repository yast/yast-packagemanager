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

  File:       Y2PM.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Core class providing access to all components of the
  Package Management creating them on demand.

/-*/
#ifndef Y2PM_h
#define Y2PM_h

#include <iosfwd>

//#include <y2pm/InstTarget.h>
//#include <y2pm/InstSrcManager.h>
#include <y2pm/PMPackageManager.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Y2PM
/**
 * Core class providing access to all components of the
 * Package Management creating them on demand.
 **/
class Y2PM {

  private:

    //static InstTarget * _instTarget;

    //static InstSrcManager * _instSrcManager;

    static PMPackageManager * _packageManager;

  public:

#if 0
    /**
     * Access to the installation target
     **/
    static InstTarget & instTarget();
#endif

#if 0
    /**
     * Access to the installation source manager
     **/
    static InstSrcManager & instSrcManager();
#endif

    /**
     * Access to the package manager
     **/
    static PMPackageManager & packageManager();
};

///////////////////////////////////////////////////////////////////

#endif // Y2PM_h
