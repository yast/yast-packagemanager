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

   File:       PMPackageDataProviderPtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMPackageDataProviderPtr_h
#define PMPackageDataProviderPtr_h

#include <y2pm/PMDataProviderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProviderPtr
//	CLASS NAME : constPMPackageDataProviderPtr
/**
 *
 **/
DEFINE_DERIVED_POINTER(PMPackageDataProvider, PMDataProvider, PMDataProvider );

#endif // PMPackageDataProviderPtr_h

