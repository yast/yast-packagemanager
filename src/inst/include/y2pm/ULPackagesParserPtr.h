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

  File:       ULPackagesParserPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constULPackagesParserPtr and ULPackagesParserPtr.

/-*/
#ifndef ULPackagesParserPtr_h
#define ULPackagesParserPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : ULPackagesParserPtr
//	CLASS NAME : constULPackagesParserPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(ULPackagesParser);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref ULPackagesParser *></code>
 *
 * <b>See:</> @ref ULPackagesParser
 **/
class ULPackagesParserPtr {};
/**
 * @short Refcounting <code>&lt;const @ref ULPackagesParser *></code>
 *
 * <b>See:</> @ref ULPackagesParser
 **/
class constULPackagesParserPtr {};
#endif // just for kdoc

#endif // ULPackagesParserPtr_h

