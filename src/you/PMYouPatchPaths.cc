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

  File:       PMYouPatchPaths.cc

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Provide path information for YOU patches.

/-*/

#include <iostream>
#include <fstream>
#include <sstream>

#include <y2util/Y2SLog.h>

#include <y2pm/InstSrcError.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/PMPackage.h>
#include <y2pm/Wget.h>
#include <y2pm/YouError.h>

#include <y2pm/PMYouPatchPaths.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchPaths
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMYouPatchPaths);

PMYouPatchPaths::PMYouPatchPaths()
{
  init("Dummy","1.0","i386");
}

PMYouPatchPaths::PMYouPatchPaths( const string &product, const string &version,
                                  const string &baseArch )
{
  init( product, version, baseArch );
}

void PMYouPatchPaths::init( const string &product, const string &version,
                            const string &baseArch )
{
  _product = product;
  _version = version;
  _baseArch = PkgArch( baseArch );

  _businessProduct = ( product != "SuSE-Linux" );

  string path = baseArch + "/update/";
  if ( _businessProduct  ) {
    path += product + "/";
    _patchUrl = Url( "http://support.suse.de/" );
  } else {
    _patchUrl = Url( "ftp://ftp.suse.com/pub/suse/" );
  }
  path += version;
  
  _patchPath = path + "/patches/";
  _rpmPath = path + "/rpm/";
  _scriptPath = path + "/scripts/";
}

void PMYouPatchPaths::setPatchPath( const Pathname &path )
{
  _patchPath = path;
}

Pathname PMYouPatchPaths::patchPath()
{
  return _patchPath;
}

void PMYouPatchPaths::setPatchUrl( const Url &url )
{
  _patchUrl = url;
}

Url PMYouPatchPaths::patchUrl()
{
  return _patchUrl;
}

Pathname PMYouPatchPaths::rpmPath( const PMPackagePtr &pkg )
{
  string rpmName = pkg->name();
  rpmName += "-";
  rpmName += pkg->version();
  rpmName += "-";
  rpmName += pkg->release();
  rpmName += ".";
  rpmName += baseArch();
  rpmName += ".rpm";
  return _rpmPath + rpmName;
}

Pathname PMYouPatchPaths::scriptPath( const string &scriptName )
{
  return _scriptPath + scriptName;
}

Pathname PMYouPatchPaths::localDir()
{
  return "/var/lib/YaST2/you/";
}

Pathname PMYouPatchPaths::installDir()
{
  return localDir() + "installed/";
}

Pathname PMYouPatchPaths::localSuseServers()
{
  return "/etc/suseservers";
}

string PMYouPatchPaths::product()
{
  return _product;
}

string PMYouPatchPaths::version()
{
  return _version;
}

PkgArch PMYouPatchPaths::baseArch()
{
  return _baseArch;
}

bool PMYouPatchPaths::businessProduct()
{
  return _businessProduct;
}

PMError PMYouPatchPaths::requestServers( const string &u, bool addFile )
{
  D__ << "url: '" << u << endl;

  // Check if YAST2_LOADFTPSERVER is set to yes
  const char *cmd ="grep 'YAST2_LOADFTPSERVER=' /etc/sysconfig/onlineupdate |"
                   "grep yes >/dev/null 2>/dev/null";
  int status = system( cmd );
  
  if ( status == 0 ) {
    string url = u;

    if ( addFile ) {
      if ( businessProduct() ) {
        url += "suseservers_http.txt";
      } else {
        url += "suseservers.txt";
      }
    }

    if ( url.empty() ) {
  //    url = "http://www.suse.de/de/support/download/";
      url = "http://localhost/cgi-bin/suseservers.cgi";
      url += "?product=" + product();
      url += "&version=" + version();
      url += "&basearch=" + string( baseArch() );
    }

    D__ << "url: '" << url << endl;

    MediaAccess media;

    Wget wget;

    WgetStatus status = wget.getFile( url, localSuseServers().asString() );

    if ( status != WGET_OK ) return PMError( YouError::E_get_suseservers_failed );
  }

  string line;
  ifstream in( localSuseServers().asString().c_str() );
  if ( in.fail() ) return PMError( YouError::E_read_suseservers_failed );
  while( getline( in, line ) ) {
    if ( *line.begin() != '#' ) {
      Url url( line );
      if ( url.isValid() ) _servers.push_back( url );
      D__ << "Mirror url: " << url.asString() << endl;
    }
  }

  return PMError();
}

list<Url> PMYouPatchPaths::servers()
{
  if ( _servers.size() == 0 ) {
    list<Url> servers;
    servers.push_back( defaultServer() );
    return servers;
  } else {
    return _servers;
  }
}

Url PMYouPatchPaths::defaultServer()
{
  if ( _servers.size() == 0 ) {
    if ( businessProduct() ) {
      return Url( "http://sdb.suse.de/download/" );
    } else {
      return Url( "ftp://ftp.suse.com/pub/suse/" );
    }
  } else {
    return *_servers.begin();
  }
}
