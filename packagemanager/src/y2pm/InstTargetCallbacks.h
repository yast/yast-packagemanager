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

  File:       InstTargetCallbacks.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Callbacks triggered by InstTarget

/-*/
#ifndef InstTargetCallbacks_h
#define InstTargetCallbacks_h

#include <iosfwd>

#include <y2pm/PMCallbacks.h>

///////////////////////////////////////////////////////////////////
namespace InstTargetCallbacks {
///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Reporting progress of script execution.
  ///////////////////////////////////////////////////////////////////
  struct ScriptExecCallback : public RedirectCallback<ScriptExecCallback> {
    virtual void start( const Pathname & pkpath ) = 0;
    /**
     * Execution time is unpredictable. ProgressData range will be set to
     * [0:0]. Aprox. every half second progress is reported with incrementing
     * counter value. If <CODE>false</CODE> is returned, execution is canceled.
     **/
    virtual bool progress( const ProgressData & prg ) = 0;
    virtual void stop( PMError error ) = 0;
  };

  class ScriptExecReport : public Report<ScriptExecCallback> {
    virtual void start( const Pathname & pkpath ) {
      ScriptExecCallback::start( pkpath );
    }
    virtual bool progress( const ProgressData & prg ) {
      return ScriptExecCallback::progress( prg );
    }
    virtual void stop( PMError error ) {
      ScriptExecCallback::stop( error );
    }
  };

  extern ScriptExecReport scriptExecReport;

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
} // namespace InstTargetCallbacks
///////////////////////////////////////////////////////////////////

#endif // InstTargetCallbacks_h
