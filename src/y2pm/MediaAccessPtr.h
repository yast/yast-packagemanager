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

   File:	MediaAccessPtr.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

/-*/
#ifndef MediaAccessPtr_h
#define MediaAccessPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaAccessPtr
//	CLASS NAME : constMediaAccessPtr
/**
 *
 **/
DEFINE_BASE_POINTER(MediaAccess);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref MediaAccess *></code>
 *
 * <b>See:</> @ref MediaAccess
 **/
class MediaAccessPtr {};
/**
 * @short Refcounting <code>&lt;const @ref MediaAccess *></code>
 *
 * <b>See:</> @ref MediaAccess
 **/
class constMediaAccessPtr {};
#endif // just for kdoc

#endif // MediaAccessPtr_h

