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

   Purpose:	Implementation class for MediaHandler
		This class handles access to SMB media
/-*/
#ifndef MediaSMB_h
#define MediaSMB_h

#include <string>

#include <y2pm/MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaSMB
class MediaSMB : public MediaHandler {

    public:
	// constructor

	MediaSMB (const Url& url);

	MEDIA_HANDLER_API

	~MediaSMB();

    public:

	std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // MediaSMB_h
