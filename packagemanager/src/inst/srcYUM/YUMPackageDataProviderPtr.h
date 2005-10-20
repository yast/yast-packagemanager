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

   File:       YUMPackageDataProviderPtr.h
   Purpose:    pointer class for YUM::PackageDataProvider
   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#ifndef YUMPackageDataProviderPtr_h
#define YUMPackageDataProviderPtr_h

#include <y2pm/PMPackageDataProviderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : YUM::PackageDataProviderPtr
//	CLASS NAME : YUM::constPackageDataProviderPtr
/**
 *
 **/
DEFINE_DERIVED_POINTER_IN_NAMESPACE( YUM, PackageDataProvider, PMPackageDataProvider );
///////////////////////////////////////////////////////////////////


#endif // YUMPackageDataProviderPtr_h

