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

   File:       PMSolvablePtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMSolvablePtr_h
#define PMSolvablePtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSolvablePtr
//	CLASS NAME : constPMSolvablePtr
/**
 *
 **/
DEFINE_BASE_POINTER(PMSolvable);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMSolvable *></code>
 *
 * <b>See:</> @ref PMSolvable
 **/
class PMSolvablePtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMSolvable *></code>
 *
 * <b>See:</> @ref PMSolvable
 **/
class constPMSolvablePtr {};
#endif // just for kdoc

#endif // PMSolvablePtr_h
