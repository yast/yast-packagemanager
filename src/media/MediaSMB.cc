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

   File:	MediaSMB.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to CD or DVD media
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/Mount.h>
#include <y2pm/MediaSMB.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaSMB
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::MediaSMB
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaSMB::MediaSMB (const Url& url)
    : MediaHandler (url)
{
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::~MediaSMB
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaSMB::~MediaSMB()
{
    if (_attachPoint != "") {
	release ();
    }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
MediaSMB::dumpOn( ostream & str ) const
{
    return MediaHandler::dumpOn(str);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::attachTo
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : attach media to path
//
MediaResult
MediaSMB::attachTo (const Pathname & to)
{
    if(!_url.isValid())
	    return E_bad_url;

    if(_url.getHost().empty())
	    return E_no_host_specified;

    const char* const filesystem = "smbfs";
    const char *mountpoint = to.asString().c_str();
    Mount mount;
    MediaResult ret;

    string path = "//";
    path += _url.getHost();
    path += _url.getPath();

    string options = _url.getOption("mountoptions");
    string username = _url.getUsername();
    string password = _url.getPassword();
    // need to add guest to prevent smbmount from asking for password
    if(options.empty())
    {
	options="ro,guest";
    }
    else if ( password.empty()
	    && options.find("guest") == string::npos
	    && options.find("credentials") == string::npos
	    && options.find("password") == string::npos )
    {
	options += ",guest";
    }

    if(!username.empty())
	options += ",username=" + username;

    if(!password.empty())
	options += ",password=" + password;

    MIL << "try mount " << path
	<< " to " << mountpoint
	<< " filesystem " << filesystem << ": ";

    ret = mount.mount(path,mountpoint,filesystem,options);
    if(ret == E_none)
    {
	MIL << "succeded" << endl;
    }
    else
    {
	MIL << "failed: " <<  media_result_strings[ret] << endl;
	return ret;
    }

    _attachPoint = to;

    return E_none;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::release
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION : release attached media
//
MediaResult
MediaSMB::release (bool eject)
{
    if(_attachPoint.asString().empty())
    {
	return E_not_attached;
    }
    
    MIL << "umount " << _attachPoint.asString();

    Mount mount;
    MediaResult ret;

    if ((ret = mount.umount(_attachPoint.asString())) != E_none)
    {
	MIL << "failed: " <<  media_result_strings[ret] << endl;
	return ret;
    }
    
    MIL << "succeded" << endl;

    _attachPoint = "";
    return ret;

}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaSMB::provideFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	get file denoted by path to 'attached path'
//	filename is interpreted relative to the attached url
//	and a path prefix is preserved to destination

MediaResult
MediaSMB::provideFile (const Pathname & filename) const
{
    // no retrieval needed, NFS path is mounted at destination
    if(!_url.isValid())
	return E_bad_url;

    if(_attachPoint.asString().empty())
	return E_not_attached;

    Pathname src = _attachPoint;
    src += filename;

    PathInfo info(src);
    
    if(!info.isFile())
    {
	    D__ << src.asString() << " does not exist" << endl;
	    return E_file_not_found;
    }

    return E_none;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::findFile
//	METHOD TYPE : MediaResult
//
//	DESCRIPTION :
//	find file denoted by pattern
//	filename is interpreted relative to the attached url
//
//	pattern is a string with an optional trailing '*'
//

const Pathname *
MediaSMB::findFile (const Pathname & dirname, const string & pattern) const
{
    return scanDirectory (dirname, pattern);
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaSMB::getDirectory
//	METHOD TYPE : const std::list<std::string> *
//
//	DESCRIPTION :
//	get directory denoted by path to a string list

const std::list<std::string> *
MediaSMB::dirInfo (const Pathname & dirname) const
{
    return readDirectory (dirname);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::getInfo
//	METHOD TYPE : const PathInfo *
//
//	DESCRIPTION :
//	get file information

const PathInfo *
MediaSMB::fileInfo (const Pathname & filename) const
{
    // no retrieval needed, CD is mounted at destination
    return new PathInfo (filename);
}
