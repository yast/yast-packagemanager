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

   File:	MediaHTTP.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to HTTP media
/-*/
#ifndef MediaHTTP_h
#define MediaHTTP_h

#include <y2pm/MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaHTTP
class MediaHTTP : public MediaHandler {

	std::string _proxy;		// http proxy server
	int _port;		// proxy port

	// if authorization required

	std::string _user;		// username
	std::string _pass;		// password

    public:
	// constructor

	MediaHTTP (const std::string & server, const std::string & path, const std::string & options);

	MEDIA_HANDLER_API

	~MediaHTTP();

    public:

	std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // MediaHTTP_h
