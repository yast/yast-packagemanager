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

  File:       PMLocale.cc

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Provide translation support for packagemanager.

/-*/

#include <y2pm/PMLocale.h>

#include <libintl.h>

#include <iostream>

void PMLocale::setTextdomain()
{
    const char *domain = "packagemanager";

    const char *result = bindtextdomain( domain, LOCALEDIR );
    result = bind_textdomain_codeset( domain, "utf8" );
    result = textdomain( domain );

    // Make change known.
    {
	extern int _nl_msg_cat_cntr;
	++_nl_msg_cat_cntr;
    }
}
