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

   Purpose:	Implementation class for MediaHandler
		This class handles access to directory media

/-*/
#ifndef MediaDIR_h
#define MediaDIR_h

#include <MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaDIR

class MediaDIR : public MediaHandler {

    public:
	// constructor

	MediaDIR (const string & device, const string & path, const string & options);

	MEDIA_HANDLER_API

	~MediaDIR();

    public:

	std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // MediaDIR_h
