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

   File:	MediaSMB.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to CD or DVD media
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/Mount.h>
#include <y2pm/MediaSMB.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaSMB
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::MediaSMB
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaSMB::MediaSMB( const Url &      url_r,
		    const Pathname & attach_point_hint_r,
		    MediaAccess::MediaType type_r )
    : MediaHandler( url_r, attach_point_hint_r,
		    false, // attachPoint_is_mediaroot
		    false, // does_download
		    type_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::attachTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that not already attached, and attachPoint is a directory.
//
PMError MediaSMB::attachTo(bool next)
{
    if(_url.getHost().empty())
	    return Error::E_no_host_specified;
    if(next)
	return Error::E_not_supported_by_media;

    const char* const filesystem = "smbfs";
    const char *mountpoint = attachPoint().asString().c_str();
    Mount mount;
    PMError ret;

    string path = "//";
    path += _url.getHost();
    path += _url.getPath();

    string options = _url.getOption("mountoptions");
    string username = _url.getUsername();
    string password = _url.getPassword();
    // need to add guest to prevent smbmount from asking for password
    if(options.empty())
    {
	options="ro,guest";
    }
    else if ( password.empty()
	    && options.find("guest") == string::npos
	    && options.find("credentials") == string::npos
	    && options.find("password") == string::npos )
    {
	options += ",guest";
    }

    if(!username.empty())
	options += ",username=" + username;

    if(!password.empty())
	options += ",password=" + password;

    MIL << "try mount " << path
	<< " to " << mountpoint
	<< " filesystem " << filesystem << ": ";

    ret = mount.mount(path,mountpoint,filesystem,options);
    if(ret == Error::E_ok)
    {
	MIL << "succeded" << endl;
    }
    else
    {
	MIL << "failed: " <<  ret << endl;
	return ret;
    }

    return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::releaseFrom
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaSMB::releaseFrom( bool eject )
{
    MIL << "umount " << attachPoint();

    Mount mount;
    PMError ret;

    if ((ret = mount.umount(attachPoint().asString())) != Error::E_ok)
    {
	MIL << "failed: " <<  ret << endl;
	return ret;
    }

    MIL << "succeded" << endl;

    return ret;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaSMB::getFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaSMB::getFile (const Pathname & filename) const
{
  return MediaHandler::getFile( filename );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaSMB::getDirInfo( std::list<std::string> & retlist,
			      const Pathname & dirname, bool dots ) const
{
  return MediaHandler::getDirInfo( retlist, dirname, dots );
}

#if 0
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::findFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//	find file denoted by pattern
//	filename is interpreted relative to the attached url
//
//	pattern is a string with an optional trailing '*'
//

const Pathname *
MediaSMB::findFile (const Pathname & dirname, const string & pattern) const
{
    return scanDirectory (dirname, pattern);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::getInfo
//	METHOD TYPE : const PathInfo *
//
//	DESCRIPTION :
//	get file information

const PathInfo *
MediaSMB::fileInfo (const Pathname & filename) const
{
    // no retrieval needed, CD is mounted at destination
    return new PathInfo (filename);
}
#endif
