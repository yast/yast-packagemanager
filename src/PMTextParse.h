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

  Purpose: Conversion of  y2pm specific datatypes to/from text representation.

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
// PM::CandidateOrder
///////////////////////////////////////////////////////////////////
template<>
bool TextConvert<PM::CandidateOrder>::toText( const value_type & value_r, text_type & asText_r ) {
  switch ( value_r ) {
#define ENUM_OUT(T) case PM::CO_##T: asText_r = #T; return true
    ENUM_OUT( DEFAULT );
    ENUM_OUT( AVS );
    ENUM_OUT( ASV );
#undef ENUM_OUT
  }
  // unknown value:
  asText_r = "DEFAULT";
  return true;
}
template<>
bool TextConvert<PM::CandidateOrder>::fromText( value_type & value_r, const text_type & asText_r ) {
  text_type match( stringutil::toUpper( asText_r ) );
#define ENUM_OUT(T) if ( match == #T ) { value_r = PM::CO_##T; return true; }
  ENUM_OUT( DEFAULT );
  ENUM_OUT( AVS );
  ENUM_OUT( ASV );
#undef ENUM_OUT
  // unknown value:
  value_r = PM::CO_DEFAULT;
  return true;
}


///////////////////////////////////////////////////////////////////
} // namespace TextParse
///////////////////////////////////////////////////////////////////

#endif // PMTextParse_h
