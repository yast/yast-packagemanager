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

   File:	MediaWget.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for FTP, HTTP and HTTPS MediaHandler

/-*/
#ifndef MediaWget_h
#define MediaWget_h

#include <y2pm/MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaWget
/**
 * @short Implementation class for FTP, HTTP and HTTPS MediaHandler
 * @see MediaHandler
 **/
class MediaWget : public MediaHandler {

  protected:

    MEDIA_HANDLER_API;

  public:

    MediaWget( const Url &      url_r,
	       const Pathname & attach_point_hint_r,
	       MediaAccess::MediaType type_r );

    virtual ~MediaWget() { release(); }
};

///////////////////////////////////////////////////////////////////

#endif // MediaWget_h
