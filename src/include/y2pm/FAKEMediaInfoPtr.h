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

   File:       FAKEMediaInfoPtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef FAKEMediaInfoPtr_h
#define FAKEMediaInfoPtr_h

#include <y2util/RepDef.h>
#include <y2pm/MediaInfoPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : FAKEMediaInfoPtr
//	CLASS NAME : constFAKEMediaInfoPtr
/**
 *
 **/
DEFINE_DERIVED_HANDLES(FAKEMediaInfo,public,MediaInfo);

#endif // FAKEMediaInfoPtr_h

