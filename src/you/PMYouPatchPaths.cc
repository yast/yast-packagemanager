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
#include <y2pm/YouError.h>
#include <y2pm/InstTarget.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectable.h>

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

PMYouPatchPaths::~PMYouPatchPaths()
{
  if ( _config ) {
    _config->save();
    delete _config;
  }
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

  _youUrl = defaultMirrorList();
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
  _config = 0;

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
  _distProduct = descr->content_distproduct().asString();

  DBG << "Product Name: " << product << endl;
  DBG << "Prpduct Version: " << version << endl;
  DBG << "BaseArch: " << baseArch << endl;
  DBG << "YouUrl: " << youUrl << endl;
  DBG << "YouType: " << youType << endl;
  DBG << "YouPatch: " << youPath << endl;
  DBG << "DistProduct: " << _distProduct << endl;

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

Pathname PMYouPatchPaths::localYouServers()
{
  return "/etc/youservers";
}

Pathname PMYouPatchPaths::cachedYouServers()
{
  return localDir() + "youservers";
}

string PMYouPatchPaths::directoryFileName()
{
  return "directory.3";
}

Pathname PMYouPatchPaths::cookiesFile()
{
  return localDir() + "cookies";
}

Pathname PMYouPatchPaths::configFile()
{
  return localDir() + "config";
}

SysConfig *PMYouPatchPaths::config()
{
  if ( !_config ) {
    _config = new SysConfig( configFile() );
  }
  
  return _config;
}

string PMYouPatchPaths::product()
{
  return _product;
}

string PMYouPatchPaths::distProduct()
{
  return _distProduct;
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
  DBG << "url: '" << u << "'" << endl;

  string lastServer = config()->readEntry( "LastServer" );
  if ( !lastServer.empty() ) {
    addServer( Url( lastServer ) );
  }

  PMError error = readServers( localYouServers() );
  if ( error ) return error;

  SysConfig cfg( "onlineupdate" );
  
  if ( cfg.readBoolEntry( "YAST2_LOADFTPSERVER", true ) ) {
    string url = u;

    if ( url.empty() ) {
      if ( _youUrl.empty() ) url = defaultMirrorList();
      else url = _youUrl;
      url += "?product=" + product();
      url += "&version=" + version();
      url += "&basearch=" + string( baseArch() );
      url += "&business=";
      if ( businessProduct() ) url += "1";
      else url += "0";

      url += "&distproduct=" + distProduct();

      addPackageVersion( "yast2-online-update", url );
      addPackageVersion( "yast2-packagemanager", url );
      addPackageVersion( "liby2util", url );
    }

    url = encodeUrl( url );

    DBG << "url: '" << url << "'" << endl;

    PMError error = MediaAccess::getFile( Url( url ), cachedYouServers() );
    if ( error ) {
      return PMError( YouError::E_get_suseservers_failed );
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

PMError PMYouPatchPaths::readServers( const Pathname &file )
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
    return PMError( YouError::E_read_suseservers_failed, file.asString() );
  }
  while( getline( in, line ) ) {
    if ( !line.empty() && *line.begin() != '#' ) {
      Url url( line );
      if ( url.isValid() ) addServer( url );
    }
  }

  return PMError();
}

void PMYouPatchPaths::addServer( const Url &url )
{
  D__ << "Mirror url: " << url << endl;

  list<Url>::const_iterator it = find( _servers.begin(), _servers.end(), url );
  if ( it == _servers.end() ) _servers.push_back( url );
}

void PMYouPatchPaths::addPackageVersion( const string &pkgName,
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

Url PMYouPatchPaths::currentServer()
{
  return *(servers().begin());
}

string PMYouPatchPaths::encodeUrl( const string &url )
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

string PMYouPatchPaths::mirrorList()
{
  return _youUrl;
}

string PMYouPatchPaths::defaultMirrorList()
{
  return "http://www.suse.de/cgi-bin/suseservers.cgi";
}
