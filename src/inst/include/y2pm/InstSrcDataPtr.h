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

   File:       InstSrcDataPtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constInstSrcDataPtr and InstSrcDataPtr.

/-*/
#ifndef InstSrcDataPtr_h
#define InstSrcDataPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcDataPtr
//	CLASS NAME : constInstSrcDataPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(InstSrcData);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref InstSrcData *></code>
 *
 * <b>See:</> @ref InstSrcData
 **/
class InstSrcDataPtr {};
/**
 * @short Refcounting <code>&lt;const @ref InstSrcData *></code>
 *
 * <b>See:</> @ref InstSrcData
 **/
class constInstSrcDataPtr {};
#endif // just for kdoc

#endif // InstSrcDataPtr_h

