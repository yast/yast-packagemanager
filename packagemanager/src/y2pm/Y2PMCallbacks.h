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

  File:       Y2PMCallbacks.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Callbacks triggered by Y2PM

/-*/
#ifndef Y2PMCallbacks_h
#define Y2PMCallbacks_h

#include <iosfwd>

#include <y2util/FSize.h>

#include <y2pm/InstSrcPtr.h>

#include <y2pm/PMCallbacks.h>
#include <y2pm/RpmDbCallbacks.h>
#include <y2pm/MediaCallbacks.h>
#include <y2pm/InstTargetCallbacks.h>
#include <y2pm/InstSrcManagerCallbacks.h>
#include <y2pm/PMPackageManagerCallbacks.h>
#include <y2pm/PMSelectionManagerCallbacks.h>

///////////////////////////////////////////////////////////////////
namespace Y2PMCallbacks {
///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Reporting messages during commitPackages
  ///////////////////////////////////////////////////////////////////
  struct CommitCallback : public RedirectCallback<CommitCallback> {
    virtual void advanceToMedia( constInstSrcPtr srcptr, unsigned mediaNr ) = 0;
  };

  class CommitReport : public Report<CommitCallback> {
    virtual void advanceToMedia( constInstSrcPtr srcptr, unsigned mediaNr ) {
      CommitCallback::advanceToMedia( srcptr, mediaNr );
    }
  };

  extern CommitReport commitReport;

  ///////////////////////////////////////////////////////////////////
  // Reporting package provision (e.g. downloading from a remote InstSsrc).
  ///////////////////////////////////////////////////////////////////
  struct CommitProvideCallback : public RedirectCallback<CommitProvideCallback> {
    virtual void start( constPMPackagePtr pkg, bool sourcepkg ) = 0;
    virtual CBSuggest attempt( unsigned cnt ) = 0;
    virtual CBSuggest result( PMError error, const Pathname & localpath ) = 0;
    virtual void stop( PMError error, const Pathname & localpath ) = 0;
  };

  class CommitProvideReport : public Report<CommitProvideCallback> {
    virtual void start( constPMPackagePtr pkg, bool sourcepkg ) {
      CommitProvideCallback::start( pkg, sourcepkg );
    }
    virtual CBSuggest attempt( unsigned cnt ) {
      return CommitProvideCallback::attempt( cnt );
    }
    virtual CBSuggest result( PMError error, const Pathname & localpath ) {
      return CommitProvideCallback::result( error, localpath );
    }
    virtual void stop( PMError error, const Pathname & localpath ) {
      CommitProvideCallback::stop( error, localpath );
    }
  };

  extern CommitProvideReport commitProvideReport;

  ///////////////////////////////////////////////////////////////////
  // Reporting rpm package installation.
  ///////////////////////////////////////////////////////////////////
  struct CommitInstallCallback : public RedirectCallback<CommitInstallCallback> {
    virtual void start( constPMPackagePtr pkg, bool sourcepkg, const Pathname & path ) = 0;
    virtual CBSuggest attempt( unsigned cnt ) = 0;
    virtual CBSuggest result( PMError error ) = 0;
    virtual void stop( PMError error ) = 0;
  };

  class CommitInstallReport : public Report<CommitInstallCallback> {
    virtual void start( constPMPackagePtr pkg, bool sourcepkg, const Pathname & path ) {
      CommitInstallCallback::start( pkg, sourcepkg, path );
    }
    virtual CBSuggest attempt( unsigned cnt ) {
      return CommitInstallCallback::attempt( cnt );
    }
    virtual CBSuggest result( PMError error ) {
      return CommitInstallCallback::result( error );
    }
    virtual void stop( PMError error ) {
      CommitInstallCallback::stop( error );
    }
  };

  extern CommitInstallReport commitInstallReport;

  ///////////////////////////////////////////////////////////////////
  // Reporting rpm package deletion.
  ///////////////////////////////////////////////////////////////////
  struct CommitRemoveCallback : public RedirectCallback<CommitRemoveCallback> {
    virtual void start( constPMPackagePtr pkg ) = 0;
    virtual CBSuggest attempt( unsigned cnt ) = 0;
    virtual CBSuggest result( PMError error ) = 0;
    virtual void stop( PMError error ) = 0;
  };

  class CommitRemoveReport : public Report<CommitRemoveCallback> {
    virtual void start( constPMPackagePtr pkg ) {
      CommitRemoveCallback::start( pkg );
    }
    virtual CBSuggest attempt( unsigned cnt ) {
      return CommitRemoveCallback::attempt( cnt );
    }
    virtual CBSuggest result( PMError error ) {
      return CommitRemoveCallback::result( error );
    }
    virtual void stop( PMError error ) {
      CommitRemoveCallback::stop( error );
    }
  };

  extern CommitRemoveReport commitRemoveReport;

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
} // namespace Y2PMCallbacks
///////////////////////////////////////////////////////////////////

#endif // Y2PMCallbacks_h
