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

  File:       PMSelectionDataProviderPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constPMSelectionDataProviderPtr
  and PMSelectionDataProviderPtr.

/-*/
#ifndef PMSelectionDataProviderPtr_h
#define PMSelectionDataProviderPtr_h

#include <y2pm/PMDataProviderPtr.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMSelectionDataProviderPtr
//	CLASS NAME : constPMSelectionDataProviderPtr
///////////////////////////////////////////////////////////////////
DEFINE_DERIVED_POINTER(PMSelectionDataProvider, PMDataProvider, PMDataProvider );

#endif // PMSelectionDataProviderPtr_h

