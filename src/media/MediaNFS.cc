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

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
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
MediaNFS::MediaNFS (const Url& url)
    : MediaHandler (url)
    , _mountflags (MS_RDONLY)
{
    // parse options to _mountflags
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::~MediaNFS
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaNFS::~MediaNFS()
{
    if (_attachPoint != "") {
	release ();
    }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
MediaNFS::dumpOn( ostream & str ) const
{
    return MediaHandler::dumpOn(str);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::attachTo
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : attach media to path
//
MediaResult
MediaNFS::attachTo (const Pathname & to)
{
    if(!_url.isValid())
	    return E_bad_url;

    if(_url.getHost().empty())
	    return E_no_host_specified;

    const char* const filesystem = "nfs";
    const char *mountpoint = to.asString().c_str();

    string path = _url.getHost();
    path += ':';
    path += _url.getPath();

    MIL << "try mount " << path
	<< " to " << mountpoint
	<< " filesystem " << filesystem << ": ";
    if(!::mount (path.c_str(), mountpoint, filesystem, _mountflags, NULL))
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
//	METHOD NAME : MediaNFS::release
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : release attached media
//
MediaResult
MediaNFS::release (bool eject)
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
//	METHOD NAME : MediaNFS::provideFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	get file denoted by path to 'attached path'
//	filename is interpreted relative to the attached url
//	and a path prefix is preserved to destination

MediaResult
MediaNFS::provideFile (const Pathname & filename) const
{
    // no retrieval needed, NFS path is mounted at destination
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
//	METHOD NAME : MediaNFS::findFile
//	METHOD TYPE : MediaResult
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
//	METHOD NAME : MediaNFS::getDirectory
//	METHOD TYPE : const std::list<std::string> *
//
//	DESCRIPTION :
//	get directory denoted by path to a string list

const std::list<std::string> *
MediaNFS::dirInfo (const Pathname & dirname) const
{
    return readDirectory (dirname);
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
