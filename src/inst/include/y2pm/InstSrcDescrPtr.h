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

  File:       InstSrcDescrPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constInstSrcDescrPtr and InstSrcDescrPtr.

/-*/
#ifndef InstSrcDescrPtr_h
#define InstSrcDescrPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcDescrPtr
//	CLASS NAME : constInstSrcDescrPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(InstSrcDescr);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref InstSrcDescr *></code>
 *
 * <b>See:</> @ref InstSrcDescr
 **/
class InstSrcDescrPtr {};
/**
 * @short Refcounting <code>&lt;const @ref InstSrcDescr *></code>
 *
 * <b>See:</> @ref InstSrcDescr
 **/
class constInstSrcDescrPtr {};
#endif // just for kdoc

#endif // InstSrcDescrPtr_h

