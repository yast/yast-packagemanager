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

   File:       PMYouPatchDataProviderPtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMYouPatchDataProviderPtr_h
#define PMYouPatchDataProviderPtr_h

#include <y2pm/PMDataProviderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchDataProviderPtr
//	CLASS NAME : constPMYouPatchDataProviderPtr
/**
 *
 **/
DEFINE_DERIVED_POINTER(PMYouPatchDataProvider, PMDataProvider, PMDataProvider );

#endif // PMYouPatchDataProviderPtr_h

