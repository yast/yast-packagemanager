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

#include <MediaSMB.h>

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
MediaSMB::MediaSMB (const string & server, const string & path, const string & options)
    : MediaHandler (server, path)
    , _mountflags (MS_RDONLY)
{
    // parse options to _mountflags
    // "user=<username>,pass=<password>,domain=<smbdomain>"
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::~MediaSMB
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaSMB::~MediaSMB()
{
    if (_attachedTo != "") {
	release ();
    }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & MediaSMB::dumpOn( ostream & str ) const
{
    str << "MediaSMB (" << _server << "@" << _path << ")";
    return str;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::attach
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : attach media at path
//
MediaResult MediaSMB::attach (const Pathname & to)
{
    
    const char *mountpoint = to.asString().c_str();
    if (mount (_server.c_str(), mountpoint, "smbfs", _mountflags, 0) != 0) {
	return E_system;
    }
    _attachedTo = to;

    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::release
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : release attached media
//
MediaResult MediaSMB::release (void)
{
    if (umount (_attachedTo.asString().c_str()) != 0) {
	return E_system;
    }
    _attachedTo = "";
    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaSMB::getFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	get file denoted by path to 'attached path'
//	filename is interpreted relative to the attached url
//	and a path prefix is preserved to destination

MediaResult MediaSMB::getFile (const Pathname & filename) const
{
    // no retrieval needed, SMB path is mounted at destination
    return E_none;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::findFile
//	METHOD TYPE : MediaResult
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
//	METHOD NAME : MediaSMB::getDirectory
//	METHOD TYPE : const Attribute &
//
//	DESCRIPTION :
//	get directory denoted by path to Attribute::A_StringArray

const Attribute *
MediaSMB::dirInfo (const Pathname & dirname) const
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
