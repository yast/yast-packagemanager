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

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

/-*/

#include <iostream>
#include <fstream>
#include <sstream>

#include <pwd.h>

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
#include <y2pm/MediaCurl.h>
#include <y2pm/PMYouSettings.h>

#include <y2pm/PMYouProduct.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouProduct
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMYouProduct);

PMYouProduct::PMYouProduct( const constInstSrcDescrPtr &descr,
                            PMYouSettings &settings )
  : _productDescr( descr ), _settings( settings )
{
  PkgNameEd prodEd = descr->content_product();

  string product = prodEd.name;
  string version = prodEd.edition.version();
  string baseArch = descr->content_defaultbase();
  string youUrl = descr->content_youurl();
  string youType = descr->content_youtype();
  string youPath = descr->content_youpath();
  _distProduct = descr->content_distproduct().asString();

  DBG << "Product Name: " << product << endl;
  DBG << "Product Version: " << version << endl;
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
  _arch = Y2PM::instTarget().baseArch();
  _archs = archMap[ _arch ];
}

PMYouProduct::PMYouProduct( const string &product, const string &version,
                            const string &baseArch,
                            PMYouSettings &settings )
  : _settings( settings )
{
  init( product, version, baseArch );
}

PMYouProduct::~PMYouProduct()
{
}

void PMYouProduct::init( const string &product, const string &version,
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

void PMYouProduct::init( const string &product, const string &version,
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

void PMYouProduct::init( const string &path )
{
  if ( _businessProduct  ) {
    _settings.setPatchServer( PMYouServer( "http://support.suse.de/" ) );
  } else {
    _settings.setPatchServer( PMYouServer( "ftp://ftp.suse.com/pub/suse/" ) );
  }

  _patchPath = path + "/patches/";
  _rpmPath = path + "/rpm/";
  _scriptPath = path + "/scripts/";
}

void PMYouProduct::setPatchPath( const Pathname &path )
{
  _patchPath = path;
}

Pathname PMYouProduct::patchPath()
{
  return _settings.pathPrefix() + _patchPath;
}

Pathname PMYouProduct::rpmPath( const PMPackagePtr &pkg, bool patchRpm )
{
  return rpmPath( pkg, baseArch(), patchRpm );
}

Pathname PMYouProduct::rpmPath( const PMPackagePtr &pkg, const string &arch,
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
  return _settings.pathPrefix() + _rpmPath + rpmName;
}

Pathname PMYouProduct::scriptPath( const string &scriptName )
{
  return _settings.pathPrefix() + _scriptPath + scriptName;
}

Pathname PMYouProduct::localScriptPath( const std::string &scriptName )
{
  return _settings.localWriteDir() + "scripts/" + scriptName;
}

string PMYouProduct::product()
{
  return _product;
}

string PMYouProduct::distProduct()
{
  return _distProduct;
}

string PMYouProduct::version()
{
  return _version;
}

PkgArch PMYouProduct::baseArch()
{
  return _baseArch;
}

bool PMYouProduct::businessProduct()
{
  return _businessProduct;
}

void PMYouProduct::setArchs( const list<PkgArch> &a )
{
  _archs = a;
}

list<PkgArch> PMYouProduct::archs()
{
  return _archs;
}

PkgArch PMYouProduct::arch()
{
  return _arch;
}

string PMYouProduct::youUrl()
{
  if ( _youUrl.empty() ) return "http://www.suse.de/cgi-bin/suseservers.cgi";
  else return _youUrl;
}

void PMYouProduct::setPatchFiles( const list<string> &patchFiles )
{
  _patchFiles = patchFiles;
}

list<string> PMYouProduct::patchFiles() const
{
  return _patchFiles;
}
