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
		This class handles access to CD media
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/ExternalProgram.h>
#include <y2pm/Mount.h>
#include <y2pm/MediaCD.h>

#include <cstring> // strerror

#include <errno.h>
#include <dirent.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/cdrom.h>


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
MediaCD::MediaCD( const Url &      url_r,
		  const Pathname & attach_point_hint_r )
    : MediaHandler( url_r, attach_point_hint_r,
		    true,  // attachPoint_is_mediaroot
		    false ), // does_download
    _lastdev(-1)
{
    Url::OptionMapType options = _url.options();
    Url::OptionMapType::iterator it;

    // chop up devices and put them in a list
    if ((it=options.find("devices")) != options.end())
    {
	string devices=it->second;
	string::size_type pos;
	D__ << "parse " << devices << endl;
	while(!devices.empty())
	{
	    pos = devices.find(',');
	    string device = devices.substr(0,pos);
	    if (!device.empty())
	    {
		_devices.push_back(device);
		D__ << "use device " << device << endl;
	    }
	    if (pos!=string::npos)
		devices=devices.substr(pos+1);
	    else
		devices.erase();
	}
    }
    else
    {
	//default is /dev/cdrom
	//TODO: make configurable
	const char* const device = "/dev/cdrom";
	D__ << "use default device " << device << endl;
	_devices.push_back(device);
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::attachTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that not already attached, and attachPoint is a directory.
//
PMError MediaCD::attachTo(bool next)
{
    Mount mount;
    const char *mountpoint = attachPoint().asString().c_str();
    bool mountsucceeded = false;
    PMError ret = Error::E_mount_failed;
    int count = 0;

    DBG << "next " << next << " last " << _lastdev << " lastdevice " << _mounteddevice << endl;

    if (next && _lastdev == -1) return Error::E_not_supported_by_media;

    string options = _url.option("mountoptions");
    if (options.empty())
    {
	options="ro";
    }

    //TODO: make configurable
    list<string> filesystems;

    // if DVD, try UDF filesystem before iso9660
    if ( _url.protocol() == Url::dvd )
	filesystems.push_back("udf");

    filesystems.push_back("iso9660");

    // try all devices in sequence
    for (DeviceList::iterator it = _devices.begin()
	; !mountsucceeded && it != _devices.end()
	; ++it, count++ )
    {
	DBG << "count " << count << endl;
	if (next && count<=_lastdev )
	{
		DBG << "skip" << endl;
		continue;
	}

	DBG << "try mount " << *it << endl;

	// close tray
	const char *const argv[] = { "/bin/eject", "-t", (*it).c_str(), NULL };
	ExternalProgram eject(argv, ExternalProgram::Discard_Stderr);
	eject.close();

	// try all filesystems in sequence
	for(list<string>::iterator fsit = filesystems.begin()
	    ; !mountsucceeded && fsit != filesystems.end()
	    ; ++fsit)
	{
	    ret = mount.mount (*it, mountpoint, *fsit, options);
	    if ( ret == Error::E_ok )
	    {
		mountsucceeded = true;
		MIL << " succeded" << endl;
		_mounteddevice = *it;
		_lastdev = count;
	    }
	    else
	    {
		MIL << "failed: " << ret << endl;
	    }
	}
    }

    if (!mountsucceeded)
    {
	_mounteddevice.erase();
	_lastdev = -1;
	return ret;
    }
    DBG << _lastdev << " " << count << endl;
    return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::releaseFrom
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaCD::releaseFrom( bool eject )
{
    if (_mounteddevice.empty())		// no device mounted
    {
	if (eject)			// eject wanted -> eject all devices
	{
	    for (DeviceList::iterator it = _devices.begin()
		; it != _devices.end()
		; ++it )
	    {
		int fd;
		MIL << "eject " << (*it) << endl;
		fd = ::open ((*it).c_str(), O_RDONLY|O_NONBLOCK);
		if (fd != -1)
		{
		    ::ioctl (fd, CDROMEJECT);
		    ::close (fd);
		}
	    }
	    return Error::E_ok;
	}
	return Error::E_not_attached;
    }

    MIL << "umount " << attachPoint() << endl;

    Mount mount;
    PMError ret;
    if ((ret = mount.umount(attachPoint().asString())) != Error::E_ok)
    {
	MIL << "failed: " <<  ret << endl;
	return ret;
    }

    // eject device
    if (eject)
    {
	int fd;
	MIL << "eject " << _mounteddevice << endl;
	fd = ::open (_mounteddevice.c_str(), O_RDONLY|O_NONBLOCK);
	if (fd != -1)
	{
	    ::ioctl (fd,CDROMEJECT);
	    ::close (fd);
	}
    }

    _mounteddevice.erase();

    return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaCD::getFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaCD::getFile( const Pathname & filename ) const
{
  return MediaHandler::getFile( filename );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaCD::getDirInfo( std::list<std::string> & retlist,
			     const Pathname & dirname, bool dots ) const
{
  return MediaHandler::getDirInfo( retlist, dirname, dots );
}

#if 0
const Pathname *
MediaCD::findFile (const Pathname & dirname, const string & pattern) const
{
    return scanDirectory (dirname, pattern);
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
#endif
