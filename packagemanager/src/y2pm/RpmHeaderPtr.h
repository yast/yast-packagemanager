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

  File:       RpmHeaderPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constRpmHeaderPtr and RpmHeaderPtr.

/-*/
#ifndef RpmHeaderPtr_h
#define RpmHeaderPtr_h

#include <y2pm/binHeaderPtr.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : RpmHeaderPtr
//	CLASS NAME : constRpmHeaderPtr
///////////////////////////////////////////////////////////////////
DEFINE_DERIVED_POINTER(RpmHeader,binHeader);

#endif // RpmHeaderPtr_h

