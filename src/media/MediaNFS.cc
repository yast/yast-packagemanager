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

#include <MediaNFS.h>

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
MediaNFS::MediaNFS (const string & server, const string & path, const string & options)
    : MediaHandler (server, path)
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
    if (_attachedTo != "") {
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
ostream & MediaNFS::dumpOn( ostream & str ) const
{
    str << "MediaNFS (" << _server << "@" << _path << ")";
    return str;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::attach
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : attach media at path
//
MediaResult MediaNFS::attach (const Pathname & to)
{
    
    const char *mountpoint = to.asString().c_str();
    if (mount (_server.c_str(), mountpoint, "nfs", _mountflags, 0) != 0) {
	return E_system;
    }
    _attachedTo = to;

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
MediaResult MediaNFS::release (void)
{
    if (umount (_attachedTo.asString().c_str()) != 0) {
	return E_system;
    }
    _attachedTo = "";
    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaNFS::getFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	get file denoted by path to 'attached path'
//	filename is interpreted relative to the attached url
//	and a path prefix is preserved to destination

MediaResult MediaNFS::getFile (const Pathname & filename) const
{
    // no retrieval needed, NFS path is mounted at destination
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
//	METHOD TYPE : const Attribute &
//
//	DESCRIPTION :
//	get directory denoted by path to Attribute::A_StringArray

const Attribute *
MediaNFS::dirInfo (const Pathname & dirname) const
{
    Attribute *saattr = new Attribute (Attribute::A_StringArray);

    // prepend mountpoint to dirname
    Pathname fullpath = _attachedTo + dirname;

    // open mounted directory
    DIR *dir = opendir (fullpath.asString().c_str());
    struct dirent *entry;
    if (dir == 0)
    {
	return 0;
    }
    while ((entry = readdir (dir)) != 0)
    {
	saattr->add (entry->d_name);
    }
    closedir (dir);
    return saattr;
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
