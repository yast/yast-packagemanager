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

   File:       PMPackageSelectablePtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMPackageSelectablePtr_h
#define PMPackageSelectablePtr_h

#include <y2pm/PMSelectablePtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageSelectablePtr
//	CLASS NAME : constPMPackageSelectablePtr
/**
 *
 **/
DEFINE_DERIVED_HANDLES(PMPackageSelectable, public, PMSelectable );

#endif // PMPackageSelectablePtr_h

