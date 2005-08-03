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
  string MediaChangeCallback::changeMedia( constInstSrcDescrPtr descr,
					   const Url & currentUrl,
					   int expectedMedianr,
					   PMError error ) {
    return "S";
  }
  /**
   * DEPRECATED OLD STYLE CALLBACK (used by InstYou)
   **/
  string MediaChangeCallback::changeMedia( const string & error,
					   const string & url,
					   const string & product,
					   int current,
					   const std::string & currentLabel,
					   int expected,
					   const std::string & expectedLabel,
                                           bool doublesided ) {
    return "S";
  }

  ///////////////////////////////////////////////////////////////////
  // Reporting SourceRefresh
  ///////////////////////////////////////////////////////////////////
  SourceRefreshReport sourceRefreshReport;

  void
  SourceRefreshCallback::start( constInstSrcDescrPtr descr_r )
  {}

  SourceRefreshCallback::Result
  SourceRefreshCallback::error( Error error_r,
                                const std::string & detail )
  { return SUCCESS; } // let the implementaion decide what to do

  void SourceRefreshCallback::stop( Result result_r, Cause cause_r,
                                    const std::string & detail )
  {}

#define ENUM_OUT(V) case V: return #V
  std::string SourceRefreshCallback::asString( Result val_r ) const
  {
    switch ( val_r )
      {
        ENUM_OUT( SUCCESS );
        ENUM_OUT( RERTY );
        ENUM_OUT( SKIP_REFRESH );
        ENUM_OUT( DISABLE_SOURCE );
      }
    return "?";
  }

  std::string SourceRefreshCallback::asString( Cause val_r ) const
  {
    switch ( val_r )
      {
        ENUM_OUT( REFRESH_SKIP_CD_DVD );
        ENUM_OUT( REFRESH_NOT_SUPPORTED_BY_SOURCE );
        ENUM_OUT( SOURCE_IS_UPTODATE );
        ENUM_OUT( SOURCE_REFRESHED );
        ENUM_OUT( USERREQUEST );
      }
    return "?";
  }

  std::string SourceRefreshCallback::asString( Error val_r ) const
  {
    switch ( val_r )
      {
        ENUM_OUT( NO_SOURCE_FOUND );
        ENUM_OUT( INCOMPLETE_SOURCE_DATA );
      }
    return "?";
  }
#undef ENUM_OUT

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
