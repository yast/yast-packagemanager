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

   File:       PMYouPackageDataProviderPtr.h
   Purpose:    pointer class for PMYouPackageDataProvider
   Author:     Cornelius Schumacher <cschum@suse.de>
   Maintainer: Cornelius Schumacher <cschum@suse.de>

/-*/

#ifndef PMYouPackageDataProviderPtr_h
#define PMYouPackageDataProviderPtr_h

#include <y2pm/PMPackageDataProviderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPackageDataProviderPtr
//	CLASS NAME : constPMYouPackageDataProviderPtr
/**
 *
 **/
DEFINE_DERIVED_POINTER(PMYouPackageDataProvider, PMPackageDataProvider, PMPackageDataProvider );

#endif // PMYouPackageDataProviderPtr_h

