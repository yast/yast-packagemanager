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

   File:       PMPLAINPackageDataProviderPtr.h
   Purpose:    pointer class for PMPLAINPackageDataProvider
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#ifndef PMPLAINPackageDataProviderPtr_h
#define PMPLAINPackageDataProviderPtr_h

#include <y2pm/PMPackageDataProviderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPLAINPackageDataProviderPtr
//	CLASS NAME : constPMPLAINPackageDataProviderPtr
/**
 *
 **/
DEFINE_DERIVED_POINTER(PMPLAINPackageDataProvider, PMPackageDataProvider, PMPackageDataProvider );

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMPLAINPackageDataProvider *></code>
 *
 * <b>See:</> @ref PMPLAINPackageDataProvider
 **/
class PMPLAINPackageDataProviderPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMPLAINPackageDataProvider *></code>
 *
 * <b>See:</> @ref PMPLAINPackageDataProvider
 **/
class constPMPLAINPackageDataProviderPtr {};
#endif // just for kdoc

#endif // PMPLAINPackageDataProviderPtr_h

