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

   File:       FAKEMediaInfo.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef FAKEMediaInfo_h
#define FAKEMediaInfo_h

#include <iosfwd>

#include <y2pm/FAKEMediaInfoPtr.h>
#include <y2pm/MediaInfo.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : FAKEMediaInfo
/**
 *
 **/
class REP_CLASS(FAKEMediaInfo), public MediaInfo {
  REP_BODY(FAKEMediaInfo)

  public:

    FAKEMediaInfo();

    virtual ~FAKEMediaInfo();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // FAKEMediaInfo_h

