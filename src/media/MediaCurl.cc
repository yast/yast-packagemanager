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

   File:	MediaCurl.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access FTP, HTTP and HTTPS servers
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/ExternalProgram.h>
#include <y2util/SysConfig.h>

#include <y2pm/MediaCurl.h>
#include <y2pm/Wget.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaCurl
//
///////////////////////////////////////////////////////////////////

MediaCurl::MediaCurl( const Url &      url_r,
		      const Pathname & attach_point_hint_r,
		      MediaAccess::MediaType type_r )
    : MediaHandler( url_r, attach_point_hint_r,
		    false, // attachPoint_is_mediaroot
		    true,  // does_download
		    type_r ),
      _curl( 0 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCurl::attachTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that not already attached, and attachPoint is a directory.
//
PMError MediaCurl::attachTo (bool next)
{
  if ( next ) return Error::E_not_supported_by_media;

  if ( !_url.isValid() ) return Error::E_bad_url;

  CURLcode ret = curl_global_init( CURL_GLOBAL_ALL );
  if ( ret != 0 ) {
    ERR << "curl global init failed" << endl;
    return Error::E_error;
  }

  _curl = curl_easy_init();
  if ( !_curl ) {
    curl_global_cleanup();
    ERR << "curl easy init failed" << endl;
    return Error::E_error;
  }

  ret = curl_easy_setopt( _curl, CURLOPT_ERRORBUFFER, _curlError );
  if ( ret != 0 ) {
    return PMError( Error::E_curl_setopt_failed, "Error setting error buffer" );
  }

  ret = curl_easy_setopt( _curl, CURLOPT_FAILONERROR, true );
  if ( ret != 0 ) {
    return PMError( Error::E_curl_setopt_failed, _curlError );
  }

  SysConfig cfg( "proxy" );

  if ( _url.getProtocol() == "ftp" ) {
    _proxy = cfg.readEntry( "FTP_PROXY" );
  } else if ( _url.getProtocol() == "http" ){
     _proxy = cfg.readEntry( "HTTP_PROXY" );
  } else {
    _proxy = "";
  }

  if ( !_proxy.empty() ) {
    ret = curl_easy_setopt( _curl, CURLOPT_PROXY, _proxy.c_str() );
    if ( ret != 0 ) {
      return PMError( Error::E_curl_setopt_failed, _curlError );
    }
    
    string user = cfg.readEntry( "PROXY_USER" );
    string password = cfg.readEntry( "PROXY_PASSWORD" );
    if ( !user.empty() && !password.empty() ) {
      _proxyuserpwd = user + ":" + password;
      ret = curl_easy_setopt( _curl, CURLOPT_PROXYUSERPWD,
                              _proxyuserpwd.c_str() );
      if ( ret != 0 ) {
        return PMError( Error::E_curl_setopt_failed, _curlError );
      }
    }
  }

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCurl::releaseFrom
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaCurl::releaseFrom( bool eject )
{
  curl_easy_cleanup( _curl );
  curl_global_cleanup();

  return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaCD::getFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaCurl::getFile( const Pathname & filename ) const
{
    D__ << filename.asString() << endl;

    if(!_url.isValid())
	return PMError( Error::E_bad_url, _url.asString() );

    if(_url.getHost().empty())
	return Error::E_no_host_specified;

    Wget wget;
    string tmp;
    
    Pathname path = _url.getPath();
    path += filename;

    Url url(_url);
    url.setPath(path.asString());

    Pathname dest = attachPoint() + filename;

    string destNew = dest.asString() + ".new.yast.37456";
    
    D__ << "dest: " << dest << endl;
    D__ << "destNew: " << destNew << endl;
    
    if( PathInfo::assert_dir( dest.dirname() ) )
    {
	DBG << "assert_dir " << dest.dirname() << " failed" << endl;
	return Error::E_system;
    }

    DBG << "URL: " << url.asString().c_str() << endl;

    CURLcode ret = curl_easy_setopt( _curl, CURLOPT_URL,
                                     url.asString( true, true, true ).c_str() );
    if ( ret != 0 ) {
      return PMError( Error::E_curl_setopt_failed, _curlError );
    }

    FILE *file = fopen( destNew.c_str(), "w" );
    if ( !file ) {
      ERR << "fopen failed" << endl;
      return Error::E_error;
    }

    ret = curl_easy_setopt( _curl, CURLOPT_WRITEDATA, file );
    if ( ret != 0 ) {
      fclose( file );
      return PMError( Error::E_curl_setopt_failed, _curlError );
    }

    ret = curl_easy_perform( _curl );

    fclose( file );

    if ( ret != 0 ) {
      PathInfo::unlink( destNew );

      ERR << "curl error: " << ret << ": " << _curlError << endl;
      PMError err;
      switch ( ret ) {
        case CURLE_UNSUPPORTED_PROTOCOL:
        case CURLE_URL_MALFORMAT:
        case CURLE_URL_MALFORMAT_USER:
          err = Error::E_bad_url;
          break;
        case CURLE_HTTP_NOT_FOUND:
          {
            long httpReturnCode;
            CURLcode infoRet = curl_easy_getinfo( _curl, CURLINFO_HTTP_CODE,
                                                  &httpReturnCode );
            if ( infoRet == CURLE_OK ) {
              DBG << "HTTP return code: " << httpReturnCode << endl;
              if ( httpReturnCode == 401 ) return Error::E_login_failed;
            }
          }
        case CURLE_FTP_COULDNT_RETR_FILE:
          err = Error::E_file_not_found;
          break;
        case CURLE_BAD_PASSWORD_ENTERED:
        case CURLE_FTP_ACCESS_DENIED:
        case CURLE_FTP_USER_PASSWORD_INCORRECT:
          err = Error::E_login_failed;
          break;
        case CURLE_COULDNT_RESOLVE_PROXY:
        case CURLE_COULDNT_RESOLVE_HOST:
        case CURLE_COULDNT_CONNECT:
        case CURLE_FTP_CANT_GET_HOST:
          err = Error::E_connection_failed;
          break;
        case CURLE_WRITE_ERROR:
          err = Error::E_write_error;
          break;
        case CURLE_SSL_PEER_CERTIFICATE:
        default:
          err = Error::E_error;
          break;
      }
      
      err.setDetails( _curlError );
      return err;
    }

    if ( PathInfo::rename( destNew, dest ) != 0 ) {
      ERR << "Rename failed" << endl;
      return Error::E_system;
    }

    return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCurl::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaCurl::getDirInfo( std::list<std::string> & retlist,
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
MediaCurl::findFile (const Pathname & dirname, const string & pattern) const
{
    // FIXME: scan directory on server
    return 0;
}


/** get file information
 * */

const PathInfo *
MediaCurl::fileInfo (const Pathname & filename) const
{
    // FIXME retrieve file from server
    return 0;
}
#endif
