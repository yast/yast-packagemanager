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

   File:       PMYouPatchInfoPtr.h

   Author:     Cornelius Schumacher <cschum@suse.de>
   Maintainer: Cornelius Schumacher <cschum@suse.de>

/-*/
#ifndef PMYouPatchInfoPtr_h
#define PMYouPatchInfoPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchInfoPtr
//	CLASS NAME : constPMYouPatchInfoPtr
/**
 *
 **/
DEFINE_BASE_POINTER(PMYouPatchInfo);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMYouPatchInfo *></code>
 *
 * <b>See:</> @ref PMYouPatchInfo
 **/
class PMYouPatchInfoPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMYouPatchInfo *></code>
 *
 * <b>See:</> @ref PMYouPatchInfo
 **/
class constPMYouPatchInfoPtr {};
#endif // just for kdoc

#endif // PMYouPatchInfoPtr_h

