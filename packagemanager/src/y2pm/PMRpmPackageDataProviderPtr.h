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

   File:       PMRpmPackageDataProviderPtr.h
   Purpose:    pointer class for PMRpmPackageDataProvider
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#ifndef PMRpmPackageDataProviderPtr_h
#define PMRpmPackageDataProviderPtr_h

#include <y2pm/PMPackageDataProviderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMRpmPackageDataProviderPtr
//	CLASS NAME : constPMRpmPackageDataProviderPtr
/**
 *
 **/
DEFINE_DERIVED_POINTER(PMRpmPackageDataProvider, PMPackageDataProvider );

#endif // PMRpmPackageDataProviderPtr_h

