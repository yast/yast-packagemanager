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

#include <y2pm/PMPackagePtr.h>

#include <y2pm/PMError.h>
#include <y2pm/PMManager.h>

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

    static PMPackageManager * _PM;

    PMPackageManager();
    virtual ~PMPackageManager();

  public:

    static PMPackageManager & PM();

    typedef std::list<PMPackagePtr>     PackageList;
    typedef PackageList::iterator       PackageList_iterator;
    typedef PackageList::const_iterator PackageList_const_iterator;

  protected:

  public:

    void addPackages( const PackageList & pkglist_r );
};

///////////////////////////////////////////////////////////////////

#endif // PMPackageManager_h
