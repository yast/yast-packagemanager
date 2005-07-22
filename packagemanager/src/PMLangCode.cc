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

  File:       PMLangCode.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <Y2PM.h>
#include <y2pm/PMLangCode.h>

using namespace std;
using namespace PM;

///////////////////////////////////////////////////////////////////
//
// CLASS NAME : LocaleInfo
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : LocaleInfo::preferredLocale
//	METHOD TYPE : LangCode
//
LangCode LocaleInfo::preferredLocale()
{
  return Y2PM::getPreferredLocale();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : LocaleInfo::localeFallback
//	METHOD TYPE : LocaleOrder
//
LocaleOrder LocaleInfo::localeFallback( const LangCode & lang_r )
{
  return Y2PM::getLocaleFallback( lang_r );
}
