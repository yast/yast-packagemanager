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

   File:	MediaAccess.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Handle access to a medium

/-*/

#include <string>
#include <iostream>

#include <y2pm/MediaAccess.h>

#include <y2pm/MediaCD.h>
#include <y2pm/MediaDIR.h>
#include <y2pm/MediaDISK.h>
#include <y2pm/MediaNFS.h>
#include <y2pm/MediaSMB.h>
#include <y2pm/MediaFTP.h>
#include <y2pm/MediaHTTP.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaAccess

// constructor

MediaAccess::MediaAccess ()
    : _type (Unknown)
    , _handler (0)
{
}

// destructor
MediaAccess::~MediaAccess()
{
    if (_handler)
    {
	this->close ();
    }
}


// open URL
MediaResult
MediaAccess::open (const string & url)
{
    char *sptr = strdup (url.c_str());
    char *device, *path, *options = 0;
    char *colon;

    // parse url

    colon = strchr (sptr, ':');		// first colon
    if (colon == 0)
	return E_bad_url;
    *colon = 0;

    // set device
    device = colon + 1;

    colon = strchr (device, ':');
    if (colon == 0)
    {
	return E_bad_url;
    }
    *colon = 0;

    // set path
    path = colon + 1;

    options = strchr (path, ':');
    if (colon != 0)
    {
	options = colon + 1;
    }

    if (strcasecmp (sptr, "cd") == 0)
    {
	_type = CD;
	_handler = new MediaCD (device, path, options);
    }
    else if (strcasecmp (sptr, "dvd") == 0)
    {
	_type = DVD;
	_handler = new MediaCD (device, path, options, true);
    }
    else if (strcasecmp (sptr, "nfs") == 0)
    {
	_type = NFS;
	_handler = new MediaNFS (device, path, options);
    }
    else if (strcasecmp (sptr, "dir") == 0)
    {
	_type = DIR;
	_handler = new MediaDIR (device, path, options);
    }
    else if (strcasecmp (sptr, "disc") == 0)
    {
	_type = DISK;
	_handler = new MediaDISK (device, path, options);
    }
    else if (strcasecmp (sptr, "ftp") == 0)
    {
	_type = FTP;
	_handler = new MediaFTP (device, path, options);
    }
    else if (strcasecmp (sptr, "smb") == 0)
    {
	_type = SMB;
	_handler = new MediaSMB (device, path, options);
    }
    else if (strcasecmp (sptr, "http") == 0)
    {
	_type = HTTP;
	_handler = new MediaHTTP (device, path, options);
    }
    else
    {
	return E_bad_media_type;
    }

    // check created handler

    if (_handler == 0)
    {
	return E_system;
    }

    return E_none;
}


// close handler
void
MediaAccess::close (void)
{
    if (_handler)
    {
	_handler->release ();
	delete _handler;
	_handler = 0;
    }
    return;
}


// get Handler
MediaHandler *
MediaAccess::handler (void) const
{
    return _handler;
}


// get destination for file retrieval
const Pathname &
MediaAccess::getAttachPath (void) const
{
    return _destination;
}


// attach media to directory
MediaResult
MediaAccess::attachTo (const Pathname & to)
{
    PathInfo info (to);

    if (!info.isDir())			// must be dir
    {
	return E_not_a_directory;
    }

    _destination = to;

    if (_handler == 0)
    {
	return E_not_open;
    }

    return _handler->attachTo (to);
}

// release attached media
MediaResult
MediaAccess::release (void)
{
    if (_handler == 0)
    {
	return E_not_open;
    }

    return _handler->release ();
}


// provide file denoted by path to attach dir
//
// filename is interpreted relative to the attached url
// and a path prefix is preserved to destination
MediaResult
MediaAccess::provideFile (const Pathname & filename) const
{
    if (_handler == 0)
    {
	return E_not_open;
    }
    if (_destination == "")
    {
	return E_no_destination;
    }
    return _handler->provideFile (filename);
}


// find file denoted by pattern
//
// filename is interpreted relative to the attached url
// pattern might have a single trailing '*'
//
const Pathname *
MediaAccess::findFile (const Pathname & dirname, const string & pattern) const
{
    if (_handler == 0)
    {
	return 0;
    }
    if (pattern == "")
    {
	return 0;
    }
    return _handler->findFile (dirname, pattern);
}


// get file information
const std::list<std::string> *
MediaAccess::dirInfo (const Pathname & filename) const
{
    if (_handler == 0)
    {
	return 0;
    }
    if (_destination == "")
    {
	return 0;
    }
    return _handler->dirInfo (filename);
}

// get file information
const PathInfo *
MediaAccess::fileInfo (const Pathname & filename) const
{
    if (_handler == 0)
    {
	return 0;
    }
    if (_destination == "")
    {
	return 0;
    }
    return _handler->fileInfo (filename);
}


// clean up a file from destination
// if filename == "", the whole destination is cleared
MediaResult
MediaAccess::cleanUp (const Pathname & filename) const
{
    Pathname fullname = _destination + filename;
    PathInfo info (fullname);

    if (info.isDir())
    {
	// rm -rf
    }
    else
    {
	unlink (fullname.asString().c_str());
    }
    return E_none;
}


std::ostream &
MediaAccess::dumpOn( std::ostream & str ) const
{
    str << "MediaAccess (";
    switch (_type)
    {
	case CD:   str << "CD"; break;
	case DVD:  str << "DVD"; break;
	case NFS:  str << "NFS"; break;
	case DIR:  str << "DIR"; break;
	case DISK: str << "DISK"; break;
	case FTP:  str << "FTP"; break;
	case SMB:  str << "SMB"; break;
	case HTTP: str << "HTTP"; break;
	default:   str << "???"; break;
    }
    str << "@" << _destination.asString() << endl;
    return str;
}

///////////////////////////////////////////////////////////////////

