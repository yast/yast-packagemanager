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

   File:       PMQueryPtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constPMQueryPtr and PMQueryPtr.

/-*/
#ifndef PMQueryPtr_h
#define PMQueryPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMQueryPtr
//	CLASS NAME : constPMQueryPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(PMQuery);

#endif // PMQueryPtr_h
