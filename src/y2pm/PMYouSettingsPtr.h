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
#ifndef PMYouSettingsPtr_h
#define PMYouSettingsPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouSettingsPtr
//	CLASS NAME : constPMYouSettingsPtr
/**
 *
 **/
DEFINE_BASE_POINTER(PMYouSettings);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMYouSettings *></code>
 *
 * <b>See:</> @ref PMYouSettings
 **/
class PMYouSettingsPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMYouSettings *></code>
 *
 * <b>See:</> @ref PMYouSettings
 **/
class constPMYouSettingsPtr {};
#endif // just for kdoc

#endif

