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

   Purpose:	Implementation class for SMB MediaHandler

/-*/
#ifndef MediaSMB_h
#define MediaSMB_h

#include <y2pm/MediaHandler.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaSMB
/**
 * @short Implementation class for SMB MediaHandler
 *
 * NOTE: The implementation currently serves both, "smbfs"
 * and "cifs". The only difference is the vfstype passed to
 * the mount command.
 * @see MediaHandler
 **/
class MediaSMB : public MediaHandler {

  private:

    /**
     * vfstype for mount. This is either "smbfs"
     * or "cifs" (rewritten by MediaCIFS).
     **/
    const char* _vfstype;

  protected:

    MEDIA_HANDLER_API;

    /**
     * MediaCIFS rewrites the vfstype to "cifs"
     * within it's constructor.
     **/
    void mountAsCIFS() { _vfstype = "cifs"; }

  public:

    MediaSMB( const Url&       url_r,
	      const Pathname & attach_point_hint_r );

    virtual ~MediaSMB() { release(); }
};

///////////////////////////////////////////////////////////////////

#endif // MediaSMB_h
