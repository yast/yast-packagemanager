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

  File:       InstSrcData_ULPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constInstSrcData_ULPtr and InstSrcData_ULPtr.

/-*/
#ifndef InstSrcData_ULPtr_h
#define InstSrcData_ULPtr_h

#include <y2pm/InstSrcDataPtr.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcData_ULPtr
//	CLASS NAME : constInstSrcData_ULPtr
///////////////////////////////////////////////////////////////////
DEFINE_DERIVED_POINTER(InstSrcData_UL,InstSrcData,InstSrcData);

#endif // InstSrcData_ULPtr_h

