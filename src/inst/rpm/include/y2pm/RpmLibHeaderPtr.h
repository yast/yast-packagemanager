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

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : RpmLibHeaderPtr
//	CLASS NAME : constRpmLibHeaderPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(RpmLibHeader);

#endif // RpmLibHeaderPtr_h

