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

  File:       PMSelectionDataProviderPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constPMSelectionDataProviderPtr
  and PMSelectionDataProviderPtr.

/-*/
#ifndef PMSelectionDataProviderPtr_h
#define PMSelectionDataProviderPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMSelectionDataProviderPtr
//	CLASS NAME : constPMSelectionDataProviderPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(PMSelectionDataProvider);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMSelectionDataProvider *></code>
 *
 * <b>See:</> @ref PMSelectionDataProvider
 **/
class PMSelectionDataProviderPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMSelectionDataProvider *></code>
 *
 * <b>See:</> @ref PMSelectionDataProvider
 **/
class constPMSelectionDataProviderPtr {};
#endif // just for kdoc

#endif // PMSelectionDataProviderPtr_h

