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

   File:	MediaHTTP.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to CD or DVD media
/-*/

#include <iostream>

#include <y2pm/MediaHTTP.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaHTTP
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHTTP::MediaHTTP
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaHTTP::MediaHTTP (const string & server, const string & path, const string & options)
    : MediaHandler (server, path)
{
    // parse options to _mountflags
    // options = "user=<username>,pass=<password>,proxy=<proxy>,port=<proxyport>,type=<proxytype>"
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHTTP::~MediaHTTP
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaHTTP::~MediaHTTP()
{
    if (_attachPoint != "") {
	release ();
    }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHTTP::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
MediaHTTP::dumpOn( ostream & str ) const
{
    str << "MediaHTTP (" << _device << "@" << _path << ")";
    return str;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHTTP::attachTo
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : attach media to path
//
MediaResult
MediaHTTP::attachTo (const Pathname & to)
{
    
    // FIXME
    // connect to HTTP server '_device', cd to '_path'
    //
    // copy files to '_attachPoint' later

    _attachPoint = to;

    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHTTP::release
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : release attached media
//
MediaResult
MediaHTTP::release (void)
{
    if (umount (_attachPoint.asString().c_str()) != 0) {
	return E_system;
    }
    _attachPoint = "";
    return E_none;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaHTTP::provideFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	get file denoted by path to 'attached path'
//	filename is interpreted relative to the attached url
//	and a path prefix is preserved to destination

MediaResult
MediaHTTP::provideFile (const Pathname & filename) const
{
    // FIXME: retrieve file from server, save below _attacedTo
    return E_none;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHTTP::findFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	find file denoted by pattern
//	filename is interpreted relative to the attached url
//
//	pattern is a string with an optional trailing '*'
//

const Pathname *
MediaHTTP::findFile (const Pathname & dirname, const string & pattern) const
{
    // FIXME: scan directory on server
    return 0;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaHTTP::getDirectory
//	METHOD TYPE : const std::list<std::string> *
//
//	DESCRIPTION :
//	get directory denoted by path to a string list

const std::list<std::string> *
MediaHTTP::dirInfo (const Pathname & dirname) const
{
    // FIXME: pull directory from server
    return 0;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHTTP::getInfo
//	METHOD TYPE : const PathInfo *
//
//	DESCRIPTION :
//	get file information

const PathInfo *
MediaHTTP::fileInfo (const Pathname & filename) const
{
    // FIXME: pull file from server
    return 0;
}
