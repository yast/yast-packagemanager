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

  File:       RpmLib.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Private includes to acces librpm. Not for public header files.

/-*/
#ifndef RpmLib_h
#define RpmLib_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C" {
#include <rpm/rpmlib.h>
#include <rpm/rpmmacro.h>
#include <fcntl.h>
}

#endif // RpmLib_h
