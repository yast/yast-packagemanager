/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SUSE LINUX AG |
\----------------------------------------------------------------------/

  File:       PMLocale.h

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Provide support for translation handling.

/-*/
#ifndef PMLocale_h
#define PMLocale_h

#include <libintl.h>

#define _(MSG)  dgettext( "packagemanager", (MSG) )

class PMLocale
{
  public:
    static void setTextdomain();
};

#endif
