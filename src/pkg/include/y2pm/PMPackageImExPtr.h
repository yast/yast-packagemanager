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

  File:       PMPackageImExPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constPMPackageImExPtr and PMPackageImExPtr.

/-*/
#ifndef PMPackageImExPtr_h
#define PMPackageImExPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMPackageImExPtr
//	CLASS NAME : constPMPackageImExPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(PMPackageImEx);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMPackageImEx *></code>
 *
 * <b>See:</> @ref PMPackageImEx
 **/
class PMPackageImExPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMPackageImEx *></code>
 *
 * <b>See:</> @ref PMPackageImEx
 **/
class constPMPackageImExPtr {};
#endif // just for kdoc

#endif // PMPackageImExPtr_h

