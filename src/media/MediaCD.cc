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

   File:	MediaCD.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to CD or DVD media
/-*/

#include <iostream>

#include <y2pm/MediaCD.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaCD
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::MediaCD
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaCD::MediaCD (const string & device, const string & path, const string & options, bool as_dvd)
    : MediaHandler (device, path)
    , _mountflags (MS_RDONLY)
    , _as_dvd (as_dvd)
{
    // parse options to _mountflags
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::~MediaCD
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaCD::~MediaCD()
{
    if (_attachPoint != "") {
	release ();
    }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
MediaCD::dumpOn( ostream & str ) const
{
    str << "MediaCD (" << _device << "@" << _path << ")";
    return str;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::attachTo
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : attach media at path
//
MediaResult
MediaCD::attachTo (const Pathname & to)
{
    // FIXME, issue "eject -t" to close the tray
    
    // if DVD, try UDF filesystem before iso9660

    const char *mountpoint = to.asString().c_str();
    if (!_as_dvd
	|| (mount (_device.c_str(), mountpoint, "udf", _mountflags, 0) != 0)) {

	if (mount (_device.c_str(), mountpoint, "iso9660", _mountflags, 0) != 0) {
	    return E_system;
	}
    }
    _attachPoint = to;

    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::release
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : release attached media
//
MediaResult
MediaCD::release (bool eject)
{
    if (umount (_attachPoint.asString().c_str()) != 0) {
	return E_system;
    }
    // FIXME implement 'eject'
    _attachPoint = "";
    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaCD::provideFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	get file denoted by path to 'attached path'
//	filename is interpreted relative to the attached url
//	and a path prefix is preserved to destination

MediaResult
MediaCD::provideFile (const Pathname & filename) const
{
    // no retrieval needed, CD is mounted at destination
    return E_none;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::findFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	find file denoted by pattern
//	filename is interpreted relative to the attached url
//
//	pattern is a string with an optional trailing '*'
//

const Pathname *
MediaCD::findFile (const Pathname & dirname, const string & pattern) const
{
    return scanDirectory (dirname, pattern);
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaCD::getDirectory
//	METHOD TYPE : const std::list<std::string> *
//
//	DESCRIPTION :
//	get directory denoted by path to Attribute::A_StringArray

const std::list<std::string> *
MediaCD::dirInfo (const Pathname & dirname) const
{
    return readDirectory (dirname);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::getInfo
//	METHOD TYPE : const PathInfo *
//
//	DESCRIPTION :
//	get file information

const PathInfo *
MediaCD::fileInfo (const Pathname & filename) const
{
    // no retrieval needed, CD is mounted at destination
    return new PathInfo (filename);
}
