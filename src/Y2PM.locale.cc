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
#include <y2util/setutil.h>
#include <y2util/LangCode.h>

#include <Y2PM.h>
#include "PMRcValues.h"

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

  private:

    LangCode _preferred_locale;

    LocaleSet _requested_locales;

  public:

    static LangCode _locale_fallback;

    /**
     * @ref purge unset value in langset and return true if langset
     * is not empty after this.
     **/
    static bool purge( LocaleSet & langset_r ) {
      langset_r.erase( LangCode() );
      return langset_r.size();
    }

  public:

    LocaleSettings()
      : _preferred_locale( _locale_fallback )
    {
      char * envlist[] = { "LC_ALL", "LC_CTYPE", "LANG", NULL };
      for ( char ** envvar = envlist; *envvar; ++envvar )
        {
          char * envlang = getenv( *envvar );
          if ( envlang )
            {
              string envstr( envlang );
              MIL << "$" << *envvar << "='" << envstr << "'" << endl;
              if ( envstr != "POSIX" && envstr != "C" )
                {
                  LangCode lang( envlang );
                  if ( lang.isSet() )
                    {
                      _preferred_locale = lang;
                      break;
                    }
                }
            }
        }
      MIL << "Preferred locale: '" << _preferred_locale << "'" << endl;
      MIL << "Fallback locale: '" << _locale_fallback << "'" << endl;
    }

    const LangCode & getPreferredLocale() const { return _preferred_locale; }

    const LocaleSet & getRequestedLocales() const { return _requested_locales; }

    public:

    /**
     * Set preferred_locale. Return true if value actually changed.
     **/
    bool setPreferredLocale( LangCode newLocale_r ) {
      if ( ! newLocale_r.isSet() ) {
	// empty -> use fallback
	newLocale_r = LocaleSettings::_locale_fallback;
      }

      if ( newLocale_r != _preferred_locale ) {
	_preferred_locale = newLocale_r;
	return true;
      }
      return false;
    }

    /**
     * Set requested_locales. Return true if value actually changed.
     * Ignored, added and deleted locale values are passed back.
     **/
    bool setRequestedLocales( const LocaleSet & newLocales_r,
			      LocaleSet & addSet_r,
			      LocaleSet & delSet_r ) {
      addSet_r.clear();
      delSet_r.clear();
      LocaleSet newSet = newLocales_r;

      if ( purge( newSet ) ) {
	addSet_r = set_difference( newSet, _requested_locales );
	delSet_r = set_difference( _requested_locales, newSet );
      }

      if ( addSet_r.size() || delSet_r.size() ) {
	_requested_locales.swap( newSet );
	return true;
      }
      return false;
    }
    /**
     * Set requested_locales. Return true if value actually changed.
     **/
    bool setRequestedLocales( const LocaleSet & newLocales_r ) {
      LocaleSet addSet_r, delSet_r;
      return setRequestedLocales( newLocales_r, addSet_r, delSet_r );
    }

    /**
     * Add addLocales_r to requested_locales. Return true if value actually changed.
     * Added locale values are passed back.
     **/
    bool addRequestedLocales( const LocaleSet & addLocales_r,
			      LocaleSet & addSet_r ) {
      addSet_r.clear();
      LocaleSet tmpSet = addLocales_r;

      if ( purge( tmpSet ) ) {
	addSet_r = set_difference( tmpSet, _requested_locales );
      }

      if ( addSet_r.size() ) {
	_requested_locales.insert( addSet_r.begin(), addSet_r.end() );
	return true;
      }
      return false;
    }
    /**
     * Add addLocales_r to requested_locales. Return true if value actually changed.
     **/
    bool addRequestedLocales( const LocaleSet & addLocales_r ) {
      LocaleSet addSet_r;
      return addRequestedLocales( addLocales_r, addSet_r );
    }

    /**
     * Delete delLocales_r from requested_locales. Return true if value actually changed.
     * Deleted locale values are passed back.
     **/
    bool delRequestedLocales( const LocaleSet & delLocales_r,
			      LocaleSet & delSet_r ) {
      delSet_r.clear();
      LocaleSet tmpSet = delLocales_r;

      if ( purge( tmpSet ) ) {
	delSet_r = set_intersection( tmpSet, _requested_locales );
      }

      if ( delSet_r.size() ) {
	tmpSet = set_difference( _requested_locales, delSet_r );
	_requested_locales.swap( tmpSet );
	return true;
      }
      return false;
    }
    /**
     * Delete delLocales_r from requested_locales. Return true if value actually changed.
     **/
    bool delRequestedLocales( const LocaleSet & delLocales_r ) {
      LocaleSet delSet_r;
      return delRequestedLocales( delLocales_r, delSet_r );
    }
};

LangCode Y2PM::LocaleSettings::_locale_fallback( "en" );

///////////////////////////////////////////////////////////////////

inline Y2PM::LocaleSettings & Y2PM::localeSettings() {
  static LocaleSettings * _localeSettings = 0;
  if ( ! _localeSettings ) {
    _localeSettings = new LocaleSettings;
    _localeSettings->setRequestedLocales( PM::rcValues().requestedLocales );
  }
  return *_localeSettings;
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::getLocaleFallback
//	METHOD TYPE : Y2PM::LocaleOrder
//
Y2PM::LocaleOrder Y2PM::getLocaleFallback( const LangCode & locale_r ) {
  LocaleOrder ret;

  if ( ! locale_r.isSet() ) {
    // _locale_fallback only
    ret.push_back( LocaleSettings::_locale_fallback );
  } else {

    // locale_r first
    ret.push_back( locale_r );

    if ( locale_r.hasCountry() ) {
      // country code stripped
      ret.push_back( locale_r.language() );
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
  return localeSettings().getPreferredLocale();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::setPreferredLocale
//	METHOD TYPE : PMError
//
PMError Y2PM::setPreferredLocale( const LangCode & preferred_locale_r )
{
  if ( localeSettings().setPreferredLocale( preferred_locale_r ) ) {
    preferredLocaleChanged();
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
  return localeSettings().getRequestedLocales();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::setRequestedLocales
//	METHOD TYPE : PMError
//
PMError Y2PM::setRequestedLocales( const Y2PM::LocaleSet & newLocales_r )
{
  LocaleSet addSet;
  LocaleSet delSet;
  if ( localeSettings().setRequestedLocales( newLocales_r, addSet, delSet ) ) {
    requestedLocalesChanged( addSet, delSet );
  }
  return PMError::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::addRequestedLocales
//	METHOD TYPE : PMError
//
PMError Y2PM::addRequestedLocales( const LocaleSet & addLocales_r )
{
  LocaleSet addSet;
  if ( localeSettings().addRequestedLocales( addLocales_r, addSet ) ) {
    LocaleSet delSet;
    requestedLocalesChanged( addSet, delSet );
  }
  return PMError::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::delRequestedLocales
//	METHOD TYPE : PMError
//
PMError Y2PM::delRequestedLocales( const LocaleSet & delLocales_r )
{
  LocaleSet delSet;
  if ( localeSettings().delRequestedLocales( delLocales_r, delSet ) ) {
    LocaleSet addSet;
    requestedLocalesChanged( addSet, delSet );
  }
  return PMError::E_ok;
}

