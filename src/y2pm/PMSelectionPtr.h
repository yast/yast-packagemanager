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

  File:       PMSelectionPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constPMSelectionPtr and PMSelectionPtr.

/-*/
#ifndef PMSelectionPtr_h
#define PMSelectionPtr_h

#include <y2pm/PMObjectPtr.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMSelectionPtr
//	CLASS NAME : constPMSelectionPtr
///////////////////////////////////////////////////////////////////
DEFINE_DERIVED_POINTER( PMSelection, PMObject, PMSolvable );

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMSelection *></code>
 *
 * <b>See:</> @ref PMSelection
 **/
class PMSelectionPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMSelection *></code>
 *
 * <b>See:</> @ref PMSelection
 **/
class constPMSelectionPtr {};
#endif // just for kdoc

#endif // PMSelectionPtr_h
