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

   File:       PMSelectablePtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMSelectablePtr_h
#define PMSelectablePtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectablePtr
//	CLASS NAME : constPMSelectablePtr
/**
 *
 **/
DEFINE_BASE_POINTER(PMSelectable);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMSelectable *></code>
 *
 * <b>See:</> @ref PMSelectable
 **/
class PMSelectablePtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMSelectable *></code>
 *
 * <b>See:</> @ref PMSelectable
 **/
class constPMSelectablePtr {};
#endif // just for kdoc

#endif // PMSelectablePtr_h

