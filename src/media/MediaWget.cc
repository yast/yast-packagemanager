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

MediaWget::MediaWget( const Url &      url_r,
		      const Pathname & attach_point_hint_r,
		      MediaAccess::MediaType type_r )
    : MediaHandler( url_r, attach_point_hint_r,
		    false, // attachPoint_is_mediaroot
		    true,  // does_download
		    type_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaWget::attachTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that not already attached, and attachPoint is a directory.
//
PMError MediaWget::attachTo (bool next)
{
    if(next)
	return Error::E_not_supported_by_media;
  // FIXME
  // connect to FTP server, cd to path
  //
  // copy files to '_attachPoint' later

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaWget::releaseFrom
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaWget::releaseFrom( bool eject )
{
  return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaCD::getFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaWget::getFile( const Pathname & filename ) const {

    D__ << filename.asString() << endl;

    if(!_url.isValid())
	return Error::E_bad_url;

    if(_url.getHost().empty())
	return Error::E_no_host_specified;

    Wget wget;
    string tmp;
    
    Pathname path = _url.getPath();
    path += filename;

    Url url(_url);
    url.setPath(path.asString());

    // TODO: recreate fs structure
    Pathname dest = attachPoint()+filename;
    if(PathInfo::assert_dir(dest.dirname()))
    {
	DBG << "assert_dir " << dest.asString() << " failed" << endl;
	return Error::E_system;
    }

    WgetStatus status = wget.getFile( url, dest );
    if(status == WGET_OK)
	return Error::E_ok;
    else
    {
	DBG << "wget error: " << wget.error_string(status) << endl;
        switch ( status ) {
            case WGET_ERROR_FILE:
                return Error::E_file_not_found;
            case WGET_ERROR_LOGIN:
                return Error::E_login_failed;
            case WGET_ERROR_CONNECT:
                return Error::E_connection_failed;
            case WGET_ERROR_PROXY_AUTH:
                return Error::E_proxyauth_failed;
            case WGET_ERROR_UNEXPECTED:
  	        return Error::E_system;            
            case WGET_ERROR_SERVER:
            case WGET_ERROR_INVALID_URL:
            default:
                return Error::E_error;
        }
    }
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaWget::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaWget::getDirInfo( std::list<std::string> & retlist,
			       const Pathname & dirname, bool dots ) const
{
  return Error::E_not_supported_by_media;
}

#if 0
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


/** get file information
 * */

const PathInfo *
MediaWget::fileInfo (const Pathname & filename) const
{
    // FIXME retrieve file from server
    return 0;
}
#endif
