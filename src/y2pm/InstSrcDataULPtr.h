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

  File:       InstSrcDataULPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constInstSrcDataULPtr and InstSrcDataULPtr.

/-*/
#ifndef InstSrcDataULPtr_h
#define InstSrcDataULPtr_h

#include <y2pm/InstSrcDataPtr.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcDataULPtr
//	CLASS NAME : constInstSrcDataULPtr
///////////////////////////////////////////////////////////////////
DEFINE_DERIVED_POINTER(InstSrcDataUL,InstSrcData,InstSrcData);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref InstSrcDataUL *></code>
 *
 * <b>See:</> @ref InstSrcDataUL
 **/
class InstSrcDataULPtr {};
/**
 * @short Refcounting <code>&lt;const @ref InstSrcDataUL *></code>
 *
 * <b>See:</> @ref InstSrcDataUL
 **/
class constInstSrcDataULPtr {};
#endif // just for kdoc

#endif // InstSrcDataULPtr_h

