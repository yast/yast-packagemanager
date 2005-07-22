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

   File:	MediaDIR.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for DIR MediaHandler.

/-*/
#ifndef MediaDIR_h
#define MediaDIR_h

#include <y2pm/MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaDIR

/**
 * @short Implementation class for DIR MediaHandler
 * @see MediaHandler
 **/
class MediaDIR : public MediaHandler {

  protected:

    MEDIA_HANDLER_API;

  public:

    MediaDIR( const Url &      url_r,
	      const Pathname & attach_point_hint_r );

    virtual ~MediaDIR() { release(); }
};

///////////////////////////////////////////////////////////////////

#endif // MediaDIR_h
