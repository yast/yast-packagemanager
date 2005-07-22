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

  File:       binHeaderPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constbinHeaderPtr and binHeaderPtr.

/-*/
#ifndef binHeaderPtr_h
#define binHeaderPtr_h

#include <y2util/RepDef.h>

///////////////////////////////////////////////////////////////////
//      CLASS NAME : binHeaderPtr
//      CLASS NAME : constbinHeaderPtr
///////////////////////////////////////////////////////////////////
DEFINE_BASE_POINTER(binHeader);

#endif // binHeaderPtr_h
