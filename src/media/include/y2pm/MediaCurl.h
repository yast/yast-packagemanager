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

   File:	MediaCurl.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for FTP, HTTP and HTTPS MediaHandler

/-*/
#ifndef MediaCurl_h
#define MediaCurl_h

#include <y2pm/MediaHandler.h>

#include <curl/curl.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaCurl
/**
 * @short Implementation class for FTP, HTTP and HTTPS MediaHandler
 * @see MediaHandler
 **/
class MediaCurl : public MediaHandler {

  protected:

    MEDIA_HANDLER_API;

  public:

    MediaCurl( const Url &      url_r,
	       const Pathname & attach_point_hint_r,
	       MediaAccess::MediaType type_r );

    virtual ~MediaCurl() { release(); }

    static void setCookieFile( const Pathname & );

    class Callbacks
    {
      public:
        virtual bool progress( int percent ) = 0;
    };

    static void setCallbacks( Callbacks *c ) { _callbacks = c; }

  protected:
    static int progressCallback( void *clientp, double dltotal, double dlnow,
                                 double ultotal, double ulnow );

  private:
    CURL *_curl;
    char _curlError[ CURL_ERROR_SIZE ]; 
    
    std::string _userpwd;
    std::string _proxy;
    std::string _proxyuserpwd;

    static Pathname _cookieFile;

    static Callbacks *_callbacks;
};

///////////////////////////////////////////////////////////////////

#endif // MediaCurl_h
