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
		  const Pathname & attach_point_hint_r,
		  MediaAccess::MediaType type_r )
    : MediaHandler( url_r, attach_point_hint_r,
		    true,  // attachPoint_is_mediaroot
		    false, // does_download
		    type_r )
{
    Url::OptionMapType options = _url.getOptions();
    Url::OptionMapType::iterator it;

    // chop up devices and put them in a list
    if((it=options.find("devices")) != options.end())
    {
	string devices=it->second;
	string::size_type pos;
	D__ << "parse " << devices << endl;
	while(!devices.empty())
	{
	    pos = devices.find(',');
	    string device = devices.substr(0,pos);
	    if(!device.empty())
	    {
		_devices.push_back(device);
		D__ << "use device " << device << endl;
	    }
	    if(pos!=string::npos)
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
PMError MediaCD::attachTo()
{
    Mount mount;
    const char *mountpoint = attachPoint().asString().c_str();
    bool mountsucceeded = false;
    PMError ret = Error::E_ok;

    string options = _url.getOption("mountoptions");
    if (options.empty())
    {
	options="ro";
    }

    //TODO: make configurable
    list<string> filesystems;

    // if DVD, try UDF filesystem before iso9660
    if (_url.getProtocol() == "dvd")
	filesystems.push_back("udf");

    filesystems.push_back("iso9660");

    // try all devices in sequence
    for (DeviceList::iterator it = _devices.begin()
	; !mountsucceeded && it != _devices.end()
	; ++it )
    {
	// close tray
	const char *const argv[] = { "/bin/eject", "-t", (*it).c_str(), NULL };
	ExternalProgram *process = new ExternalProgram (argv, ExternalProgram::Discard_Stderr);
	delete process;

	// try all filesystems in sequence
	for(list<string>::iterator fsit = filesystems.begin()
	    ; !mountsucceeded && fsit != filesystems.end()
	    ; ++fsit)
	{
#if 0 // mount.mount does the logging
	    MIL << "try mount " << *it
		<< " to " << mountpoint
		<< " filesystem " << *fsit << ": ";
#endif
	    ret = mount.mount (*it, mountpoint, *fsit, options);
	    if( ret == Error::E_ok )
	    {
		mountsucceeded = true;
		MIL << "succeded" << endl;
		_mounteddevice = *it;
	    }
	    else
	    {
		MIL << "failed: " << ret << endl;
	    }
	}
    }

    if(!mountsucceeded)
    {
	_mounteddevice.erase();
	return ret;
    }
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
    if(_mounteddevice.empty())
    {
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
    if(eject)
    {
	int fd;
	MIL << "eject " << _mounteddevice << endl;
	fd = ::open(_mounteddevice.c_str(), O_RDONLY|O_NONBLOCK);
	if(fd != -1)
	{
	    ::ioctl(fd,CDROMEJECT);
	    ::close(fd);
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
