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
#include <y2pm/MediaCallbacks.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>

#include "config.h"

using namespace std;
using namespace MediaCallbacks;

Pathname MediaCurl::_cookieFile = "/var/lib/YaST2/cookies";

MediaCurl::Callbacks *MediaCurl::_callbacks = 0;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaCurl
//
///////////////////////////////////////////////////////////////////

MediaCurl::MediaCurl( const Url &      url_r,
		      const Pathname & attach_point_hint_r )
    : MediaHandler( url_r, attach_point_hint_r,
		    false, // attachPoint_is_mediaroot
		    true ), // does_download
      _curl( 0 ), _connected( false )
{
}

void MediaCurl::setCookieFile( const Pathname &fileName )
{
  _cookieFile = fileName;
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

  _connected = true;

  ret = curl_easy_setopt( _curl, CURLOPT_ERRORBUFFER, _curlError );
  if ( ret != 0 ) {
    return PMError( Error::E_curl_setopt_failed, "Error setting error buffer" );
  }

  ret = curl_easy_setopt( _curl, CURLOPT_FAILONERROR, true );
  if ( ret != 0 ) {
    return PMError( Error::E_curl_setopt_failed, _curlError );
  }

  if ( _url.protocol() == Url::ftp && _url.username().empty() ) {
    string id = "yast2@";
    id += VERSION;
    DBG << "Anonymous FTP identification: '" << id << "'" << endl;
    _userpwd += "anonymous:" + id;
    ret = curl_easy_setopt( _curl, CURLOPT_USERPWD, _userpwd.c_str() );
    if ( ret != 0 ) {
        return PMError( Error::E_curl_setopt_failed, _curlError );
    }
  }

  SysConfig cfg( "proxy" );

  if ( cfg.readBoolEntry( "PROXY_ENABLED", false ) ) {
    if ( _url.protocol() == Url::ftp ) {
      _proxy = cfg.readEntry( "FTP_PROXY" );
    } else if ( _url.protocol() == Url::http ) {
       _proxy = cfg.readEntry( "HTTP_PROXY" );
    } else {
      _proxy = "";
    }

    D__ << "Proxy: " << _proxy << endl;

    if ( !_proxy.empty() ) {
      ret = curl_easy_setopt( _curl, CURLOPT_PROXY, _proxy.c_str() );
      if ( ret != 0 ) {
        return PMError( Error::E_curl_setopt_failed, _curlError );
      }

      string curlrcFile = string( getenv("HOME") ) + string( "/.curlrc" );
      SysConfig curlrc( curlrcFile );
      _proxyuserpwd = curlrc.readEntry( "proxy-user" );

      ret = curl_easy_setopt( _curl, CURLOPT_PROXYUSERPWD,
                              _proxyuserpwd.c_str() );
      if ( ret != 0 ) {
          return PMError( Error::E_curl_setopt_failed, _curlError );
      }
    }
  }

  _currentCookieFile = _cookieFile.asString();

  ret = curl_easy_setopt( _curl, CURLOPT_COOKIEFILE,
                          _currentCookieFile.c_str() );
  if ( ret != 0 ) {
    return PMError( Error::E_curl_setopt_failed, _curlError );
  }

  ret = curl_easy_setopt( _curl, CURLOPT_COOKIEJAR,
                          _currentCookieFile.c_str() );
  if ( ret != 0 ) {
    return PMError( Error::E_curl_setopt_failed, _curlError );
  }

  ret = curl_easy_setopt( _curl, CURLOPT_PROGRESSFUNCTION,
                          &MediaCurl::progressCallback );
  if ( ret != 0 ) {
    return PMError( Error::E_curl_setopt_failed, _curlError );
  }

  ret = curl_easy_setopt( _curl, CURLOPT_NOPROGRESS, false );
  if ( ret != 0 ) {
    return PMError( Error::E_curl_setopt_failed, _curlError );
  }

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCurl::disconnectFrom
//	METHOD TYPE : PMError
//
PMError MediaCurl::disconnectFrom()
{
  if ( _connected ) curl_easy_cleanup( _curl );
  _connected = false ;

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
  disconnect();

  curl_global_cleanup();

  return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaCurl::getFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaCurl::getFile( const Pathname & filename ) const
{
    D__ << filename.asString() << endl;

    if(!_url.isValid())
	return PMError( Error::E_bad_url, _url.asString() );

    if(_url.host().empty())
	return Error::E_no_host_specified;

    string path = _url.path();
    if ( !path.empty() && path != "/" && *path.rbegin() == '/' &&
         filename.absolute() ) {
      // If url has a path with trailing slash, remove the leading slash from
      // the absolute file name
      path += filename.asString().substr( 1, filename.asString().size() - 1 );
    } else if ( filename.relative() ) {
      // Add trailing slash to path, if not already there
      if ( !path.empty() && *path.rbegin() != '/' ) path += "/";
      // Remove "./" from begin of relative file name
      path += filename.asString().substr( 2, filename.asString().size() - 2 );
    } else {
      path += filename.asString();
    }

    Url url( _url );
    url.setPath( path );

    // Use absolute file name to prevent access of files outside of the
    // hierarchy below the attach point.
    Pathname dest = attachPoint() + filename.absolutename();

    string destNew = dest.asString() + ".new.yast.37456";

    D__ << "dest: " << dest << endl;
    D__ << "destNew: " << destNew << endl;

    if( PathInfo::assert_dir( dest.dirname() ) )
    {
	DBG << "assert_dir " << dest.dirname() << " failed" << endl;
	return PMError( Error::E_system, dest.dirname().asString() );
    }

    DBG << "URL: " << url.asString().c_str() << endl;

    string urlBuffer = url.saveAsString();

    CURLcode ret = curl_easy_setopt( _curl, CURLOPT_URL,
                                     urlBuffer.c_str() );
    if ( ret != 0 ) {
      return PMError( Error::E_curl_setopt_failed, _curlError );
    }

    FILE *file = fopen( destNew.c_str(), "w" );
    if ( !file ) {
      ERR << "fopen failed" << endl;
      return PMError( Error::E_write_error, destNew );
    }

    ret = curl_easy_setopt( _curl, CURLOPT_WRITEDATA, file );
    if ( ret != 0 ) {
      fclose( file );
      return PMError( Error::E_curl_setopt_failed, _curlError );
    }

    // Set callback and perform.
    DownloadProgressReport::Send report( downloadProgressReport );
    report->start( url, dest );
    if ( curl_easy_setopt( _curl, CURLOPT_PROGRESSDATA, &report ) != 0 ) {
      WAR << "Can't set CURLOPT_PROGRESSDATA: " << _curlError << endl;;
    }

    ret = curl_easy_perform( _curl );
    fclose( file );

    if ( curl_easy_setopt( _curl, CURLOPT_PROGRESSDATA, NULL ) != 0 ) {
      WAR << "Can't unset CURLOPT_PROGRESSDATA: " << _curlError << endl;;
    }

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
              string msg = "HTTP return code: " +
                           stringutil::numstring( httpReturnCode ) +
                           " (URL: " + url.asString() + ")";
              DBG << msg << endl;
              if ( httpReturnCode == 401 )
                err = PMError( Error::E_login_failed, "URL: " + url.asString() );
              else
		err = PMError( Error::E_file_not_found, msg );
	      report->stop( err );
	      return err;
            }
          }
          break;
        case CURLE_FTP_COULDNT_RETR_FILE:
        case CURLE_FTP_ACCESS_DENIED:
          err = Error::E_file_not_found;
          break;
        case CURLE_BAD_PASSWORD_ENTERED:
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
        case CURLE_ABORTED_BY_CALLBACK:
          err = Error::E_user_abort;
          break;
        case CURLE_SSL_PEER_CERTIFICATE:
        default:
          err = Error::E_error;
          break;
      }

      err.setDetails( _curlError );
      report->stop( err );
      return err;
    }

    if ( PathInfo::rename( destNew, dest ) != 0 ) {
      ERR << "Rename failed" << endl;
      report->stop( Error::E_write_error );
      return Error::E_write_error;
    }

    report->stop( Error::E_ok );
    return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCurl::getDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached
//
PMError MediaCurl::getDir( const Pathname & dirname, bool recurse_r ) const
{
  PathInfo::dircontent content;
  PMError err = getDirInfo( content, dirname, /*dots*/false );

  if ( ! err ) {
    for ( PathInfo::dircontent::const_iterator it = content.begin(); it != content.end(); ++it ) {
      Pathname filename = dirname + it->name;
      int res = 0;

      switch ( it->type ) {
      case PathInfo::NOT_AVAIL: // old directory.yast contains no typeinfo at all
      case PathInfo::T_FILE:
	err = getFile( filename );
	break;
      case PathInfo::T_DIR: // newer directory.yast contain at least directory info
	if ( recurse_r ) {
	  err = getDir( filename, recurse_r );
	} else {
	  res = PathInfo::assert_dir( localPath( filename ) );
	  if ( res ) {
	    WAR << "Ignore error (" << res <<  ") on creating local directory '" << localPath( filename ) << "'" << endl;
	  }
	}
	break;
      default:
	// don't provide devices, sockets, etc.
	break;
      }

      if ( err ) {
	break;
      }
    }
  }

  return err;
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
  PMError err = getDirectoryYast( retlist, dirname, dots );
  if ( err ) {
    err = Error::E_not_supported_by_media;
  }
  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCurl::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaCurl::getDirInfo( PathInfo::dircontent & retlist,
			       const Pathname & dirname, bool dots ) const
{
  PMError err = getDirectoryYast( retlist, dirname, dots );
  if ( err ) {
    err = Error::E_not_supported_by_media;
  }
  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaCurl::progressCallback
//	METHOD TYPE : int
//
//	DESCRIPTION : Progress callback triggered from MediaCurl::getFile
//
int MediaCurl::progressCallback( void *clientp, double dltotal, double dlnow,
                                 double ultotal, double ulnow )
{
  DownloadProgressReport::Send * reportP = reinterpret_cast<DownloadProgressReport::Send*>( clientp );
  if ( reportP ) {
    ProgressData pd( 0, int(dltotal), int(dlnow) );
    (*reportP)->progress( pd );
  }

#warning YOU callbacks still active
  if ( _callbacks ) {
    if ( _callbacks->progress( int( dlnow * 100 / dltotal ) ) ) return 0;
    else return 1;
  }

  return 0;
}
