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
MediaCD::MediaCD (const Url& url)
    : MediaHandler (url)
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
    return MediaHandler::dumpOn(str);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::attachTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : attach media at path
//
PMError
MediaCD::attachTo (const Pathname & to)
{
    // FIXME, issue "eject -t" to close the tray
    // really? mine does close automatically -- lnussel

    if(!_attachPoint.empty())
	return Error::E_already_attached;

    _attachPoint = to;

    Mount mount;
    const char *mountpoint = _attachPoint.asString().c_str();
    bool mountsucceeded = false;
    PMError ret = Error::E_ok;

    string options = _url.getOption("mountoptions");
    if(options.empty())
    {
	options="ro";
    }

    //TODO: make configurable
    list<string> filesystems;

    // if DVD, try UDF filesystem before iso9660
    if(_url.getProtocol() == "dvd")
	filesystems.push_back("udf");

    filesystems.push_back("iso9660");

    // try all devices in sequence
    for(DeviceList::iterator it = _devices.begin()
	; !mountsucceeded && it != _devices.end()
	; ++it )
    {
	// try all filesystems in sequence
	for(list<string>::iterator fsit = filesystems.begin()
	    ; !mountsucceeded && fsit != filesystems.end()
	    ; ++fsit)
	{
	    MIL << "try mount " << *it
		<< " to " << mountpoint
		<< " filesystem " << *fsit << ": ";
	    ret = mount.mount(*it,mountpoint,*fsit,options);
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
	_attachPoint = "";
	_mounteddevice.erase();
	return ret;
    }
    return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::release
//	METHOD TYPE : PMError
//
//	DESCRIPTION : release attached media
//
PMError
MediaCD::release (bool eject)
{
    if(_mounteddevice.empty())
    {
	return Error::E_not_attached;
    }

    MIL << "umount " << _attachPoint.asString() << endl;

    Mount mount;
    PMError ret;
    if ((ret = mount.umount(_attachPoint.asString())) != Error::E_ok)
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

    _attachPoint = "";
    _mounteddevice.erase();

    return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaCD::provideFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//	get file denoted by path to 'attached path'
//	filename is interpreted relative to the attached url
//	and a path prefix is preserved to destination

PMError
MediaCD::provideFile (const Pathname & filename) const
{
    // no retrieval needed, CD is mounted at destination

    if(!_url.isValid())
	return Error::E_bad_url;

    if(_attachPoint.asString().empty())
	return Error::E_not_attached;

    Pathname src = _attachPoint;
    src += _url.getPath();
    src += filename;

    PathInfo info(src);

    if(!info.isFile())
    {
	    D__ << src.asString() << " does not exist" << endl;
	    return Error::E_file_not_found;
    }
    return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCD::findFile
//	METHOD TYPE : PMError
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
