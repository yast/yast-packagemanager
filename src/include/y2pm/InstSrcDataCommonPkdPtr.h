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

   File:       InstSrcDataCommonPkdPtr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef InstSrcDataCommonPkdPtr_h
#define InstSrcDataCommonPkdPtr_h

#include <y2pm/InstSrcDataPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDataCommonPkdPtr
//	CLASS NAME : constInstSrcDataCommonPkdPtr
/**
 *
 **/
DEFINE_DERIVED_HANDLES( InstSrcDataCommonPkd, public, InstSrcData );

#endif // InstSrcDataCommonPkdPtr_h

