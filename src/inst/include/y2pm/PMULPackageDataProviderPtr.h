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

   File:       PMULPackageDataProviderPtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constPMULPackageDataProviderPtr and PMULPackageDataProviderPtr.

/-*/
#ifndef PMULPackageDataProviderPtr_h
#define PMULPackageDataProviderPtr_h

#include <y2pm/PMPackageDataProviderPtr.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMULPackageDataProviderPtr
//	CLASS NAME : constPMULPackageDataProviderPtr
///////////////////////////////////////////////////////////////////
DEFINE_DERIVED_POINTER(PMULPackageDataProvider,PMPackageDataProvider,PMPackageDataProvider);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMULPackageDataProvider *></code>
 *
 * <b>See:</> @ref PMULPackageDataProvider
 **/
class PMULPackageDataProviderPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMULPackageDataProvider *></code>
 *
 * <b>See:</> @ref PMULPackageDataProvider
 **/
class constPMULPackageDataProviderPtr {};
#endif // just for kdoc

#endif // PMULPackageDataProviderPtr_h

