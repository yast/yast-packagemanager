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

  File:       RpmLibHeaderPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constRpmLibHeaderPtr and RpmLibHeaderPtr.

/-*/
#ifndef RpmLibHeaderPtr_h
#define RpmLibHeaderPtr_h

#include <y2pm/binHeaderPtr.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : RpmLibHeaderPtr
//	CLASS NAME : constRpmLibHeaderPtr
///////////////////////////////////////////////////////////////////
DEFINE_DERIVED_POINTER(RpmLibHeader,binHeader,binHeader);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref RpmLibHeader *></code>
 *
 * <b>See:</> @ref RpmLibHeader
 **/
class RpmLibHeaderPtr {
};
/**
 * @short Refcounting <code>&lt;const @ref RpmLibHeader *></code>
 *
 * <b>See:</> @ref RpmLibHeader
 **/
class constRpmLibHeaderPtr {
};
#endif // just for kdoc

#endif // RpmLibHeaderPtr_h

