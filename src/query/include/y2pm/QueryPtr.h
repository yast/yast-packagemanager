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

   File:       QueryPtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constQueryPtr and QueryPtr.

/-*/
#ifndef QueryPtr_h
#define QueryPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : QueryPtr
//	CLASS NAME : constQueryPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(Query);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref Query *></code>
 *
 * <b>See:</> @ref Query
 **/
class QueryPtr {};
/**
 * @short Refcounting <code>&lt;const @ref Query *></code>
 *
 * <b>See:</> @ref Query
 **/
class constQueryPtr {};
#endif // just for kdoc

#endif // QueryPtr_h
