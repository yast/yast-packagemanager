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

   File:	MediaFTP.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to FTP media
/-*/
#ifndef MediaFTP_h
#define MediaFTP_h

#include <y2pm/MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaFTP
class MediaFTP : public MediaHandler {

	std::string _user;		// username
	std::string _pass;		// password
	std::string _proxy;		// proxy server
	std::string _port;		// proxy port
	std::string _proxytype;	// proxy type ("ftp", "http")

    public:
	// constructor

	MediaFTP (const std::string & server, const std::string & path, const std::string & options);

	MEDIA_HANDLER_API

	~MediaFTP();

    public:

	std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // MediaFTP_h
