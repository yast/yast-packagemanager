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

   File:	MediaSMB.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for SMB MediaHandler

/-*/
#ifndef MediaSMB_h
#define MediaSMB_h

#include <y2pm/MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaSMB
/**
 * @short Implementation class for SMB MediaHandler
 * @see MediaHandler
 **/
class MediaSMB : public MediaHandler {

  protected:

    MEDIA_HANDLER_API;

  public:

    MediaSMB( const Url&       url_r,
	      const Pathname & attach_point_hint_r,
	      MediaAccess::MediaType type_r );

    virtual ~MediaSMB() { release(); }
};

///////////////////////////////////////////////////////////////////

#endif // MediaSMB_h
