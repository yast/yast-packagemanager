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
	//default is /dev/cdrom; for dvd: /dev/dvd if it exists
	//TODO: make configurable
        string device( "/dev/cdrom" );
	if ( _url.protocol() == Url::dvd && PathInfo( "/dev/dvd" ).isBlk() ) {
	  device = "/dev/dvd";
	}
	D__ << "use default device " << device << endl;
	_devices.push_back(device);
    }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::openTray
//	METHOD TYPE : bool
//
bool MediaCD::openTray( const string & device_r )
{
  int fd = ::open( device_r.c_str(), O_RDONLY|O_NONBLOCK );
  if ( fd == -1 ) {
    WAR << "Unable to open '" << device_r << "' (" << ::strerror( errno ) << ")" << endl;
    return false;
  }
  int res = ::ioctl( fd, CDROMEJECT );
  ::close( fd );
  if ( res ) {
    WAR << "Eject " << device_r << " failed (" << ::strerror( errno ) << ")" << endl;
    return false;
  }
  MIL << "Eject " << device_r << endl;
  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::closeTray
//	METHOD TYPE : bool
//
bool MediaCD::closeTray( const string & device_r )
{
  int fd = ::open( device_r.c_str(), O_RDONLY|O_NONBLOCK );
  if ( fd == -1 ) {
    WAR << "Unable to open '" << device_r << "' (" << ::strerror( errno ) << ")" << endl;
    return false;
  }
  int res = ::ioctl( fd, CDROMCLOSETRAY );
  ::close( fd );
  if ( res ) {
    WAR << "Close tray " << device_r << " failed (" << ::strerror( errno ) << ")" << endl;
    return false;
  }
  DBG << "Close tray " << device_r << endl;
  return true;
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
    int count = 0;

    D__ << "next " << next << " last " << _lastdev << " lastdevice " << _mounteddevice << endl;

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
    PMError err = Error::E_mount_failed;
    for (DeviceList::iterator it = _devices.begin()
	; !mountsucceeded && it != _devices.end()
	; ++it, count++ )
    {
	D__ << "count " << count << endl;
	if (next && count<=_lastdev )
	{
		D__ << "skip" << endl;
		continue;
	}

	// close tray
	closeTray( *it );

	// try all filesystems in sequence
	for(list<string>::iterator fsit = filesystems.begin()
	    ; !mountsucceeded && fsit != filesystems.end()
	    ; ++fsit)
	{
	    err = mount.mount (*it, mountpoint, *fsit, options);
	    if ( ! err )
	    {
		mountsucceeded = true;
		_mounteddevice = *it;
		_lastdev = count;
	    }
	}
    }

    if (!mountsucceeded)
    {
	_mounteddevice.erase();
	_lastdev = -1;
	return err;
    }
    D__ << _lastdev << " " << count << endl;
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
	        openTray( *it );
	    }
	    return Error::E_ok;
	}
	return Error::E_not_attached;
    }

    Mount mount;
    PMError err;
    if ((err = mount.umount(attachPoint().asString())))
    {
	return err;
    }

    // eject device
    if (eject)
    {
        openTray( _mounteddevice );
    }

    _mounteddevice.erase();

    return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::forceEject
//	METHOD TYPE : void
//
// Asserted that media is not attached.
//
void MediaCD::forceEject()
{
  if ( _mounteddevice.empty() ) {	// no device mounted
    for ( DeviceList::iterator it = _devices.begin(); it != _devices.end(); ++it ) {
      if ( openTray( *it ) )
	break; // on 1st success
    }
  }
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
//	METHOD NAME : MediaCD::getDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaCD::getDir (const Pathname & dirname) const
{
  return MediaHandler::getDir( dirname );
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaCD::getDirInfo( PathInfo::dircontent & retlist,
			   const Pathname & dirname, bool dots ) const
{
  return MediaHandler::getDirInfo( retlist, dirname, dots );
}
