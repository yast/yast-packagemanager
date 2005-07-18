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

  File:       InstSrcDataYUMPtr.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Declaration of pointer classes constInstSrcDataYUMPtr and InstSrcDataYUMPtr.

/-*/
#ifndef InstSrcDataYUMPtr_h
#define InstSrcDataYUMPtr_h

#include <y2pm/InstSrcDataPtr.h>

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcDataYUMPtr
//	CLASS NAME : constInstSrcDataYUMPtr
///////////////////////////////////////////////////////////////////
DEFINE_DERIVED_POINTER(InstSrcDataYUM,InstSrcData);

#endif // InstSrcDataYUMPtr_h

