/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

   Author:     Cornelius Schumacher <cschum@suse.de>
   Maintainer: Cornelius Schumacher <cschum@suse.de>

/-*/
#ifndef PMYOUPRODUCTPTR_H
#define PMYOUPRODUCTPTR_H

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouProductPtr
//	CLASS NAME : constPMYouProductPtr
/**
 *
 **/
DEFINE_BASE_POINTER(PMYouProduct);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMYouProduct *></code>
 *
 * <b>See:</> @ref PMYouProduct
 **/
class PMYouProductPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMYouProduct *></code>
 *
 * <b>See:</> @ref PMYouProduct
 **/
class constPMYouProductPtr {};
#endif // just for kdoc

#endif
