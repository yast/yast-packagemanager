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

  File:       PMTextParse.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef PMTextParse_h
#define PMTextParse_h

#include <iosfwd>

#include <y2pm/PMTypes.h>

#include "TextParse.h"

//////////////////////////////////////////////////////////////////
namespace TextParse {
;//////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
// SINGLELINE
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// LangCode
///////////////////////////////////////////////////////////////////
template<>
bool TextConvert<LangCode>::toText( const value_type & value_r, text_type & asText_r ) {
  asText_r = value_r;
  return true;
}
template<>
bool TextConvert<LangCode>::fromText( value_type & value_r, const text_type & asText_r ) {
  value_r = value_type( asText_r ); // explicit constructor
  return true;
}

///////////////////////////////////////////////////////////////////
} // namespace TextParse
///////////////////////////////////////////////////////////////////

#endif // PMTextParse_h
