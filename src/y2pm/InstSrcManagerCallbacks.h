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

  File:       InstSrcManagerCallbacks.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Callbacks triggered by InstSrcManager

/-*/
#ifndef InstSrcManagerCallbacks_h
#define InstSrcManagerCallbacks_h

#include <iosfwd>

#include <y2pm/PMCallbacks.h>
#include <y2pm/InstSrcPtr.h>

///////////////////////////////////////////////////////////////////
namespace InstSrcManagerCallbacks {
///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Reporting need to change media
  ///////////////////////////////////////////////////////////////////
  struct MediaChangeCallback : public RedirectCallback<MediaChangeCallback> {
    /**
     * Whether a recipient is set.
     **/
    virtual bool isSet() = 0;
    /** media change callback
     * - product name (i.e "SuSE Linux Professional 8.1")
     * - product error
     * - media type (0=CD, 1=DVD, ...)
     * - expected media number
     * - found media number (0 == none)
     * return "": retry, "S": skip, "C" cancel, "I" ignore, "E" eject, else new url
     **/
    virtual std::string changeMedia( const std::string & error,
				     const std::string & url,
				     const std::string & product,
				     int current,
				     const std::string & currentLabel,
				     int expected,
				     const std::string & expectedLabel,
                                     bool doublesided ) = 0;
  };

  class MediaChangeReport : public Report<MediaChangeCallback> {
    virtual bool isSet() {
      return MediaChangeCallback::isSet();
    }
    virtual std::string changeMedia( const std::string & error,
				     const std::string & url,
				     const std::string & product,
				     int current,
				     const std::string & currentLabel,
				     int expected,
				     const std::string & expectedLabel,
                                     bool doublesided ) {
      return MediaChangeCallback::changeMedia( error, url, product,
					       current, currentLabel,
					       expected, expectedLabel,
					       doublesided );
    }
  };

  extern MediaChangeReport mediaChangeReport;

#if 0
  ///////////////////////////////////////////////////////////////////
  // Reporting @
  ///////////////////////////////////////////////////////////////////
  struct @Callback : public RedirectCallback<@Callback> {
    virtual void start() = 0;
    virtual void progress( const ProgressData & prg ) = 0;
    virtual void stop( PMError error ) = 0;
  };

  class @Report : public Report<@Callback> {
    virtual void start() {
      @Callback::start();
    }
    virtual void progress( const ProgressData & prg ) {
      @Callback::progress( prg );
    }
    virtual void stop( PMError error ) {
      @Callback::stop( error );
    }
  };

  extern @Report @Report;
#endif

///////////////////////////////////////////////////////////////////
} // namespace InstSrcManagerCallbacks
///////////////////////////////////////////////////////////////////

#endif // InstSrcManagerCallbacks_h
