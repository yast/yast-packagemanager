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

   File:       PMObjectPtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMObjectPtr_h
#define PMObjectPtr_h

#include <y2pm/PMSolvablePtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPObjectPtr
//	CLASS NAME : constPMPObjectPtr
/**
 *
 **/
DEFINE_DERIVED_POINTER( PMObject, PMSolvable );

#endif // PMObjectPtr_h
