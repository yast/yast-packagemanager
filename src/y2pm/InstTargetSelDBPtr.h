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

  File:       InstTargetSelDBPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constInstTargetSelDBPtr and InstTargetSelDBPtr.

/-*/
#ifndef InstTargetSelDBPtr_h
#define InstTargetSelDBPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstTargetSelDBPtr
//	CLASS NAME : constInstTargetSelDBPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(InstTargetSelDB);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref InstTargetSelDB *></code>
 *
 * <b>See:</> @ref InstTargetSelDB
 **/
class InstTargetSelDBPtr {};
/**
 * @short Refcounting <code>&lt;const @ref InstTargetSelDB *></code>
 *
 * <b>See:</> @ref InstTargetSelDB
 **/
class constInstTargetSelDBPtr {};
#endif // just for kdoc

#endif // InstTargetSelDBPtr_h

