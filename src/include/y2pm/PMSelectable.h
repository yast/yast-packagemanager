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

#include <y2pm/PMSelectablePtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectable
/**
 *
 **/
class REP_CLASS(PMSelectable) {
  REP_BODY(PMSelectable)

  protected:

    friend class PMManager;

    PMSelectable();

    virtual ~PMSelectable();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMSelectable_h

