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

  File:       RpmDbCallbacks.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Callbacks triggered by RpmDb

/-*/
#ifndef RpmDbCallbacks_h
#define RpmDbCallbacks_h

#include <iosfwd>

#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMCallbacks.h>

///////////////////////////////////////////////////////////////////
namespace RpmDbCallbacks {
///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Reporting progress of rpm database conversion.
  ///////////////////////////////////////////////////////////////////
  struct ConvertDbCallback : public RedirectCallback<ConvertDbCallback> {
    virtual void start( const Pathname & v3db ) = 0;
    virtual void progress( const ProgressData & prg,
			   unsigned failed, unsigned ignored, unsigned alreadyInV4 ) = 0;
    virtual void dbInV4( const std::string & pkg ) = 0;
    /**
     * PROCEED: Continue to see if more errors occur, but discard new db.
     * SKIP:    Ignore error and continue.
     * CANCEL:  Stop conversion and discard new db.
     **/
    virtual CBSuggest dbReadError( int offset ) = 0;
    virtual void dbWriteError( const std::string & pkg ) = 0;
    virtual void stop( PMError error ) = 0;
  };

  class ConvertDbReport : public Report<ConvertDbCallback> {
    virtual void start( const Pathname & v3db ) {
      ConvertDbCallback::start( v3db );
    }
    virtual void progress( const ProgressData & prg,
			   unsigned failed, unsigned ignored, unsigned alreadyInV4 ) {
      ConvertDbCallback::progress( prg, failed, ignored, alreadyInV4 );
    }
    virtual void dbInV4( const std::string & pkg ) {
      ConvertDbCallback::dbInV4( pkg );
    }
    virtual CBSuggest dbReadError( int offset ) {
      return ConvertDbCallback::dbReadError( offset );
    }
    virtual void dbWriteError( const std::string & pkg ) {
      ConvertDbCallback::dbWriteError( pkg );
    }
    virtual void stop( PMError error ) {
      ConvertDbCallback::stop( error );
    }
  };

  extern ConvertDbReport convertDbReport;

  ///////////////////////////////////////////////////////////////////
  // Reporting progress of rpm database rebuild.
  ///////////////////////////////////////////////////////////////////
  struct RebuildDbCallback : public RedirectCallback<RebuildDbCallback> {
    virtual void start() = 0;
    virtual void progress( const ProgressData & prg ) = 0;
    virtual void notify( const std::string & msg ) = 0;
    virtual void stop( PMError error ) = 0;
  };

  class RebuildDbReport : public Report<RebuildDbCallback> {
    virtual void start() {
      RebuildDbCallback::start();
    }
    virtual void progress( const ProgressData & prg ) {
      RebuildDbCallback::progress( prg );
    }
    virtual void notify( const std::string & msg ) {
      RebuildDbCallback::notify( msg );
    }
    virtual void stop( PMError error ) {
      RebuildDbCallback::stop( error );
    }
  };

  extern RebuildDbReport rebuildDbReport;

  ///////////////////////////////////////////////////////////////////
  // Reporting progress reading the rpm database
  ///////////////////////////////////////////////////////////////////
  struct ScanDbCallback : public RedirectCallback<ScanDbCallback> {
    virtual void start() = 0;
    virtual void progress( const ProgressData & prg ) = 0;
    virtual void stop( PMError error ) = 0;
  };

  class ScanDbReport : public Report<ScanDbCallback> {
    virtual void start() {
      ScanDbCallback::start();
    }
    virtual void progress( const ProgressData & prg ) {
      ScanDbCallback::progress( prg );
    }
    virtual void stop( PMError error ) {
      ScanDbCallback::stop( error );
    }
  };

  extern ScanDbReport scanDbReport;

  ///////////////////////////////////////////////////////////////////
  // Reporting progress of rpm package installation.
  ///////////////////////////////////////////////////////////////////
  struct InstallPkgCallback : public RedirectCallback<InstallPkgCallback> {
    virtual void start( const Pathname & filename ) = 0;
    virtual void progress( const ProgressData & prg ) = 0;
    virtual void stop( PMError error ) = 0;
  };

  class InstallPkgReport : public Report<InstallPkgCallback> {
    virtual void start( const Pathname & filename ) {
      InstallPkgCallback::start( filename );
    }
    virtual void progress( const ProgressData & prg ) {
      InstallPkgCallback::progress( prg );
    }
    virtual void stop( PMError error ) {
      InstallPkgCallback::stop( error );
    }
  };

  extern InstallPkgReport installPkgReport;

  ///////////////////////////////////////////////////////////////////
  // Reporting progress of rpm package deletion.
  ///////////////////////////////////////////////////////////////////
  struct RemovePkgCallback : public RedirectCallback<RemovePkgCallback> {
    virtual void start( const std::string & label ) = 0;
    virtual void progress( const ProgressData & prg ) = 0;
    virtual void stop( PMError error ) = 0;
  };

  class RemovePkgReport : public Report<RemovePkgCallback> {
    virtual void start( const std::string & label ) {
      RemovePkgCallback::start( label );
    }
    virtual void progress( const ProgressData & prg ) {
      RemovePkgCallback::progress( prg );
    }
    virtual void stop( PMError error ) {
      RemovePkgCallback::stop( error );
    }
  };

  extern RemovePkgReport removePkgReport;

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
} // namespace RpmDbCallbacks
///////////////////////////////////////////////////////////////////

#endif // RpmDbCallbacks_h
