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

	string _user;		// username
	string _pass;		// password
	string _proxy;		// proxy server
	string _port;		// proxy port
	string _proxytype;	// proxy type ("ftp", "http")

    public:
	// constructor

	MediaFTP (const string & server, const string & path, const string & options);

	MEDIA_HANDLER_API

	~MediaFTP();

    public:

	std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // MediaFTP_h
