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

  File:       InstSrcManagerCallbacks.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Callbacks triggered by InstSrcManager

/-*/

#include <iostream>

#include <y2pm/InstSrcManagerCallbacks.h>

using namespace std;

///////////////////////////////////////////////////////////////////
namespace InstSrcManagerCallbacks {
///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Reporting need to change media
  ///////////////////////////////////////////////////////////////////
  MediaChangeReport mediaChangeReport;

  bool MediaChangeCallback::isSet() {
    return false;
  }
  string MediaChangeCallback::changeMedia( constInstSrcPtr instSrc,
					   const string & error,
					   const string & url,
					   const string & product,
					   int current,
					   int expected ) {
    return "";
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
} // namespace InstSrcManagerCallbacks
///////////////////////////////////////////////////////////////////
