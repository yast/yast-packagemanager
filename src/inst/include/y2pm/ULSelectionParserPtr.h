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

  File:       ULSelectionParserPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constULSelectionParserPtr and ULSelectionParserPtr.

/-*/
#ifndef ULSelectionParserPtr_h
#define ULSelectionParserPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : ULSelectionParserPtr
//	CLASS NAME : constULSelectionParserPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(ULSelectionParser);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref ULSelectionParser *></code>
 *
 * <b>See:</> @ref ULSelectionParser
 **/
class ULSelectionParserPtr {};
/**
 * @short Refcounting <code>&lt;const @ref ULSelectionParser *></code>
 *
 * <b>See:</> @ref ULSelectionParser
 **/
class constULSelectionParserPtr {};
#endif // just for kdoc

#endif // ULSelectionParserPtr_h

