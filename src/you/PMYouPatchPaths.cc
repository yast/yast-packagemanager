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
  DBG << "YouPath: " << youPath << endl;
  DBG << "DistProduct: " << _distProduct << endl;

  if ( youPath.empty() ) {
    init( product, version, baseArch );
  } else {
    init( product, version, baseArch, youUrl, youPath, youType == "business" );
  }

  InstSrcDescr::ArchMap archMap = descr->content_archmap();
  _arch = instTarget.baseArch();
  _archs = archMap[ _arch ];

  return PMError();
}

void PMYouPatchPaths::setPathPrefix( const Pathname &prefix )
{
  _pathPrefix = prefix;
}

Pathname PMYouPatchPaths::pathPrefix()
{
  return _pathPrefix;
}

void PMYouPatchPaths::setPatchPath( const Pathname &path )
{
  _patchPath = path;
}

Pathname PMYouPatchPaths::patchPath()
{
  return _pathPrefix + _patchPath;
}

void PMYouPatchPaths::setPatchUrl( const Url &url )
{
  _patchUrl = url;
}

Url PMYouPatchPaths::patchUrl()
{
  return _patchUrl;
}

Pathname PMYouPatchPaths::mediaPatchesFile()
{
  return "media.1/patches";
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
  return _pathPrefix + _rpmPath + rpmName;
}

Pathname PMYouPatchPaths::scriptPath( const string &scriptName )
{
  return _pathPrefix + _scriptPath + scriptName;
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

PkgArch PMYouPatchPaths::arch()
{
  return _arch;
}

string PMYouPatchPaths::youUrl()
{
  if ( _youUrl.empty() ) return "http://www.suse.de/cgi-bin/suseservers.cgi";
  else return _youUrl;
}
