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

  File:       librpmDbPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constlibrpmDbPtr and librpmDbPtr.

/-*/
#ifndef librpmDbPtr_h
#define librpmDbPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : librpmDbPtr
//	CLASS NAME : constlibrpmDbPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(librpmDb);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref librpmDb *></code>
 *
 * <b>See:</> @ref librpmDb
 **/
class librpmDbPtr {};
/**
 * @short Refcounting <code>&lt;const @ref librpmDb *></code>
 *
 * <b>See:</> @ref librpmDb
 **/
class constlibrpmDbPtr {};
#endif // just for kdoc

#endif // librpmDbPtr_h

