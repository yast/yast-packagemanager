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

   File:	MediaNFS.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to CD or DVD media
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/MediaNFS.h>
#include <y2pm/Mount.h>

#include <dirent.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaNFS
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::MediaNFS
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaNFS::MediaNFS (const Url& url)
    : MediaHandler (url)
{
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::~MediaNFS
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaNFS::~MediaNFS()
{
    if (_attachPoint != "") {
	release ();
    }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
MediaNFS::dumpOn( ostream & str ) const
{
    return MediaHandler::dumpOn(str);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::attachTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : attach media to path
//
PMError
MediaNFS::attachTo (const Pathname & to)
{
    if(!_url.isValid())
	    return Error::E_bad_url;

    if(_url.getHost().empty())
	    return Error::E_no_host_specified;

    const char* const filesystem = "nfs";
    const char *mountpoint = to.asString().c_str();
    Mount mount;

    string path = _url.getHost();
    path += ':';
    path += _url.getPath();

    string options = _url.getOption("mountoptions");
    if(options.empty())
    {
	options="ro";
    }

    MIL << "try mount " << path
	<< " to " << mountpoint
	<< " filesystem " << filesystem << ": ";

    PMError ret = mount.mount(path,mountpoint,filesystem,options);
    if(ret == Error::E_ok)
    {
	MIL << "succeded" << endl;
    }
    else
    {
	MIL << "failed: " <<  ret << endl;
	return ret;
    }

    _attachPoint = to;

    return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::release
//	METHOD TYPE : PMError
//
//	DESCRIPTION : release attached media
//
PMError
MediaNFS::release (bool eject)
{
    if(_attachPoint.asString().empty())
    {
	return Error::E_not_attached;
    }
    
    MIL << "umount " << _attachPoint.asString();

    Mount mount;
    PMError ret;

    if ((ret = mount.umount(_attachPoint.asString())) != Error::E_ok)
    {
	MIL << "failed: " <<  ret << endl;
	return ret;
    }
    
    MIL << "succeded" << endl;

    _attachPoint = "";
    return ret;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaNFS::provideFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//	get file denoted by path to 'attached path'
//	filename is interpreted relative to the attached url
//	and a path prefix is preserved to destination

PMError
MediaNFS::provideFile (const Pathname & filename) const
{
    // no retrieval needed, NFS path is mounted at destination
    if(!_url.isValid())
	return Error::E_bad_url;

    if(_attachPoint.asString().empty())
	return Error::E_not_attached;

    Pathname src = _attachPoint;
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
//	METHOD NAME : MediaNFS::findFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//	find file denoted by pattern
//	filename is interpreted relative to the attached url
//
//	pattern is a string with an optional trailing '*'
//

const Pathname *
MediaNFS::findFile (const Pathname & dirname, const string & pattern) const
{
    return scanDirectory (dirname, pattern);
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaNFS::getDirectory
//	METHOD TYPE : const std::list<std::string> *
//
//	DESCRIPTION :
//	get directory denoted by path to a string list

const std::list<std::string> *
MediaNFS::dirInfo (const Pathname & dirname) const
{
    return readDirectory (dirname);
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaNFS::getInfo
//	METHOD TYPE : const PathInfo *
//
//	DESCRIPTION :
//	get file information

const PathInfo *
MediaNFS::fileInfo (const Pathname & filename) const
{
    // no retrieval needed, CD is mounted at destination
    return new PathInfo (filename);
}
