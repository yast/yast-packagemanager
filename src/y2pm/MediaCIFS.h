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

   File:	MediaCIFS.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for CIFS MediaHandler

/-*/
#ifndef MediaCIFS_h
#define MediaCIFS_h

#include <y2pm/MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaCIFS
/**
 * @short Implementation class for CIFS MediaHandler
 * @see MediaHandler
 **/
class MediaCIFS : public MediaHandler {

  protected:

    MEDIA_HANDLER_API;

  public:

    MediaCIFS( const Url&       url_r,
	      const Pathname & attach_point_hint_r );

    virtual ~MediaCIFS() { release(); }
};

///////////////////////////////////////////////////////////////////

#endif // MediaCIFS_h
