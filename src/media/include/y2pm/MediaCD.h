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

   Purpose:	Implementation class for CD/DVD MediaHandler

/-*/
#ifndef MediaCD_h
#define MediaCD_h

#include <y2pm/MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaCD
/**
 * @short Implementation class for CD/DVD MediaHandler
 * @see MediaHandler
 **/
class MediaCD : public MediaHandler {

  private:

    unsigned long _mountflags;

    typedef std::list<std::string> DeviceList;
    /** list of devices to try to mount */
    DeviceList _devices;

    /** which device has been mounted */
    std::string _mounteddevice;

    /** number of last successful mounted device in list */
    int _lastdev;

  protected:

    MEDIA_HANDLER_API;

  public:

    MediaCD( const Url &      url_r,
	     const Pathname & attach_point_hint_r,
	     MediaAccess::MediaType type_r );

    virtual ~MediaCD() { release(); }
};

///////////////////////////////////////////////////////////////////

#endif // MediaCD_h
