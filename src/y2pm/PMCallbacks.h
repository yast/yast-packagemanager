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

  File:       PMCallbacks.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Provides some types frequently used in callback settings.

/-*/
#ifndef PMCallbacks_h
#define PMCallbacks_h

#include <iosfwd>

#include <y2util/CallBack.h>
#include <y2util/Pathname.h>
#include <y2util/ProgressCounter.h>

#include <y2pm/PMError.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : CBSuggest
/**
 * @short Enum value for YCPCallaback response (PROCEED, RETRY, SKIP, CANCEL)
 *
 * <CODE>CBSuggest</CODE> is nothing more than an enum value of
 * <CODE>CBSuggest::Action</CODE>. The only purpose for wraping
 * it into a class is the construction from a std::string.
 *
 * Use <CODE>PROCEED</CODE> if you've got no special suggestion.
 **/
class CBSuggest {
  friend std::ostream & operator<<( std::ostream & str, const CBSuggest & obj )
    { return str << obj.action; }

  public:

    /**
     * Action values
     **/
    enum Action {
      PROCEED,
      RETRY,
      SKIP,
      CANCEL
    };

    friend std::ostream & operator<<( std::ostream & str, const Action & obj );

  private:

    /**
     * Action value
     **/
    Action action;

  public:

    /**
     * Constructor. Defaults to <CODE>PROCEED</CODE>.
     **/
    CBSuggest( const Action & action_r = PROCEED )
      : action( action_r )
    {}
    /**
     * Constructor. You may use any unique abbreviation of an enum
     * value in upper, lower or mixed case. Empty or unknown values
     * default to <CODE>PROCEED</CODE>.
     **/
    explicit CBSuggest( const std::string & str_r );

    /**
     * Conversion to Action value.
     **/
    operator const Action &() const { return action; }
};

///////////////////////////////////////////////////////////////////

#endif // PMCallbacks_h
