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

   File:       RpmDbPtr.h
   Purpose:    pointer class for RpmDb
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#ifndef RpmDbPtr_h
#define RpmDbPtr_h

#include <y2util/RepDef.h>

DEFINE_BASE_POINTER(RpmDb);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref RpmDb *></code>
 *
 * <b>See:</> @ref RpmDb
 **/
class RpmDbPtr {};
/**
 * @short Refcounting <code>&lt;const @ref RpmDb *></code>
 *
 * <b>See:</> @ref RpmDb
 **/
class constRpmDbPtr {};
#endif // just for kdoc

#endif // RpmDbPtr_h

