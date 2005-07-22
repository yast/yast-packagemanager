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
#include <y2pm/MediaSMB.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaCIFS
/**
 * @short Implementation class for CIFS MediaHandler
 *
 * NOTE: It's actually MediaSMB, but using "cifs"
 * as vfstype for mount.
 * @see MediaHandler
 **/
class MediaCIFS : public MediaSMB {

  public:

    MediaCIFS( const Url&       url_r,
	       const Pathname & attach_point_hint_r )
        : MediaSMB( url_r, attach_point_hint_r )
    {
      mountAsCIFS();
    }
};

///////////////////////////////////////////////////////////////////

#endif // MediaCIFS_h
