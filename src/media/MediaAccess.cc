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

#include <ctype.h>

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/MediaAccess.h>

#include <y2pm/MediaCD.h>
#include <y2pm/MediaDIR.h>
#include <y2pm/MediaDISK.h>
#include <y2pm/MediaNFS.h>
#include <y2pm/MediaSMB.h>
#include <y2pm/MediaWget.h>

using namespace std;

IMPL_BASE_POINTER(MediaAccess);

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaAccess

// constructor

MediaAccess::MediaAccess ()
    : _type (Unknown)
    , _handler (0)
{
    DBG << endl;
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
PMError
MediaAccess::open (const Url& url)
{
    if(!url.isValid())
	return Error::E_bad_url;

    this->close();

    D__ << url.asString() << endl;

    string protocol = url.getProtocol();

    for(unsigned i=0;i<protocol.length();i++)
    {
	protocol[i]=tolower(protocol[i]);
    }

    if ( protocol == "cd" )
    {
	_type = CD;
	_handler = new MediaCD (url);
    }
    else if ( protocol == "dvd" )
    {
	_type = DVD;
	_handler = new MediaCD (url);
    }
    else if ( protocol == "nfs" )
    {
	_type = NFS;
	_handler = new MediaNFS (url);
    }
    else if ( protocol == "dir" )
    {
	_type = DIR;
	_handler = new MediaDIR (url);
    }
    else if ( protocol == "disk" )
    {
	_type = DISK;
	_handler = new MediaDISK (url);
    }
    else if ( protocol == "ftp" )
    {
	_type = FTP;
	_handler = new MediaWget (url);
    }
    else if ( protocol == "smb" )
    {
	_type = SMB;
	_handler = new MediaSMB (url);
    }
    else if ( protocol == "http" )
    {
	_type = HTTP;
	_handler = new MediaWget (url);
    }
    else if ( protocol == "https" )
    {
	_type = HTTPS;
	_handler = new MediaWget (url);
    }
    else
    {
	return Error::E_bad_media_type;
    }

    // check created handler

    if (_handler == 0)
    {
	return Error::E_system;
    }

    return Error::E_ok;
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
MediaAccess::getAttachPoint (void) const
{
    return _destination;
}


// attach media to directory
PMError
MediaAccess::attachTo (const Pathname & to)
{
    PathInfo info (to);

    if (!info.isDir())			// must be dir
    {
	D__ << to.asString() << " is no directory" << endl;
	return Error::E_not_a_directory;
    }

    _destination = to;

    if (_handler == 0)
    {
	return Error::E_not_open;
    }

    return _handler->attachTo (to);
}

// release attached media
PMError
MediaAccess::release (bool eject)
{
    if (_handler == 0)
    {
	return Error::E_not_open;
    }

    return _handler->release (eject);
}


// provide file denoted by path to attach dir
//
// filename is interpreted relative to the attached url
// and a path prefix is preserved to destination
PMError
MediaAccess::provideFile (const Pathname & filename) const
{
    D__ << filename.asString() << endl;
    if (_handler == 0)
    {
	return Error::E_not_open;
    }
    if (_destination.empty())
    {
	return Error::E_no_destination;
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
    if (pattern.empty())
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
    if (_destination.empty())
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
    if (_destination.empty())
    {
	return 0;
    }
    return _handler->fileInfo (filename);
}


// clean up a file from destination
// if filename == "", the whole destination is cleared
PMError
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
    return Error::E_ok;
}


std::ostream &
MediaAccess::dumpOn( std::ostream & str ) const
{
    str << "MediaAccess (";

    const char * tstr = "???"; // default for unknown types
    switch (_type)
    {
	case CD:    tstr = "CD";    break;
	case DVD:   tstr = "DVD";   break;
	case NFS:   tstr = "NFS";   break;
	case DIR:   tstr = "DIR";   break;
	case DISK:  tstr = "DISK";  break;
	case FTP:   tstr = "FTP";   break;
	case SMB:   tstr = "SMB";   break;
	case HTTP:  tstr = "HTTP";  break;
	case HTTPS: tstr = "HTTPS"; break;
	///////////////////////////////////////////////////////////////////
	// no default: let compiler warn '... not handled in switch'
        ///////////////////////////////////////////////////////////////////
	case Unknown:
	  break;
    }
    str << tstr << "@" << _destination.asString() << endl;
    return str;
}

///////////////////////////////////////////////////////////////////

