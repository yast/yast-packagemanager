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

   File:	MediaNFS.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for NFS MediaHandler

/-*/
#ifndef MediaNFS_h
#define MediaNFS_h

#include <y2pm/MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaNFS
/**
 * @short Implementation class for NFS MediaHandler
 * @see MediaHandler
 **/
class MediaNFS : public MediaHandler {

  protected:

    MEDIA_HANDLER_API;

  public:

    MediaNFS( const Url&       url_r,
	      const Pathname & attach_point_hint_r );

    virtual ~MediaNFS() { release(); }
};

///////////////////////////////////////////////////////////////////

#endif // MediaNFS_h
