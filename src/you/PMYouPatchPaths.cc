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
#include <y2util/SysConfig.h>

#include <Y2PM.h>
#include <y2pm/InstSrcError.h>
#include <y2pm/PMPackage.h>
#include <y2pm/Wget.h>
#include <y2pm/YouError.h>
#include <y2pm/InstTarget.h>
#include <y2pm/InstSrcDescr.h>

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
  init("SuSE-Linux","8.1","i386");
}

PMYouPatchPaths::PMYouPatchPaths( const string &product, const string &version,
                                  const string &baseArch )
{
  init( product, version, baseArch );
}

void PMYouPatchPaths::init( const string &product, const string &version,
                            const string &baseArch, const string &url,
                            const string &path, bool business )
{
  _product = product;
  _version = version;
  _baseArch = PkgArch( baseArch );

  _youUrl = url;
  _businessProduct = business;

  init( path );
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
  }
  path += version;
  
  init ( path );
}

void PMYouPatchPaths::init( const string &path )
{  
  if ( _businessProduct  ) {
    _patchUrl = Url( "http://support.suse.de/" );
  } else {
    _patchUrl = Url( "ftp://ftp.suse.com/pub/suse/" );
  }

  _patchPath = path + "/patches/";
  _rpmPath = path + "/rpm/";
  _scriptPath = path + "/scripts/";
}

PMError PMYouPatchPaths::initProduct()
{
  InstTarget &instTarget = Y2PM::instTarget( true );
  const std::list<constInstSrcDescrPtr> &products = instTarget.getProducts();
  std::list<constInstSrcDescrPtr>::const_iterator it = products.begin();

  if ( it == products.end() ) {
    ERR << "No products installed." << endl;
    return YouError::E_error;
  }

  constInstSrcDescrPtr descr = *it;

  PkgNameEd prodEd = descr->content_product();

  string product = prodEd.name;
  string version = prodEd.edition.version();
  string baseArch = descr->content_defaultbase();
  string youUrl = descr->content_youurl();
  string youType = descr->content_youtype();
  string youPath = descr->content_youpath();

  DBG << "Product Name: " << product << endl;
  DBG << "Prpduct Version: " << version << endl;
  DBG << "BaseArch: " << baseArch << endl;
  DBG << "YouUrl: " << youUrl << endl;
  DBG << "YouType: " << youType << endl;
  DBG << "YouPatch: " << youPath << endl;

  if ( youPath.empty() ) {
    init( product, version, baseArch );
  } else {
    init( product, version, baseArch, youUrl, youPath, youType == "business" );
  }

  InstSrcDescr::ArchMap archMap = descr->content_archmap();
  string basearch = instTarget.baseArch();
  _archs = archMap[ basearch ];

  return PMError();
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

Pathname PMYouPatchPaths::rpmPath( const PMPackagePtr &pkg, bool patchRpm )
{
  return rpmPath( pkg, baseArch(), patchRpm );
}

Pathname PMYouPatchPaths::rpmPath( const PMPackagePtr &pkg, const string &arch,
                                   bool patchRpm )
{
  string rpmName = arch + "/";
  rpmName += pkg->name();
  rpmName += "-";
  rpmName += pkg->version();
  rpmName += "-";
  rpmName += pkg->release();
  rpmName += ".";
  rpmName += arch;
  if ( patchRpm ) rpmName += ".patch";
  rpmName += ".rpm";
  return _rpmPath + rpmName;
}

Pathname PMYouPatchPaths::scriptPath( const string &scriptName )
{
  return _scriptPath + scriptName;
}

Pathname PMYouPatchPaths::localScriptPath( const std::string &scriptName )
{
  return localDir() + "scripts/" + scriptName;
}

Pathname PMYouPatchPaths::localDir()
{
  return "/var/lib/YaST2/you/";
}

Pathname PMYouPatchPaths::attachPoint()
{
  return localDir() + "mnt/";
}

Pathname PMYouPatchPaths::installDir()
{
  return localDir() + "installed/";
}

Pathname PMYouPatchPaths::externalRpmDir()
{
  return localDir() + "external/";
}

Pathname PMYouPatchPaths::localSuseServers()
{
  return "/etc/suseservers";
}

string PMYouPatchPaths::directoryFileName()
{
  return "directory.3";
}

Pathname PMYouPatchPaths::cookiesFile()
{
  return localDir() + "cookies";
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

list<PkgArch> PMYouPatchPaths::archs()
{
  return _archs;
}

PMError PMYouPatchPaths::requestServers( const string &u )
{
  DBG << "url: '" << u << endl;

  SysConfig cfg( "onlineupdate" );
  
  if ( cfg.readBoolEntry( "YAST2_LOADFTPSERVER", true ) ) {
    string url = u;

    if ( url.empty() ) {
      if ( _youUrl.empty() ) url = "http://www.suse.de/cgi-bin/suseservers.cgi";
      else url = _youUrl;
      url += "?product=" + product();
      url += "&version=" + version();
      url += "&basearch=" + string( baseArch() );
      url += "&business=";
      if ( businessProduct() ) url += "1";
      else url += "0";
    }

    url = encodeUrl( url );

    DBG << "url: '" << url << endl;

    Wget wget;
    wget.setCookiesFile( cookiesFile().asString() );

    WgetStatus status = wget.getFile( Url(url), localSuseServers() );

    if ( status != WGET_OK ) {
      return PMError( YouError::E_get_suseservers_failed,
                      wget.error_string( status ) );
    }
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

string PMYouPatchPaths::encodeUrl( const string &url )
{
  D__ << url << endl;

  string result;
  
  string::const_iterator it;
  for( it = url.begin(); it != url.end(); ++it ) {
    if ( *it == ' ' ) result += "%20";
    else result += *it;
  }

  D__ << result << endl;

  return result;
}
