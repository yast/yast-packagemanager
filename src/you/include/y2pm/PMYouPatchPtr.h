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

   File:       PMYouPatchPtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMYouPatchPtr_h
#define PMYouPatchPtr_h

#include <y2pm/PMObjectPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchPtr
//	CLASS NAME : constPMYouPatchPtr
/**
 *
 **/
DEFINE_DERIVED_POINTER( PMYouPatch, PMObject, PMSolvable );

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMYouPatch *></code>
 *
 * <b>See:</> @ref PMYouPatch
 **/
class PMYouPatchPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMYouPatch *></code>
 *
 * <b>See:</> @ref PMYouPatch
 **/
class constPMYouPatchPtr {};
#endif // just for kdoc

#endif // PMYouPatchPtr_h
