/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

   File:       PMYouPatchPathsPtr.h

   Author:     Cornelius Schumacher <cschum@suse.de>
   Maintainer: Cornelius Schumacher <cschum@suse.de>

/-*/
#ifndef PMYouPatchPathsPtr_h
#define PMYouPatchPathsPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchPathsPtr
//	CLASS NAME : constPMYouPatchPathsPtr
/**
 *
 **/
DEFINE_BASE_POINTER(PMYouPatchPaths);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMYouPatchPaths *></code>
 *
 * <b>See:</> @ref PMYouPatchPaths
 **/
class PMYouPatchPathsPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMYouPatchPaths *></code>
 *
 * <b>See:</> @ref PMYouPatchPaths
 **/
class constPMYouPatchPathsPtr {};
#endif // just for kdoc

#endif // PMYouPatchPathsPtr_h

