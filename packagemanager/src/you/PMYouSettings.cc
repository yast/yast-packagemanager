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

  Purpose: Provide path information for YOU patches.

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
#include <y2pm/PMYouProduct.h>

#include <y2pm/PMYouSettings.h>

using namespace std;

static const char *langmap[] = {
        "en_GB"         , "english",
        "en_US"         , "english",
        "en"            , "english",
        "de_DE"         , "german",
        "de_CH"         , "german",
        "de"            , "german",
        "fr"            , "french",
        "br_FR"         , "french",
        "fr_FR"         , "french",
        "fr_CH"         , "french",
        "it"            , "italian",
        "it_IT"         , "italian",
        "es"            , "spanish",
        "es_ES"         , "spanish",
        "nl"            , "dutch",
        "nl_NL"         , "dutch",
        "pt"            , "portuguese",
        "pt_PT"         , "portuguese",
        "pt_BR"         , "brazilian",
        "hu"            , "hungarian",
        "hu_HU"         , "hungarian",
        "pl"            , "polish",
        "pl_PL"         , "polish",
        "el_GR"         , "greek",
        "tr_TR"         , "turkish",
        "tr"            , "turkish",
        "ru"            , "russian",
        "ru_RU"         , "russian",
        "ru_RU.KOI8-R"  , "russian",
        "cs"            , "czech",
        "cs_CZ"         , "czech",
        "ja"            , "japanese",
        "ja_JP"         , "japanese",
        "ko"            , "korean",
        "ko_KR"         , "korean",
        0               , 0
};

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouSettings
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMYouSettings);

PMYouSettings::PMYouSettings()
{
  init("SuSE-Linux","9.1","i386");
}

PMYouSettings::PMYouSettings( const string &product, const string &version,
                              const string &baseArch )
{
  init( product, version, baseArch );
}

PMYouSettings::~PMYouSettings()
{
  if ( _config ) {
    _config->save();
    delete _config;
  }
}

void PMYouSettings::init( const string &product, const string &version,
                          const string &baseArch )
{
  _config = 0;

  _checkSignatures = true;
  _reloadPatches = true;
  _noExternalPackages = false;
  _dryRun = false;
  _getAll = false;
  _getOnly = false;
  _isLogEnabled = true;

  PMYouProductPtr p = new PMYouProduct( product, version, baseArch,
                                        *this );
  _products.push_back( p );

  MediaCurl::setCookieFile( cookiesFile() );

  setLangCode( LangCode() );
}

PMError PMYouSettings::initProduct()
{
  _products.clear();

  // make sure target and managers are up to date:
  Y2PM::instTargetUpdate();

  InstTarget &instTarget = Y2PM::instTarget();
  const std::list<constInstSrcDescrPtr> &products = instTarget.getProducts();

  if ( products.empty() ) {
    ERR << "No products installed." << endl;
    return YouError::E_error;
  }

  std::list<constInstSrcDescrPtr>::const_iterator it = products.begin();
  for( it = products.begin(); it != products.end(); ++it ) {
    PMYouProductPtr p = new PMYouProduct( *it, *this );
    DBG << "Product: " << p->product() << " " << p->version() << endl;
    if ( p->noYou() ) {
      DBG << "YOU ignores this product." << endl;
    } else {
      _products.push_back( p );
    }
  }

  return PMError();
}

void PMYouSettings::setPathPrefix( const Pathname &prefix )
{
  _pathPrefix = prefix;
}

Pathname PMYouSettings::pathPrefix()
{
  return _pathPrefix;
}

void PMYouSettings::filterExclusiveProducts( const list<string> &exclusiveProducts )
{
  if ( exclusiveProducts.empty() ) return;

  DBG << "Filter exclusive products" << endl;

  std::list<PMYouProductPtr>::iterator it = _products.begin();
  while( it != _products.end() ) {
    string product = (*it)->product() + "-" + (*it)->version();
    list<string>::const_iterator it2;
    for( it2 = exclusiveProducts.begin(); it2 != exclusiveProducts.end();
         ++it2 ) {
       if ( product == *it2 ) break;
    }
    if ( it2 == exclusiveProducts.end() ) {
      DBG << "Remove product " << product << endl;
      it = _products.erase( it );
    } else {
      ++it;
    }
  }
}

void PMYouSettings::setPatchServer( const PMYouServer &server )
{
  _patchServer = server;
}

PMYouServer PMYouSettings::patchServer()
{
  return _patchServer;
}

void PMYouSettings::setUsernamePassword( const string &username,
                                           const string &password )
{
  _patchServer.setUsernamePassword( username, password );
}

Url PMYouSettings::patchUrl()
{
  return _patchServer.url();
}

Pathname PMYouSettings::mediaPatchesFile()
{
  return "media.1/patches";
}

Pathname PMYouSettings::localDir()
{
  return "/var/lib/YaST2/you/";
}

Pathname PMYouSettings::localWriteDir()
{
  if ( getuid() == 0 ) {
    return localDir();
  } else {
    string homedir = getpwuid( getuid() )->pw_dir;
    Pathname path = homedir + "/.yast2/you";
    int err = PathInfo::assert_dir( path );
    if ( err ) {
      ERR << "Unable to assert '" << path << "' errno: " << err << endl;
    }
    return path;
  }
}

Pathname PMYouSettings::attachPoint()
{
  Pathname path = localWriteDir() + "mnt/";
  int err = PathInfo::assert_dir( path );
  if ( err ) {
    ERR << "Unable to assert '" << path << "' errno: " << err << endl;
  }
  return path;
}

Pathname PMYouSettings::rootAttachPoint()
{
  return localDir() + "mnt/";
}

Pathname PMYouSettings::installDir()
{
  return localDir() + "installed/";
}

Pathname PMYouSettings::externalRpmDir()
{
  return localWriteDir() + "external/";
}

Pathname PMYouSettings::filesDir()
{
  return localWriteDir() + "files/";
}

string PMYouSettings::directoryFileName()
{
  string dir = _patchServer.directory();
  if ( dir.empty() ) return "directory.3";
  else return dir;
}

string PMYouSettings::mediaMapFileName()
{
  return "mediamap";
}

Pathname PMYouSettings::cookiesFile()
{
  return localWriteDir() + "cookies";
}

Pathname PMYouSettings::configFile()
{
  return localDir() + "config";
}

Pathname PMYouSettings::passwordFile()
{
  return localWriteDir() + "password";
}

Pathname PMYouSettings::logFile()
{
  return localWriteDir() + "youlog";
}

SysConfig *PMYouSettings::config()
{
  if ( !_config ) {
    _config = new SysConfig( configFile() );
  }

  return _config;
}

void PMYouSettings::setLangCode( const LangCode &l )
{
  D__ << "Set lang code: " << l << endl;

  _lang = l;

  if ( ! _lang.isSet() ) _lang = Y2PM::getPreferredLocale();
}

string PMYouSettings::locale() const
{
  return translateLangCode( _lang );
}

string PMYouSettings::defaultLocale() const
{
  return "english";
}

string PMYouSettings::translateLangCode( const LangCode &lang )
{
  string result = lang.code();

  const char **code = langmap;
  while( *code ) {

    if ( *code == lang ) {
      result = *(code + 1);
      break;
    }

    code += 2;
  }

  D__ << "Translated " << lang << " to " << result << endl;

  return result;
}

list<PMYouProductPtr> PMYouSettings::products() const
{
  return _products;
}

PMYouProductPtr PMYouSettings::primaryProduct() const
{
  std::list<PMYouProductPtr>::const_iterator iter=_products.begin();
  while (iter != _products.end() && (*iter)->noYou())
    ++iter;

  if ( iter == _products.end() ) {
    ERR << "No known products." << endl;
    return 0;
  }
  else
    return *iter;
}

void PMYouSettings::setCheckSignatures( bool check )
{
  _checkSignatures = check;
}

bool PMYouSettings::checkSignatures() const
{
  return _checkSignatures;
}

/**
  Set if patches should be reloaded from the server.
*/
void PMYouSettings::setReloadPatches( bool reload )
{
  _reloadPatches = reload;
}

bool PMYouSettings::reloadPatches()
{
  return _reloadPatches;
}

void PMYouSettings::setNoExternalPackages( bool noExternal )
{
  _noExternalPackages = noExternal;
}

bool PMYouSettings::noExternalPackages() const
{
  return _noExternalPackages;
}

void PMYouSettings::setDryRun( bool dry )
{
  _dryRun = dry;
}

bool PMYouSettings::dryRun() const
{
  return _dryRun;
}

void PMYouSettings::setGetAll( bool dry )
{
  _getAll = dry;
}

bool PMYouSettings::getAll() const
{
  return _getAll;
}

void PMYouSettings::setGetOnly( bool dry )
{
  _getOnly = dry;
}

bool PMYouSettings::getOnly() const
{
  return _getOnly;
}

void PMYouSettings::setLogEnabled( bool e )
{
  _isLogEnabled = e;
}

bool PMYouSettings::isLogEnabled() const
{
  return _isLogEnabled;
}

void PMYouSettings::setMasterMedia( const PMYouMediaPtr &m )
{
  _masterMedia = m;
}

PMYouMediaPtr PMYouSettings::masterMedia()
{
  return _masterMedia;
}
