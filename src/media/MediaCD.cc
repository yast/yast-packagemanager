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
MediaCD::MediaCD (const Url& url)
    : MediaHandler (url)
    , _mountflags (MS_RDONLY)
{
    Url::OptionMapType options = _url.getOptions();
    Url::OptionMapType::iterator it;

    // chop up devices and put them in a list
    if((it=options.find("devices")) != options.end())
    {
	string devices=it->second;
	string::size_type pos;
	while(!devices.empty())
	{
	    pos = devices.find(',');
	    if(pos != string::npos)
	    {
		string device = devices.substr(0,pos);
		if(!device.empty())
		{
		    _devices.push_back(device);
		}
		devices=devices.substr(pos+1);
	    }
	}
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
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : attach media at path
//
MediaResult
MediaCD::attachTo (const Pathname & to)
{
    // FIXME, issue "eject -t" to close the tray
    
    _attachPoint = to;

    const char *mountpoint = _attachPoint.asString().c_str();
    bool mountsucceeded = false;

    // try all devices in turn
    for(DeviceList::iterator it = _devices.begin()
	; !mountsucceeded && it != _devices.end()
	; ++it )
    {
	// if DVD, try UDF filesystem before iso9660
	if (_url.getProtocol() != "dvd"
	    || (mount (it->c_str(), mountpoint, "udf", _mountflags, 0) != 0)) {

	    if (mount (it->c_str(), mountpoint, "iso9660", _mountflags, 0) != 0) {
		mountsucceeded = true;
	    }
	}
    }

    return (mountsucceeded?E_none:E_system);
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
