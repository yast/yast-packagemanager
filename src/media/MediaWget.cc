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

   File:	MediaWget.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access FTP, HTTP and HTTPS servers
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/ExternalProgram.h>
#include <y2pm/MediaWget.h>
#include <y2pm/Wget.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaWget
//
///////////////////////////////////////////////////////////////////

MediaWget::MediaWget (const Url& url)
    : MediaHandler (url)
{
}


MediaWget::~MediaWget()
{
    if (!_attachPoint.empty()) {
	release ();
    }
}


ostream &
MediaWget::dumpOn( ostream & str ) const
{
    return MediaHandler::dumpOn(str);
}


/** attach media to path
 * */

MediaResult
MediaWget::attachTo (const Pathname & to)
{
    
    // FIXME
    // connect to FTP server, cd to path
    //
    // copy files to '_attachPoint' later

    if(!_url.isValid())
	return E_bad_url;

    _attachPoint = to;

    D__ << _attachPoint.asString() << endl;

    return E_none;
}


/** release attached media
 * */

MediaResult
MediaWget::release (bool eject)
{
    _attachPoint = string();
    return E_none;
}


/** get file denoted by path to 'attached path' filename is interpreted
 * relative to the attached url and a path prefix is preserved to destination
 * */ 

MediaResult MediaWget::provideFile (const Pathname & filename) const {

    D__ << filename.asString() << endl;

    if(!_url.isValid())
	return E_bad_url;

    if(_url.getHost().empty())
	return E_bad_url;

    Wget wget;
    string tmp;

    wget.setProxyUser(_url.getOption("proxyuser"), _url.getOption("proxypassword"));

    Pathname path = "/";
    path += _url.getPath();
    path += filename;

    // let url assemble the username&password part, then append the path
    // manually
    string url=_url.asString(false);
    url += path.asString();

    D__ << url << endl;

    // TODO: recreate fs structure
    Pathname dest = _attachPoint+filename;
    if(PathInfo::mkdir(dest.dirname()))
    {
	DBG << "mkdir " << dest.asString() << " failed" << endl;
	return E_system;
    }
    
    WgetStatus status = wget.getFile( url, dest.asString() );
    if(status == WGET_OK)
	return E_none;
    else
    {
	DBG << "wget error: " << wget.error_string(status) << endl;
	//TODO wget errors
	return E_system;
    }
}

/** find file denoted by pattern
 *
 * @param	filename is interpreted relative to the attached url
 * @pattern	pattern is a string with an optional trailing '*'
 * */

const Pathname *
MediaWget::findFile (const Pathname & dirname, const string & pattern) const
{
    // FIXME: scan directory on server
    return 0;
}


/** get directory denoted by path to a string list
 * */

const std::list<std::string> *
MediaWget::dirInfo (const Pathname & dirname) const
{
    // FIXME: pull directory from server
    return 0;
}


/** get file information
 * */

const PathInfo *
MediaWget::fileInfo (const Pathname & filename) const
{
    // FIXME retrieve file from server
    return 0;
}
