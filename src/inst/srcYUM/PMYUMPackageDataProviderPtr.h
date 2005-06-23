/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       PMYUMPackageDataProviderPtr.h
   Purpose:    pointer class for PMYUMPackageDataProvider
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#ifndef PMYUMPackageDataProviderPtr_h
#define PMYUMPackageDataProviderPtr_h

#include <y2pm/PMPackageDataProviderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYUMPackageDataProviderPtr
//	CLASS NAME : constPMYUMPackageDataProviderPtr
/**
 *
 **/
DEFINE_DERIVED_POINTER(PMYUMPackageDataProvider, PMPackageDataProvider );

#endif // PMYUMPackageDataProviderPtr_h

