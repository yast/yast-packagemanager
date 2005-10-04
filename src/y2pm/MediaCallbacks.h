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

  File:       MediaCallbacks.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Callbacks triggered by Media

/-*/
#ifndef MediaCallbacks_h
#define MediaCallbacks_h

#include <iosfwd>

#include <y2util/Url.h>

#include <y2pm/PMCallbacks.h>

///////////////////////////////////////////////////////////////////
namespace MediaCallbacks {
///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Reporting progress of download
  ///////////////////////////////////////////////////////////////////
  struct DownloadProgressCallback : public RedirectCallback<DownloadProgressCallback> {
    virtual void start( const Url & url_r, const Pathname & localpath_r ) = 0;
    virtual bool progress( const ProgressData & prg ) = 0;
    virtual void stop( PMError error ) = 0;
  };

  class DownloadProgressReport : public Report<DownloadProgressCallback> {
    virtual void start( const Url & url_r, const Pathname & localpath_r ) {
      DownloadProgressCallback::start( url_r, localpath_r );
    }
    virtual bool progress( const ProgressData & prg ) {
      return DownloadProgressCallback::progress( prg );
    }
    virtual void stop( PMError error ) {
      DownloadProgressCallback::stop( error );
    }
  };

  extern DownloadProgressReport downloadProgressReport;

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
} // namespace MediaCallbacks
///////////////////////////////////////////////////////////////////

#endif // MediaCallbacks_h
