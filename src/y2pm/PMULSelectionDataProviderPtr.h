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

   File:       PMULSelectionDataProviderPtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constPMULSelectionDataProviderPtr and PMULSelectionDataProviderPtr.

/-*/
#ifndef PMULSelectionDataProviderPtr_h
#define PMULSelectionDataProviderPtr_h

#include <y2pm/PMSelectionDataProviderPtr.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMULSelectionDataProviderPtr
//	CLASS NAME : constPMULSelectionDataProviderPtr
///////////////////////////////////////////////////////////////////
DEFINE_DERIVED_POINTER(PMULSelectionDataProvider,PMSelectionDataProvider,PMSelectionDataProvider);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMULSelectionDataProvider *></code>
 *
 * <b>See:</> @ref PMULSelectionDataProvider
 **/
class PMULSelectionDataProviderPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMULSelectionDataProvider *></code>
 *
 * <b>See:</> @ref PMULSelectionDataProvider
 **/
class constPMULSelectionDataProviderPtr {};
#endif // just for kdoc

#endif // PMULSelectionDataProviderPtr_h

