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

  File:       MediaCallbacks.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Callbacks triggered by Media

/-*/

#include <iostream>

#include <y2pm/MediaCallbacks.h>

using namespace std;

///////////////////////////////////////////////////////////////////
namespace MediaCallbacks {
///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Reporting progress of download
  ///////////////////////////////////////////////////////////////////
  DownloadProgressReport downloadProgressReport;

  void DownloadProgressCallback::start( const Url & url_r, const Pathname & localpath_r ) {
  }
  bool DownloadProgressCallback::progress( const ProgressData & prg ) {
    return true; // continue download
  }
  void DownloadProgressCallback::stop( PMError error ) {
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
} // namespace MediaCallbacks
///////////////////////////////////////////////////////////////////
