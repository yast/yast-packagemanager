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

  File:       InstTargetProdDBPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constInstTargetProdDBPtr and InstTargetProdDBPtr.

/-*/
#ifndef InstTargetProdDBPtr_h
#define InstTargetProdDBPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstTargetProdDBPtr
//	CLASS NAME : constInstTargetProdDBPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(InstTargetProdDB);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref InstTargetProdDB *></code>
 *
 * <b>See:</> @ref InstTargetProdDB
 **/
class InstTargetProdDBPtr {};
/**
 * @short Refcounting <code>&lt;const @ref InstTargetProdDB *></code>
 *
 * <b>See:</> @ref InstTargetProdDB
 **/
class constInstTargetProdDBPtr {};
#endif // just for kdoc

#endif // InstTargetProdDBPtr_h

