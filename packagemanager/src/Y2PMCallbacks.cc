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

  File:       Y2PMCallbacks.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Callbacks triggered by Y2PM

/-*/

#include <iostream>

#include <y2pm/Y2PMCallbacks.h>

using namespace std;

///////////////////////////////////////////////////////////////////
namespace Y2PMCallbacks {
///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Reporting messages during commitPackages
  ///////////////////////////////////////////////////////////////////
  CommitReport commitReport;

  void CommitCallback::advanceToMedia( constInstSrcPtr srcptr, unsigned mediaNr ) {
  }

  ///////////////////////////////////////////////////////////////////
  // Reporting package provision (e.g. downloading from a remote InstSsrc).
  ///////////////////////////////////////////////////////////////////
  CommitProvideReport commitProvideReport;

  void CommitProvideCallback::start( constPMPackagePtr pkg, bool sourcepkg ) {
  }
  CBSuggest CommitProvideCallback::attempt( unsigned cnt ) {
    return CBSuggest::PROCEED;
  }
  CBSuggest CommitProvideCallback::result( PMError error, const Pathname & localpath ) {
    return CBSuggest::PROCEED;
  }
  void CommitProvideCallback::stop( PMError error, const Pathname & localpath ) {
  }

  ///////////////////////////////////////////////////////////////////
  // Reporting rpm package installation.
  ///////////////////////////////////////////////////////////////////
  CommitInstallReport commitInstallReport;

  void CommitInstallCallback::start( constPMPackagePtr pkg, bool sourcepkg, const Pathname & path ) {
  }
  CBSuggest CommitInstallCallback::attempt( unsigned cnt ) {
    return CBSuggest::PROCEED;
  }
  CBSuggest CommitInstallCallback::result( PMError error ) {
    return CBSuggest::PROCEED;
  }
  void CommitInstallCallback::stop( PMError error ) {
  }

  ///////////////////////////////////////////////////////////////////
  // Reporting rpm package deletion.
  ///////////////////////////////////////////////////////////////////
  CommitRemoveReport commitRemoveReport;

  void CommitRemoveCallback::start( constPMPackagePtr pkg ) {
  }
  CBSuggest CommitRemoveCallback::attempt( unsigned cnt ) {
    return CBSuggest::PROCEED;
  }
  CBSuggest CommitRemoveCallback::result( PMError error ) {
    return CBSuggest::PROCEED;
  }
  void CommitRemoveCallback::stop( PMError error ) {
  }

#if 0
  ///////////////////////////////////////////////////////////////////
  // Reporting @
  ///////////////////////////////////////////////////////////////////
  @Report @Report;

  void @Callback::start() {
  }
  void @Callback::progress( const ProgressData & prg ) {
  }
  void @Callback::stop( PMError error ) {
  }
#endif

///////////////////////////////////////////////////////////////////
} // namespace Y2PMCallbacks
///////////////////////////////////////////////////////////////////
