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

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProviderPtr
//	CLASS NAME : constPMPackageDataProviderPtr
/**
 *
 **/
DEFINE_BASE_POINTER(PMPackageDataProvider);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMPackageDataProvider *></code>
 *
 * <b>See:</> @ref PMPackageDataProvider
 **/
class PMPackageDataProviderPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMPackageDataProvider *></code>
 *
 * <b>See:</> @ref PMPackageDataProvider
 **/
class constPMPackageDataProviderPtr {};
#endif // just for kdoc

#endif // PMPackageDataProviderPtr_h

