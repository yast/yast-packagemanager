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

  File:       Y2PM.locale.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/LangCode.h>

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
#undef Y2LOG
#define Y2LOG "Y2PM"
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Y2PM::LocaleSettings
/**
 *
 **/
class Y2PM::LocaleSettings {

  public:

    static LangCode _locale_fallback;

    LangCode _preferred_locale;

    LocaleSet _requested_locales;

  public:

    LocaleSettings()
      : _preferred_locale( _locale_fallback )
    {
      char * lang = getenv ( "LANG" );
      if ( lang ) {
	MIL << "$LANG='" << lang << "'" << endl;
	string langstr( lang );
	string::size_type sep = langstr.find ( "@" ); // cut off "@"
	if ( sep != string::npos ) {
	  langstr.erase( sep );
	}
	sep = langstr.find ( "." ); // cut off "."
	if ( sep != string::npos ) {
	  langstr.erase( sep );
	}
	if ( langstr.size() ) {
	  _preferred_locale = LangCode( langstr );
	}
      }
      MIL << "Preferred locale: '" << _preferred_locale << "'" << endl;
      MIL << "Fallback locale: '" << _locale_fallback << "'" << endl;
    }
};

LangCode Y2PM::LocaleSettings::_locale_fallback( "en" );

///////////////////////////////////////////////////////////////////

inline Y2PM::LocaleSettings & Y2PM::localeSettings() {
  static LocaleSettings * _localeSettings = 0;
  if ( ! _localeSettings ) {
    _localeSettings = new LocaleSettings;
  }

  return *_localeSettings;
}

///////////////////////////////////////////////////////////////////
//
// Y2PM public locale settings interface
//
///////////////////////////////////////////////////////////////////

Y2PM::LocaleFallback Y2PM::getLocaleFallback( const LangCode & locale_r ) {
  LocaleFallback ret;

  if ( locale_r->empty() ) {
    // _locale_fallback only
    ret.push_back( LocaleSettings::_locale_fallback );
  } else {

    // locale_r first
    ret.push_back( locale_r );

    if ( locale_r.hasCountry() ) {
      // country code stripped
      ret.push_back( locale_r.languageOnly() );
    }

    if ( ret.back() != LocaleSettings::_locale_fallback ) {
      // _locale_fallback if not yet present
      ret.push_back( LocaleSettings::_locale_fallback );
    }

  }
  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::getPreferredLocale
//	METHOD TYPE : const LangCode &
//
const LangCode & Y2PM::getPreferredLocale()
{
  return localeSettings()._preferred_locale;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::setPreferredLocale
//	METHOD TYPE : void
//
PMError Y2PM::setPreferredLocale( const LangCode & preferred_locale_r )
{
  LangCode newLocale( preferred_locale_r );
  if ( newLocale->empty() ) {
    newLocale = LocaleSettings::_locale_fallback;
  }
  if ( newLocale != localeSettings()._preferred_locale ) {
    localeSettings()._preferred_locale = newLocale;
    MIL << "New preferred locale: '" << newLocale << "'" << endl;
  }
  return PMError::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::getRequestedLocales
//	METHOD TYPE : const LocaleSet &
//
const Y2PM::LocaleSet & Y2PM::getRequestedLocales()
{
  return localeSettings()._requested_locales;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::setRequestedLocales
//	METHOD TYPE : void
//
PMError Y2PM::setRequestedLocales( const Y2PM::LocaleSet & requested_locales_r )
{
#warning Checks and callbacks needed
  ///////////////////////////////////////////////////////////////////

  localeSettings()._requested_locales = requested_locales_r;
  MIL << "New requested locale: {";
  for ( LocaleSet::const_iterator it = requested_locales_r.begin(); it != requested_locales_r.end(); ++it ) {
    MIL << " " << *it;
  }
  MIL << " }" << endl;
  return PMError::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::addRequestedLocales
//	METHOD TYPE : void
//
PMError Y2PM::addRequestedLocales( const LocaleSet & requested_locales_r )
{
  LocaleSet tmpset( localeSettings()._requested_locales );
  tmpset.insert( requested_locales_r.begin(), requested_locales_r.end() );
  return setRequestedLocales( tmpset );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::delRequestedLocales
//	METHOD TYPE : void
//
PMError Y2PM::delRequestedLocales( const LocaleSet & requested_locales_r )
{
  LocaleSet tmpset( localeSettings()._requested_locales );
  for ( LocaleSet::const_iterator it = requested_locales_r.begin(); it != requested_locales_r.end(); ++it ) {
    tmpset.erase( *it );
  }
  return setRequestedLocales( tmpset );
}

