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

   File:       PMYouServersPtr.h

   Author:     Cornelius Schumacher <cschum@suse.de>
   Maintainer: Cornelius Schumacher <cschum@suse.de>

/-*/
#ifndef PMYouServersPtr_h
#define PMYouServersPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouServersPtr
//	CLASS NAME : constPMYouServersPtr
/**
 *
 **/
DEFINE_BASE_POINTER(PMYouServers);

#if 0 // just for kdoc
/**
 * @short Refcounting <code>&lt;@ref PMYouServers *></code>
 *
 * <b>See:</> @ref PMYouServers
 **/
class PMYouServersPtr {};
/**
 * @short Refcounting <code>&lt;const @ref PMYouServers *></code>
 *
 * <b>See:</> @ref PMYouServers
 **/
class constPMYouServersPtr {};
#endif // just for kdoc

#endif // PMYouServersPtr_h
