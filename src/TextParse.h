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

  File:       TextParse.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Conversion of common datatypes to/from text representation.
  For y2pm specific types see PMTextParse.h.

/-*/
#ifndef TextParse_h
#define TextParse_h

#include <iosfwd>
#include <vector>
#include <list>
#include <set>
#include <string>

#include <y2util/stringutil.h>

///////////////////////////////////////////////////////////////////
namespace TextParse {
;//////////////////////////////////////////////////////////////////

struct SingleLine {
  enum { SINGLELINE = 1, MULTILINE = !SINGLELINE };
  typedef std::string text_type;
  static void mustBeSingleLine() {}
};

struct MultiLine {
  enum { SINGLELINE = 0, MULTILINE = !SINGLELINE };
  typedef std::list<std::string> text_type;
  static void mustBeMultiLine() {}
};

///////////////////////////////////////////////////////////////////

template<typename _Vt>
struct TextConvert : public SingleLine {
  typedef _Vt value_type;
  bool toText( const value_type & value_r, text_type & asText_r );
  bool fromText( value_type & value_r, const text_type & asText_r );
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
// SINGLELINE
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// int
///////////////////////////////////////////////////////////////////
template<>
bool TextConvert<int>::toText( const value_type & value_r, text_type & asText_r ) {
  asText_r = stringutil::numstring( value_r );
  return true;
}
template<>
bool TextConvert<int>::fromText( value_type & value_r, const text_type & asText_r ) {
  value_r = atoi( asText_r.c_str() );
  return true;
}

///////////////////////////////////////////////////////////////////
// unsigned
///////////////////////////////////////////////////////////////////
template<>
bool TextConvert<unsigned>::toText( const value_type & value_r, text_type & asText_r ) {
  asText_r = stringutil::numstring( value_r );
  return true;
}
template<>
bool TextConvert<unsigned>::fromText( value_type & value_r, const text_type & asText_r ) {
  value_r = (value_type)atoi( asText_r.c_str() );
  return true;
}

///////////////////////////////////////////////////////////////////
// long
///////////////////////////////////////////////////////////////////
template<>
bool TextConvert<long>::toText( const value_type & value_r, text_type & asText_r ) {
  asText_r = stringutil::numstring( value_r );
  return true;
}
template<>
bool TextConvert<long>::fromText( value_type & value_r, const text_type & asText_r ) {
  value_r = atol( asText_r.c_str() );
  return true;
}

///////////////////////////////////////////////////////////////////
// unsigned long
///////////////////////////////////////////////////////////////////
template<>
bool TextConvert<unsigned long>::toText( const value_type & value_r, text_type & asText_r ) {
  asText_r = stringutil::numstring( value_r );
  return true;
}
template<>
bool TextConvert<unsigned long>::fromText( value_type & value_r, const text_type & asText_r ) {
  value_r = (value_type)atol( asText_r.c_str() );
  return true;
}

///////////////////////////////////////////////////////////////////
// long long
///////////////////////////////////////////////////////////////////
template<>
bool TextConvert<long long>::toText( const value_type & value_r, text_type & asText_r ) {
  asText_r = stringutil::numstring( value_r );
  return true;
}
template<>
bool TextConvert<long long>::fromText( value_type & value_r, const text_type & asText_r ) {
  value_r = atoll( asText_r.c_str() );
  return true;
}

///////////////////////////////////////////////////////////////////
// unsigned long long
///////////////////////////////////////////////////////////////////
template<>
bool TextConvert<unsigned long long>::toText( const value_type & value_r, text_type & asText_r ) {
  asText_r = stringutil::numstring( value_r );
  return true;
}
template<>
bool TextConvert<unsigned long long>::fromText( value_type & value_r, const text_type & asText_r ) {
  value_r = (value_type)atoll( asText_r.c_str() );
  return true;
}

///////////////////////////////////////////////////////////////////
// std::string
///////////////////////////////////////////////////////////////////
template<>
bool TextConvert<std::string>::toText( const value_type & value_r, text_type & asText_r ) {
  asText_r = value_r;
  return true;
}
template<>
bool TextConvert<std::string>::fromText( value_type & value_r, const text_type & asText_r ) {
  value_r = asText_r;
  return true;
}

///////////////////////////////////////////////////////////////////
//
// MULTILINE
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// std::vector<_Ct> / SingleLine _Ct
///////////////////////////////////////////////////////////////////
template<typename _Ct>
struct TextConvert<std::vector<_Ct> > : public MultiLine {
  typedef std::vector<_Ct> value_type;
  TextConvert() {
    TextConvert<_Ct>::mustBeSingleLine(); // compiler should complain if not
  }
  bool toText( const value_type & value_r, text_type & asText_r ) {
    asText_r.clear();
    TextConvert<_Ct> lconv;
    typename TextConvert<_Ct>::text_type ltext;
    for ( typename value_type::const_iterator it = value_r.begin(); it != value_r.end(); ++it ) {
      lconv.toText( *it, ltext );
      asText_r.push_back( ltext );
    }
    return true;
  }
  bool fromText( value_type & value_r, const text_type & asText_r ) {
    value_type tmpval;
    tmpval.reserve( asText_r.size() );
    TextConvert<_Ct> lconv;
    typename TextConvert<_Ct>::value_type lval;
    for ( text_type::const_iterator it = asText_r.begin(); it != asText_r.end(); ++it ) {
      lconv.fromText( lval, *it );
      tmpval.push_back( lval );
    }
    value_r.swap( tmpval );
    return true;
  }
};

///////////////////////////////////////////////////////////////////
// std::list<_Ct> / SingleLine _Ct
///////////////////////////////////////////////////////////////////
template<typename _Ct>
struct TextConvert<std::list<_Ct> > : public MultiLine {
  typedef std::list<_Ct> value_type;
  TextConvert() {
    TextConvert<_Ct>::mustBeSingleLine(); // compiler should complain if not
  }
  bool toText( const value_type & value_r, text_type & asText_r ) {
    asText_r.clear();
    TextConvert<_Ct> lconv;
    typename TextConvert<_Ct>::text_type ltext;
    for ( typename value_type::const_iterator it = value_r.begin(); it != value_r.end(); ++it ) {
      lconv.toText( *it, ltext );
      asText_r.push_back( ltext );
    }
    return true;
  }
  bool fromText( value_type & value_r, const text_type & asText_r ) {
    value_r.clear();
    TextConvert<_Ct> lconv;
    typename TextConvert<_Ct>::value_type lval;
    for ( text_type::const_iterator it = asText_r.begin(); it != asText_r.end(); ++it ) {
      lconv.fromText( lval, *it );
      value_r.push_back( lval );
    }
    return true;
  }
};

///////////////////////////////////////////////////////////////////
// std::set<_Ct> / SingleLine _Ct
///////////////////////////////////////////////////////////////////
template<typename _Ct>
struct TextConvert<std::set<_Ct> > : public MultiLine {
  typedef std::set<_Ct> value_type;
  TextConvert() {
    TextConvert<_Ct>::mustBeSingleLine(); // compiler should complain if not
  }
  bool toText( const value_type & value_r, text_type & asText_r ) {
    asText_r.clear();
    TextConvert<_Ct> lconv;
    typename TextConvert<_Ct>::text_type ltext;
    for ( typename value_type::const_iterator it = value_r.begin(); it != value_r.end(); ++it ) {
      lconv.toText( *it, ltext );
      asText_r.push_back( ltext );
    }
    return true;
  }
  bool fromText( value_type & value_r, const text_type & asText_r ) {
    value_r.clear();
    TextConvert<_Ct> lconv;
    typename TextConvert<_Ct>::value_type lval;
    for ( text_type::const_iterator it = asText_r.begin(); it != asText_r.end(); ++it ) {
      lconv.fromText( lval, *it );
      value_r.insert( lval );
    }
    return true;
  }
};

///////////////////////////////////////////////////////////////////
} // namespace TextParse
///////////////////////////////////////////////////////////////////

#endif // TextParse_h
