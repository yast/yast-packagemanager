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

   File:       PMSelectable.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMSelectable_h
#define PMSelectable_h

#include <iosfwd>
#include <list>

#include <y2pm/PMSelectablePtr.h>

#include <y2pm/PkgName.h>
#include <y2pm/PMObjectPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectable
/**
 *
 **/
class REP_CLASS(PMSelectable) {
  REP_BODY(PMSelectable)

  public:

    typedef std::list<PMObjectPtr>  PMObjectList;

  private:

    friend class PMManager;

    PMManager * _manager;

    PkgName _name;
    PMObjectPtr _installedObj;
    PMObjectPtr _candidateObj;
    PMObjectList _candidateList;

  protected:

    PMSelectable();
    PMSelectable( const PkgName & name_r );

    virtual ~PMSelectable();

  public:

    const PkgName & name() const { return _name; }
    PMObjectPtr installedObj() const { return _installedObj; }
    PMObjectPtr candidateObj() const { return _candidateObj; }
    const PMObjectList & candidateList() const { return _candidateList; }

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMSelectable_h

