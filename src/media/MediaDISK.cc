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

#include <y2pm/MediaDISK.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>

using namespace std;

#define TMPMNT "/tmp/mnt"

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
MediaDISK::MediaDISK (const string & partition, const string & path, const string & options)
    : MediaHandler (partition, path)
    , _mountflags (MS_RDONLY)
{
    // parse options to _mountflags
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
    str << "MediaDISK (" << _device << "@" << _path << ")";
    return str;
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

    const char *mountpoint = to.asString().c_str();
    string dirpath = string (TMPMNT) + _path;
    const char *dirpoint = dirpath.c_str();

    if (mount (_device.c_str(), TMPMNT, "ext2", 0, 0) != 0) {
	if (mount (dirpoint, mountpoint, "ext2", _mountflags|MS_BIND, 0) != 0) {
	    return E_system;
	}
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
MediaDISK::release (void)
{
    if (umount (_attachPoint.asString().c_str()) != 0) {
	if (umount (TMPMNT) != 0) {
	    return E_system;
	}
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
    // no retrieval needed, NFS path is mounted at destination
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
