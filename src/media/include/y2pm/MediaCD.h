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

   File:	MediaCD.h

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to CDROM media
/-*/
#ifndef MediaCD_h
#define MediaCD_h

#include <y2pm/MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaCD
class MediaCD : public MediaHandler {

    private:

	unsigned long _mountflags;

	typedef std::list<std::string> DeviceList;
	/** list of devices to try to mount */
	DeviceList _devices;
	
	/** which device has been mounted */
	std::string _mounteddevice;

    public:
	// constructor

	MediaCD (const Url& url);

	MEDIA_HANDLER_API

	~MediaCD();

    public:

	std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // MediaCD_h
