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

   File:	MediaDIR.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to a directory
/-*/

#include <iostream>

#include <MediaDIR.h>

#include <sys/mount.h>
#include <errno.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaDIR
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::MediaDIR
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaDIR::MediaDIR (const string & device, const string & path, const string & options)
    : MediaHandler (device, path)
{
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::~MediaDIR
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaDIR::~MediaDIR()
{
    if (_attachedTo != "") {	// release if still mounted
	release ();
    }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
MediaDIR::dumpOn( ostream & str ) const
{
    str << "MediaDIR (" << "@" << _path << ")";
    return str;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::attach
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : attach media at path
//
MediaResult
MediaDIR::attach (const Pathname & to)
{
    // make directory 'present'
    _attachedTo = to;

    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::attach
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : release attached media
//
MediaResult
MediaDIR::release (void)
{
    // make directory 'vanish'
    _attachedTo = "";
    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::getFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	get file denoted by path to 'attached path'
//	filename is interpreted relative to the attached url
//	and a path prefix is preserved to destination

MediaResult
MediaDIR::getFile (const Pathname & filename) const
{
    // no retrieval needed, DIR is mounted at destination
    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::findFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	find file denoted by pattern
//	filename is interpreted relative to the attached url
//
//	pattern is a string with an optional trailing '*'
//
const Pathname *
MediaDIR::findFile (const Pathname & dirname, const string & pattern) const
{
    return scanDirectory (dirname, pattern);
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaDIR::getDirectory
//	METHOD TYPE : const Attribute &
//
//	DESCRIPTION :
//	get directory denoted by path to Attribute::A_StringArray

const Attribute *
MediaDIR::dirInfo (const Pathname & dirname) const
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
//	METHOD NAME : MediaDIR::getInfo
//	METHOD TYPE : const PathInfo *
//
 //	DESCRIPTION :
//	get file information

const PathInfo *
MediaDIR::fileInfo (const Pathname & filename) const
{
    // no retrieval needed, DIR is mounted at destination
    return new PathInfo (filename);
}

