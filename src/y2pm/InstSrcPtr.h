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

  File:       InstSrcPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constInstSrcPtr and InstSrcPtr.

/-*/
#ifndef InstSrcPtr_h
#define InstSrcPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcPtr
//	CLASS NAME : constInstSrcPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(InstSrc);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref InstSrc *></code>
 *
 * <b>See:</> @ref InstSrc
 **/
class InstSrcPtr {};
/**
 * @short Refcounting <code>&lt;const @ref InstSrc *></code>
 *
 * <b>See:</> @ref InstSrc
 **/
class constInstSrcPtr {};
#endif // just for kdoc

#endif // InstSrcPtr_h
