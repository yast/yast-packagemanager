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
}

void PMYouPatchPaths::setPatchPath( const Pathname &path )
{
  _patchPath = path;
}

Pathname PMYouPatchPaths::patchPath()
{
  return _patchPath;
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

void PMYouPatchPaths::setPatchUrl( const Url &url )
{
  _patchUrl = url;
}

Url PMYouPatchPaths::patchUrl()
{
  return _patchUrl;
}

Pathname PMYouPatchPaths::localDir()
{
  return "/var/lib/YaST2/you/";
}

Pathname PMYouPatchPaths::installDir()
{
  return localDir() + "installed/";
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

PMError PMYouPatchPaths::requestServers( const string &url, const string &file )
{
  D__ << "url: '" << url << "' file: '" << file << "'" << endl;

  Url u;
  if ( url.empty() ) {
    u = Url( "http://www.suse.de/de/support/download/" );
  } else {
    u = url;
    if ( !u.isValid() ) return InstSrcError::E_bad_url;
  }

  Pathname f;
  if ( file.empty() ) {
    if ( businessProduct() ) {
      f = "suseservers_http.txt";
    } else {
      f = "suseservers.txt";
    }
  } else {
    f = file;
  }

  D__ << "url: '" << u << "' file: '" << f.asString() << "'" << endl;

  MediaAccess media;

  PMError error = media.open( u );
  if ( error ) return error;

  error = media.attach();
  if ( error ) return error;

  error = media.provideFile( f );
  if ( error ) return error;

  string line;
  ifstream in( ( media.localPath( f ) ).asString().c_str() );
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
