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

   File:	MediaNFS.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to CD or DVD media
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/MediaNFS.h>
#include <y2pm/Mount.h>

#include <dirent.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaNFS
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::MediaNFS
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaNFS::MediaNFS( const Url &      url_r,
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
//	METHOD NAME : MediaNFS::attachTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that not already attached, and attachPoint is a directory.
//
PMError MediaNFS::attachTo()
{
    if(_url.getHost().empty())
	    return Error::E_no_host_specified;

    const char* const filesystem = "nfs";
    const char *mountpoint = attachPoint().asString().c_str();
    Mount mount;

    string path = _url.getHost();
    path += ':';
    path += _url.getPath();

    string options = _url.getOption("mountoptions");
    if(options.empty())
    {
	options="ro";
    }

    MIL << "try mount " << path
	<< " to " << mountpoint
	<< " filesystem " << filesystem << ": ";

    PMError ret = mount.mount(path,mountpoint,filesystem,options);
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
//	METHOD NAME : MediaNFS::releaseFrom
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaNFS::releaseFrom( bool eject )
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
//	METHOD NAME : MediaNFS::getFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaNFS::getFile (const Pathname & filename) const
{
  return MediaHandler::getFile( filename );;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaNFS::getDirInfo( std::list<std::string> & retlist,
			     const Pathname & dirname, bool dots ) const
{
  return MediaHandler::getDirInfo( retlist, dirname, dots );
}

#if 0
///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::findFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//	find file denoted by pattern
//	filename is interpreted relative to the attached url
//
//	pattern is a string with an optional trailing '*'
//

const Pathname *
MediaNFS::findFile (const Pathname & dirname, const string & pattern) const
{
    return scanDirectory (dirname, pattern);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::getInfo
//	METHOD TYPE : const PathInfo *
//
//	DESCRIPTION :
//	get file information

const PathInfo *
MediaNFS::fileInfo (const Pathname & filename) const
{
    // no retrieval needed, CD is mounted at destination
    return new PathInfo (filename);
}
#endif
