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

   File:	MediaDISK.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to disk
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/Mount.h>
#include <y2pm/MediaDISK.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaDISK
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::MediaDISK
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaDISK::MediaDISK( const Url &      url_r,
		      const Pathname & attach_point_hint_r )
    : MediaHandler( url_r, attach_point_hint_r,
		    true,  // attachPoint_is_mediaroot
		    false ) // does_download
{
	_device = _url.option("device");
	_filesystem = _url.option("filesystem");
	if(_filesystem.empty())
		_filesystem="auto";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::attachTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that not already attached, and attachPoint is a directory.
//
PMError MediaDISK::attachTo(bool next)
{
    if(next)
	return Error::E_not_supported_by_media;
    // FIXME
    // do mount --bind <partition>/<dir> to <to>
    //   mount /dev/<partition> /tmp_mount
    //   mount /tmp_mount/<dir> <to> --bind -o ro
    // FIXME: try all filesystems

    if(_device.empty())
	    return Error::E_no_destination;

    if(_filesystem.empty())
	    return Error::E_invalid_filesystem;

    Mount mount;
    const char *mountpoint = attachPoint().asString().c_str();
    string options = _url.option("mountoptions");
    if(options.empty())
    {
	options="ro";
    }

    PMError ret = mount.mount(_device,mountpoint,_filesystem,options);
    if( ret != Error::E_ok )
    {
	return ret;
    }

    return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::releaseFrom
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaDISK::releaseFrom( bool eject )
{
    Mount mount;
    PMError ret;

    if ((ret = mount.umount(attachPoint().asString())) != Error::E_ok)
    {
	return ret;
    }

    return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaDISK::getFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaDISK::getFile (const Pathname & filename) const
{
  return MediaHandler::getFile( filename );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaDISK::getDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaDISK::getDir( const Pathname & dirname, bool recurse_r ) const
{
  return MediaHandler::getDir( dirname, recurse_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaDISK::getDirInfo( std::list<std::string> & retlist,
			     const Pathname & dirname, bool dots ) const
{
  return MediaHandler::getDirInfo( retlist, dirname, dots );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaDISK::getDirInfo( PathInfo::dircontent & retlist,
			   const Pathname & dirname, bool dots ) const
{
  return MediaHandler::getDirInfo( retlist, dirname, dots );
}
