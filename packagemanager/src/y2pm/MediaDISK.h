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

   File:	MediaDISK.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for DISK MediaHandler

/-*/
#ifndef MediaDISK_h
#define MediaDISK_h

#include <y2pm/MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaDISK
/**
 * @short Implementation class for DISK MediaHandler
 * @see MediaHandler
 **/
class MediaDISK : public MediaHandler {

  private:

    unsigned long _mountflags;

    std::string _device;
    std::string _filesystem;

  protected:

    MEDIA_HANDLER_API;

  public:

    MediaDISK( const Url &      url_r,
	       const Pathname & attach_point_hint_r );

    virtual ~MediaDISK() { release(); }
};

///////////////////////////////////////////////////////////////////

#endif // MediaDISK_h
