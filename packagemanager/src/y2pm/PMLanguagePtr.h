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

  File:       PMLanguagePtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constPMLanguagePtr and PMLanguagePtr.

/-*/
#ifndef PMLanguagePtr_h
#define PMLanguagePtr_h

#include <y2pm/PMObjectPtr.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMLanguagePtr
//	CLASS NAME : constPMLanguagePtr
///////////////////////////////////////////////////////////////////
DEFINE_DERIVED_POINTER(PMLanguage,PMSolvable);

#endif // PMLanguagePtr_h

