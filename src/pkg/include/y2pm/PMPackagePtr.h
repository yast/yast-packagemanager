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

   File:       PMPackagePtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMPackagePtr_h
#define PMPackagePtr_h

#include <y2pm/PMObjectPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackagePtr
//	CLASS NAME : constPMPackagePtr
/**
 *
 **/
DEFINE_DERIVED_POINTER( PMPackage, PMObject, PMSolvable );

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMPackage *></code>
 *
 * <b>See:</> @ref PMPackage
 **/
class PMPackagePtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMPackage *></code>
 *
 * <b>See:</> @ref PMPackage
 **/
class constPMPackagePtr {};
#endif // just for kdoc

#endif // PMPackagePtr_h
