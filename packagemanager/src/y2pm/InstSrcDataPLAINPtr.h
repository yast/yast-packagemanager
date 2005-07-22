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

  File:       InstSrcDataPLAINPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constInstSrcDataPLAINPtr and InstSrcDataPLAINPtr.

/-*/
#ifndef InstSrcDataPLAINPtr_h
#define InstSrcDataPLAINPtr_h

#include <y2pm/InstSrcDataPtr.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcDataPLAINPtr
//	CLASS NAME : constInstSrcDataPLAINPtr
///////////////////////////////////////////////////////////////////
DEFINE_DERIVED_POINTER(InstSrcDataPLAIN,InstSrcData);

#endif // InstSrcDataPLAINPtr_h

