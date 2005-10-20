/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:       PMLangCode.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef PMLangCode_h
#define PMLangCode_h

#include <iosfwd>
#include <set>
#include <map>
#include <list>
#include <string>

#include <y2util/LangCode.h>

///////////////////////////////////////////////////////////////////
namespace PM {
;//////////////////////////////////////////////////////////////////

/**
 *
 **/
typedef std::set<LangCode>  LocaleSet;

/**
 *
 **/
typedef std::list<LangCode> LocaleOrder;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : LocaleInfo
/**
 *
 **/
struct LocaleInfo {
  /**
   * The current preferred locale.
   **/
  static LangCode preferredLocale();
  /**
   * Ordered list containing the requested locale and possible fallbacks.
   **/
  static LocaleOrder localeFallback( const LangCode & lang_r );
  /**
   * Fallbacklist for the current preferred locale.
   **/
  static LocaleOrder localeFallback() { return localeFallback( preferredLocale() ); }
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : LocaleData<_Tp>
/**
 *
 **/
template<typename _Tp>
struct LocaleData : public LocaleInfo {
  typedef std::map<LangCode,_Tp> datamap;
  datamap _datamap;

  /**
   * Return data for the requested locale, considering possible fallbacks.
   * Return default data if nothing appropriate was found in datamap.
   **/
  _Tp operator()( const LangCode & lang_r ) const {
    // ckeck locale and fallbacks
    LocaleOrder langs( localeFallback( lang_r ) );
    for ( LocaleOrder::const_iterator lang = langs.begin(); lang != langs.end(); ++lang ) {
      typename datamap::const_iterator found( _datamap.find( *lang ) );
      if ( found != _datamap.end() ) {
	return found->second; // gotcha
      }
    }
    // ckeck empty locale
    typename datamap::const_iterator found( _datamap.find( LangCode() ) );
    if ( found != _datamap.end() ) {
      return found->second; // gotcha
    }
    // nothing found
    return _Tp();
  }

  /**
   * Return data for the current preferred locale.
   **/
  _Tp operator()() const { return operator()( preferredLocale() ); }
};

///////////////////////////////////////////////////////////////////

/**
 *
 **/
typedef LocaleData<std::string> LocaleString;


///////////////////////////////////////////////////////////////////
} // namespace PM
///////////////////////////////////////////////////////////////////

#endif // PMLangCode_h
