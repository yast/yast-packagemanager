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

   File:       MediaInfo.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef MediaInfo_h
#define MediaInfo_h

#include <iosfwd>

#include <y2pm/MediaInfoPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaInfo
/**
 *
 **/
class MediaInfo : virtual public Rep {
  REP_BODY(MediaInfo);

  public:

    MediaInfo();

    virtual ~MediaInfo();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // MediaInfo_h

