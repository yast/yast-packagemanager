/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:       PMYouServers.cc

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Provide YOU server information.

/-*/

#include <iostream>
#include <fstream>
#include <sstream>

#include <y2util/Y2SLog.h>
#include <y2util/SysConfig.h>

#include <Y2PM.h>
#include <y2pm/InstSrcError.h>
#include <y2pm/PMPackage.h>
#include <y2pm/YouError.h>
#include <y2pm/InstTarget.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectable.h>
#include <y2pm/PMYouPatchPaths.h>

#include <y2pm/PMYouServers.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouServers
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMYouServers);

PMYouServers::PMYouServers( const PMYouPatchPathsPtr &patchPaths )
{
  _patchPaths = patchPaths;
}

PMYouServers::~PMYouServers()
{
}

Pathname PMYouServers::localSuseServers()
{
  return "/etc/suseservers";
}

Pathname PMYouServers::localYouServers()
{
  return "/etc/youservers";
}

Pathname PMYouServers::cachedYouServers()
{
  return _patchPaths->localWriteDir() + "youservers";
}

PMError PMYouServers::requestServers( bool check )
{
  string lastServer = _patchPaths->config()->readEntry( "LastServer" );
  if ( !lastServer.empty() ) {
    addServer( parseServerLine( lastServer ) );
  }

  PMError error = readServers( localYouServers() );
  if ( error ) return error;

  SysConfig cfg( "onlineupdate" );
  
  if ( cfg.readBoolEntry( "YAST2_LOADFTPSERVER", true ) ) {
    string url = _patchPaths->youUrl();
    url += "?product=" + _patchPaths->product();
    url += "&version=" + _patchPaths->version();
    url += "&basearch=" + string( _patchPaths->baseArch() );
    url += "&arch=" + string( _patchPaths->arch() );
    
    url += "&business=";
    if ( _patchPaths->businessProduct() ) url += "1";
    else url += "0";
    
    url += "&check=";
    if ( check ) url += "1";
    else url += "0";
    
    url += "&distproduct=" + _patchPaths->distProduct();

    addPackageVersion( "yast2-online-update", url );
    addPackageVersion( "yast2-packagemanager", url );
    addPackageVersion( "liby2util", url );

    url = encodeUrl( url );

    DBG << "url: '" << url << "'" << endl;

    Pathname writeDir = _patchPaths->localWriteDir();
    int ret = PathInfo::assert_dir( writeDir );
    if ( ret != 0 ) {
      ERR << "Unable to create " << writeDir << ": errno " << ret
          << endl;
    }
    
    PMError error = MediaAccess::getFile( Url( url ), cachedYouServers() );
    if ( error ) {
      if ( error == MediaError::E_write_error ) {
        return PMError( YouError::E_write_youservers_failed );
      } else {
        return PMError( YouError::E_get_youservers_failed, error.details() );
      }
    }

    // Remove obsolete file.
    PathInfo pi( localSuseServers() );
    if ( pi.isExist() ) {
      PathInfo::unlink( localSuseServers() );
    }
  } else {
    error = readServers( localSuseServers() );
    if ( error ) return error;
  }

  error = readServers( cachedYouServers() );

  return error;
}

PMError PMYouServers::readServers( const Pathname &file )
{
  DBG << "Reading servers from " << file << endl;

  PathInfo pi( file );
  if ( !pi.isExist() ) {
    DBG << "File doesn't exist." << endl;
    return PMError();
  }

  string line;
  ifstream in( file.asString().c_str() );
  if ( in.fail() ) {
    ERR << "Error reading " << file << endl;
    return PMError( YouError::E_read_youservers_failed, file.asString() );
  }
  while( getline( in, line ) ) {
    if ( !line.empty() && *line.begin() != '#' ) {
      PMYouServer server = parseServerLine( line );
      Url url( server.url );
      if ( url.isValid() ) addServer( server );
    }
  }

  return PMError();
}

void PMYouServers::addServer( const PMYouServer &server )
{
  D__ << "Add server: " << server.name << " (" << server.url << ")" << endl;

  list<PMYouServer>::const_iterator it;
  for( it = _servers.begin(); it != _servers.end(); ++it ) {
    if ( it->url == server.url && it->name == server.name &&
         it->directory == server.directory ) break;
  }
  if ( it == _servers.end() ) _servers.push_back( server );
}

void PMYouServers::addPackageVersion( const string &pkgName,
                                         string &url )
{
  PMSelectablePtr selectable = Y2PM::packageManager().getItem( pkgName );
  if ( !selectable ) {
    WAR << pkgName << " is not installed." << endl;
  } else {
    PMPackagePtr pkg = selectable->installedObj();
    if ( !pkg ) {
      WAR << "No installed object for " << pkgName << endl;
    } else {
      url += "&" + pkgName + "=" + pkg->edition().asString();
    }
  }
}

list<PMYouServer> PMYouServers::servers()
{
  if ( _servers.size() == 0 ) {
    list<PMYouServer> servers;
    servers.push_back( defaultServer() );
    return servers;
  } else {
    return _servers;
  }
}

PMYouServer PMYouServers::defaultServer()
{
  if ( _servers.size() == 0 ) {
    PMYouServer server;
    if ( _patchPaths->businessProduct() ) {
      server.url = "http://sdb.suse.de/download/";
    } else {
      server.url = "ftp://ftp.suse.com/pub/suse/";
    }
    return server;
  } else {
    return *_servers.begin();
  }
}

PMYouServer PMYouServers::currentServer()
{
  return *(servers().begin());
}

string PMYouServers::encodeUrl( const string &url )
{
  D__ << url << endl;

  string result;
  
  string::const_iterator it;
  for( it = url.begin(); it != url.end(); ++it ) {
    switch ( *it ) {
      case ' ':
        result += "%20";
        break;
      default:
        result += *it;
    }
  }

  D__ << result << endl;

  return result;
}

PMYouServer PMYouServers::parseServerLine( const string &line )
{
  PMYouServer server;
  server.url = line;
  return server;
}
