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

   File:	MediaDISK.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to CD or DVD media
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/Mount.h>
#include <y2pm/MediaDISK.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaDISK
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::MediaDISK
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaDISK::MediaDISK (const Url& url)
    : MediaHandler (url)
{
	_device = _url.getOption("device");
	_filesystem = _url.getOption("filesystem");
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::~MediaDISK
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaDISK::~MediaDISK()
{
    if (_attachPoint != "") {
	release ();
    }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
MediaDISK::dumpOn( ostream & str ) const
{
    return MediaHandler::dumpOn(str);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::attachTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : attach media to path
//
PMError
MediaDISK::attachTo (const Pathname & to)
{
    // FIXME
    // do mount --bind <partition>/<dir> to <to>
    //   mount /dev/<partition> /tmp_mount
    //   mount /tmp_mount/<dir> <to> --bind -o ro    
    // FIXME: try all filesystems

    if(!_url.isValid())
	    return Error::E_bad_url;

    if(_device.empty())
	    return Error::E_no_destination;

    if(_filesystem.empty())
	    return Error::E_invalid_filesystem;

    Mount mount;
    const char *mountpoint = to.asString().c_str();
    string options = _url.getOption("mountoptions");
    if(options.empty())
    {
	options="ro";
    }

    MIL << "try mount " << _device
	<< " to " << mountpoint
	<< " filesystem " << _filesystem << ": ";

    PMError ret = mount.mount(_device,mountpoint,_filesystem,options);
    if( ret == Error::E_ok )
    {
	MIL << "succeded" << endl;
    }
    else
    {
	MIL << "failed" << ret << endl;
	return ret;
    }

    _attachPoint = to;

    return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::release
//	METHOD TYPE : PMError
//
//	DESCRIPTION : release attached media
//
PMError
MediaDISK::release (bool eject)
{
    if(_attachPoint.asString().empty())
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

    _attachPoint = "";
    return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaDISK::provideFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//	provide file denoted by path to 'attached path'
//	filename is interpreted relative to the attached url
//	and a path prefix is preserved to destination

PMError
MediaDISK::provideFile (const Pathname & filename) const
{
    // no retrieval needed, disk is mounted at destination
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
//	METHOD NAME : MediaDISK::findFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//	find file denoted by pattern
//	filename is interpreted relative to the attached url
//
//	pattern is a string with an optional trailing '*'
//

const Pathname *
MediaDISK::findFile (const Pathname & dirname, const string & pattern) const
{
    return scanDirectory (dirname, pattern);
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaDISK::getDirectory
//	METHOD TYPE : const std::list<std::string> *
//
//	DESCRIPTION :
//	get directory denoted by path to Attribute::A_StringArray

const std::list<std::string> *
MediaDISK::dirInfo (const Pathname & dirname) const
{
    return readDirectory (dirname);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDISK::getInfo
//	METHOD TYPE : const PathInfo *
//
//	DESCRIPTION :
//	get file information

const PathInfo *
MediaDISK::fileInfo (const Pathname & filename) const
{
    // no retrieval needed, CD is mounted at destination
    return new PathInfo (filename);
}
