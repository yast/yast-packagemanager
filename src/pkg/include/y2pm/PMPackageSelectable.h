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

   File:       PMPackageSelectable.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMPackageSelectable_h
#define PMPackageSelectable_h

#include <iosfwd>

#include <y2pm/PMPackageSelectablePtr.h>

#include <y2pm/PMSelectable.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageSelectable
/**
 *
 **/
class PMPackageSelectable : virtual public Rep, public PMSelectable {
  REP_BODY(PMPackageSelectable);

  public:

    PMPackageSelectable();

    virtual ~PMPackageSelectable();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMPackageSelectable_h

