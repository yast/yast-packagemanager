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

   File:	MediaDISK.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to DISK media
/-*/
#ifndef MediaDISK_h
#define MediaDISK_h

#include <MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaDISK
class MediaDISK : public MediaHandler {

	unsigned long _mountflags;

    public:
	// constructor

	MediaDISK (const string & partition, const string & path, const string & options);

	MEDIA_HANDLER_API

	~MediaDISK();

    public:

	std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // MediaDISK_h
