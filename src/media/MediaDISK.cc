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
		This class handles access to CD or DVD media
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
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
MediaDISK::MediaDISK (const Url& url)
    : MediaHandler (url)
    , _mountflags (MS_RDONLY)
{
	_device = _url.getOption("device");
	_filesystem = _url.getOption("filesystem");
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::~MediaDISK
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaDISK::~MediaDISK()
{
    if (_attachPoint != "") {
	release ();
    }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
MediaDISK::dumpOn( ostream & str ) const
{
    return MediaHandler::dumpOn(str);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::attachTo
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : attach media to path
//
MediaResult
MediaDISK::attachTo (const Pathname & to)
{
    // FIXME
    // do mount --bind <partition>/<dir> to <to>
    //   mount /dev/<partition> /tmp_mount
    //   mount /tmp_mount/<dir> <to> --bind -o ro    
    // FIXME: try all filesystems

    if(!_url.isValid())
	    return E_bad_url;

    if(_device.empty())
	    return E_no_destination;

    if(_filesystem.empty())
	    return E_invalid_filesystem;

    const char *mountpoint = to.asString().c_str();

    MIL << "try mount " << _device
	<< " to " << mountpoint
	<< " filesystem " << _filesystem << ": ";
    if(!::mount (_device.c_str(), mountpoint, _filesystem.c_str(), _mountflags, NULL))
    {
	MIL << "succeded" << endl;
    }
    else
    {
	D__ << strerror(errno) << endl;
	MIL << "failed" << endl;
	return E_system;
    }

    _attachPoint = to;

    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::release
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : release attached media
//
MediaResult
MediaDISK::release (bool eject)
{
    if(_attachPoint.asString().empty())
    {
	return E_not_attached;
    }
    
    MIL << "umount " << _attachPoint.asString() << endl;

    if (umount (_attachPoint.asString().c_str()) != 0) {
	    return E_system;
    }

    _attachPoint = "";
    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaDISK::provideFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	provide file denoted by path to 'attached path'
//	filename is interpreted relative to the attached url
//	and a path prefix is preserved to destination

MediaResult
MediaDISK::provideFile (const Pathname & filename) const
{
    // no retrieval needed, disk is mounted at destination
    if(!_url.isValid())
	return E_bad_url;

    if(_attachPoint.asString().empty())
	return E_not_attached;

    Pathname src = _attachPoint;
    src += _url.getPath();
    src += filename;

    PathInfo info(src);
    
    if(!info.isFile())
    {
	    D__ << src.asString() << " does not exist" << endl;
	    return E_file_not_found;
    }

    return E_none;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::findFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	find file denoted by pattern
//	filename is interpreted relative to the attached url
//
//	pattern is a string with an optional trailing '*'
//

const Pathname *
MediaDISK::findFile (const Pathname & dirname, const string & pattern) const
{
    return scanDirectory (dirname, pattern);
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaDISK::getDirectory
//	METHOD TYPE : const std::list<std::string> *
//
//	DESCRIPTION :
//	get directory denoted by path to Attribute::A_StringArray

const std::list<std::string> *
MediaDISK::dirInfo (const Pathname & dirname) const
{
    return readDirectory (dirname);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::getInfo
//	METHOD TYPE : const PathInfo *
//
//	DESCRIPTION :
//	get file information

const PathInfo *
MediaDISK::fileInfo (const Pathname & filename) const
{
    // no retrieval needed, CD is mounted at destination
    return new PathInfo (filename);
}
