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

   File:       PMPackageDataProviderULPtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMPackageDataProviderULPtr_h
#define PMPackageDataProviderULPtr_h

#include <y2pm/PMPackageDataProviderUL.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProviderULPtr
//	CLASS NAME : constPMPackageDataProviderULPtr
/**
 *
 **/
DEFINE_DERIVED_POINTER(PMPackageDataProviderUL, PMPackageDataProvider, PMPackageDataProvider );

#endif // PMPackageDataProviderULPtr_h

