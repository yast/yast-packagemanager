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

  File:       InstYou.cc

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: YaST Online Update.

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/GPGCheck.h>

#include <Y2PM.h>

#include <y2pm/PMYouPatch.h>
#include <y2pm/PMPackage.h>
#include <y2pm/YouError.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/Wget.h>
#include <y2pm/PMYouPackageDataProvider.h>
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/InstTarget.h>

#include <y2pm/InstYou.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstYou
//
///////////////////////////////////////////////////////////////////

InstYou::InstYou()
{
  _paths = new PMYouPatchPaths();
  _info = new PMYouPatchInfo();

  init();
}

InstYou::InstYou( const PMYouPatchInfoPtr &info,
                  const PMYouPatchPathsPtr &paths )
{
  _info = info;
  _paths = paths;

  init();
}

InstYou::~InstYou()
{
  _media.release();
}

void InstYou::init()
{
  _callbacks = 0;

  _selectedPatchesIt = _patches.begin();
  _progressTotal = 0;
  _progressCurrent = 0;
}

PMError InstYou::initProduct()
{
  return _paths->initProduct();
}

PMError InstYou::servers( list<Url> &servers )
{
  PMError error = _paths->requestServers();
  
  if ( error ) {
    ERR << "Error fetching servers." << endl;
    return error;
  }
  
  servers = _paths->servers();

  return PMError();
}

PMError InstYou::checkAuthorization( const Url &url, const string &regcode,
                                     const string &password )
{
  Url u( url );

  D__ << u << endl;
  
  u.setUsername( regcode );
  u.setPassword( password );

  string path = u.getPath();
  D__ << path << endl;
  if ( path.empty() ) path = "/";
  else if ( *(path.rbegin()) != '/' ) path += "/";
  D__ << path << endl;
  u.setPath( path );

  int err = PathInfo::assert_dir( _paths->localDir() );
  if ( err ) {
    ERR << "Can't create " << _paths->localDir() << " (errno: " << err << ")"
        << endl;
    return PMError( InstSrcError::E_error );
  }

  Pathname dummyFile = _paths->localDir() + "dummy";

  D__ << dummyFile << endl;
  D__ << u << endl;

  Wget wget;
  WgetStatus status = wget.getFile( u, dummyFile );

  if ( status == WGET_OK ) {
    _regcode = regcode;
    _password = password;
    PathInfo::unlink( dummyFile );
    return PMError();
  } else {
    if ( status == WGET_ERROR_LOGIN ) return YouError::E_auth_failed;
    else WAR << wget.error_string( status ) << endl;
  }

  return YouError::E_error;
}


PMError InstYou::retrievePatchInfo( const Url &url, bool checkSig )
{
  _patches.clear();

  Url u( url );
  if ( !_regcode.empty() && !_password.empty() ) {
    u.setUsername( _regcode );
    u.setPassword( _password );
  }
  _paths->setPatchUrl( u );

  PMError error = _info->getPatches( _paths, _patches, checkSig );
  if ( error ) {
    ERR << "Error downloading patchinfos: " << error << endl;
    return error;
  }

  Y2PM::youPatchManager().poolAddCandidates( _patches );

  list<PMYouPatchPtr>::const_iterator itPatch;
  for( itPatch = _patches.begin(); itPatch != _patches.end(); ++itPatch ) {
    Y2PM::packageManager().poolAddCandidates( (*itPatch)->packages() );
  }

  return PMError();
}

void InstYou::selectPatches( int kinds )
{
  PMSelectablePtr yastPatch;

  list<PMYouPatchPtr>::const_iterator it;
  for( it = _patches.begin(); it != _patches.end(); ++it ) {
    if ( ( (*it)->kind() == PMYouPatch::kind_yast )
         && hasNewPackages( *it, false ) ) {
      PMSelectablePtr selectable = (*it)->getSelectable();
      if ( !selectable ) {
        INT << "Patch has no selectable." << endl;
        return;
      }
      yastPatch = selectable;
    }
  }

  if ( yastPatch ) {
    yastPatch->user_set_install();
    D__ << "Select yast patch: " << yastPatch->name() << endl;
  } else {
    for ( it = _patches.begin(); it != _patches.end(); ++it ) {
      D__ << "Check patch " << (*it)->fullName() << " ("
          << (*it)->kindLabel() << ")" << endl;
      if ( ( (*it)->kind() & kinds ) && hasNewPackages( *it, true ) ) {
        PMSelectablePtr selectable = (*it)->getSelectable();
        if ( !selectable ) {
          INT << "Patch has no selectable." << endl;
          return;
        }
      
        PMYouPatchPtr candidate = selectable->candidateObj();
        D__ << "Patch " << (*it)->fullName();
        if ( candidate ) D__ << " has candidate." << endl;
        else D__ << " has no candidate." << endl;
        D__ << "Kind: " << candidate->kindLabel() << endl;
        if ( candidate && ( candidate->kind() & kinds ) ) {
          D__ << "Select patch: " << (*it)->fullName() << endl;
          selectable->user_set_install();
        }
      }
    }
  }
}

void InstYou::updatePackageStates()
{
  list<PMYouPatchPtr>::const_iterator it;
  for ( it = _patches.begin(); it != _patches.end(); ++it ) {
    bool toInstall = false;
    PMSelectablePtr selectable = (*it)->getSelectable();
    if ( selectable && selectable->to_install() &&
         *it == selectable->candidateObj() ) {
      toInstall = true;
    }

    list<PMPackagePtr> packages = (*it)->packages();
    list<PMPackagePtr>::const_iterator itPkg;
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
      bool pkgToInstall = toInstall;

      if ( (*it)->updateOnlyInstalled() && !(*itPkg)->hasInstalledObj() ) {
        pkgToInstall = false;
      }

      PMSelectablePtr selectablePkg = (*itPkg)->getSelectable();
      if ( selectablePkg ) {
        if ( pkgToInstall ) {
          selectablePkg->setUserCandidate( *itPkg );
          selectablePkg->user_set_install();
        } else {
          selectablePkg->clrUserCandidate();
          selectablePkg->user_unset();
        }
      } else {
        INT << "Package has no Selectable." << endl;
      }
    }
  }
}

PMError InstYou::attachSource()
{
  int err = PathInfo::assert_dir( _paths->attachPoint() );
  if ( err ) {
    ERR << "Can't create " << _paths->attachPoint() << " (errno: " << err << ")"
        << endl;
    return PMError( InstSrcError::E_error );
  }

  PMError error = _media.open( _paths->patchUrl(), _paths->attachPoint() );
  if ( error ) {
    ERR << "Error opening URL '" << _paths->patchUrl() << "'" << endl;
    return error;
  }
  error = _media.attach();
  if ( error ) {
    ERR << "Error attaching media." << endl;
  }

  return error;
}

PMError InstYou::retrievePatches( bool checkSig, bool noExternal )
{
  D__ << "Retrieve patches." << endl;

  PMError error = attachSource();
  if ( error ) return error;

  PMYouPatchPtr patch;
  for( patch = firstPatch(); patch; patch = nextPatch() ) {
    error = retrievePatch( patch, checkSig, noExternal );
    if ( error ) return error;
  }

  return error;
}

PMYouPatchPtr InstYou::firstPatch( bool resetProgress )
{
  if ( resetProgress ) {
    _progressCurrent = 0;
    _progressTotal = 0; 
    std::list<PMYouPatchPtr>::const_iterator it;
    for ( it = _patches.begin(); it != _patches.end(); ++it ) {
      PMSelectablePtr selectable = (*it)->getSelectable();
      if ( selectable && selectable->to_install() &&
           *it == selectable->candidateObj() ) {
        _progressTotal++;
      }
    }
    
    _progressTotal *= 2;
  }

  D__ << "_progressTotal: " << _progressTotal << endl;

  _selectedPatchesIt = _patches.begin();

  return nextSelectedPatch();
}

PMYouPatchPtr InstYou::nextPatch( bool *ok )
{
  ++_selectedPatchesIt;

  PMError err = progress( ++_progressCurrent * 100 / _progressTotal );

  if ( ok ) {
    if ( err == YouError::E_user_abort ) *ok = false;
    else *ok = true;
  }

  return nextSelectedPatch();
}

PMYouPatchPtr InstYou::nextSelectedPatch()
{
  while ( _selectedPatchesIt != _patches.end() ) {
    PMSelectablePtr selectable = (*_selectedPatchesIt)->getSelectable();
    if ( selectable && selectable->to_install() &&
         *_selectedPatchesIt == selectable->candidateObj() ) {
      return *_selectedPatchesIt;
    }
    ++_selectedPatchesIt;
  }

  return PMYouPatchPtr();
}

PMError InstYou::installCurrentPatch()
{
  D__ << "Install current Patch" << endl;

  if ( _selectedPatchesIt == _patches.end() ) {
    E__ << "No more patches." << endl;
    return PMError( InstSrcError::E_error );
  }

  return installPatch( *_selectedPatchesIt );
}

PMError InstYou::retrieveCurrentPatch( bool checkSig )
{
  if ( _selectedPatchesIt == _patches.end() ) {
    E__ << "No more patches." << endl;
    return PMError( InstSrcError::E_error );
  }

  return retrievePatch( *_selectedPatchesIt, checkSig );
}

PMError InstYou::installPatches( bool dryrun )
{
  PMYouPatchPtr patch;
  for( patch = firstPatch(); patch; patch = nextPatch() ) {
    PMError error = installPatch( patch, dryrun );
    if ( error ) return error;
  }

  return PMError();
}

PMError InstYou::installPatch( const PMYouPatchPtr &patch, bool dryrun )
{
  D__ << "INSTALL PATCH: " << patch->name() << endl;

  PMError error = patchProgress( 0 );
  if ( error ) return error;

  Pathname scriptPath;
  if ( !patch->preScript().empty() ) {
    scriptPath = _paths->localScriptPath( patch->preScript() );
    if ( dryrun ) {
      cout << "PRESCRIPT: " << scriptPath << endl;
    } else {
      Y2PM::instTarget().executeScript( scriptPath );
    }
  }

  error = patchProgress( 1 );
  if ( error ) return error;

  list<PMPackagePtr> packages = patch->packages();
  int progressTotal = packages.size() + 2; // number of packages plus pre and post script
  int progressCurrent = 0;
  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    error = patchProgress( ( progressCurrent++ + 1 ) * 100 /
                           progressTotal );
    if ( error ) return error;
    if ( patch->updateOnlyInstalled() ) {
      if ( !(*itPkg)->hasInstalledObj() ) {
        D__ << "Don't install '" << (*itPkg)->name()
            << "', no installed obj and UpdateOnlyInstalled=true." << endl;
        continue;
      }
    }

    Pathname fileName;
    if ( (*itPkg)->externalUrl().empty() ) {
      fileName = _media.localPath( (*itPkg)->location() );
    } else {
      fileName = (*itPkg)->location();
    }
    D__ << "INSTALL PKG " << fileName << endl;  
    if ( dryrun ) {
      cout << "INSTALL: " << fileName << endl;
    } else {
      error = Y2PM::instTarget().installPackage( fileName.asString() );
      if ( error ) {
        E__ << "Installation of RPM " << fileName << " of patch "
            << patch->name() << "failed" << endl;
        return error;
      }
    }
  }

  error = patchProgress( 99 );
  if ( error ) return error;

  if ( !dryrun ) {
    error = Y2PM::instTarget().installPatch( patch->localFile() );
    if ( error ) {
      E__ << "Error installing patch info." << endl;
      return error;
    }
  }

  if ( !patch->postScript().empty() ) {
    scriptPath = _paths->localScriptPath( patch->postScript() );
    if ( dryrun ) {
      cout << "POSTSCRIPT: " << scriptPath << endl;
    } else {
      Y2PM::instTarget().executeScript( scriptPath );
    }
  }

  error = patchProgress( 100 );
  if ( error ) return error;
  
  return error;
}

PMError InstYou::retrievePatch( const PMYouPatchPtr &patch, bool checkSig,
                                bool noExternal )
{
  D__ << "PATCH: " << patch->name() << endl;

  RpmDb rpm;

  list<PMPackagePtr> packages = patch->packages();
  int progressTotal = packages.size() + 2; // number of packages plus pre and post script
  int progressCurrent = 0;
  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    PMError error = patchProgress( progressCurrent++ * 100 /
                                   progressTotal );
    if ( error ) return error;
    if ( patch->updateOnlyInstalled() ) {
      if ( !(*itPkg)->hasInstalledObj() ) {
        D__ << "Don't download '" << (*itPkg)->name()
            << "', no installed obj and UpdateOnlyInstalled=true." << endl;
        continue;
      }
    }

    error = retrievePackage( *itPkg, noExternal );
    if ( error ) return error;
    if ( checkSig ) {
      string localRpm;
      string externalUrl = (*itPkg)->externalUrl();
      if ( externalUrl.empty() ) {
        localRpm = _media.localPath( (*itPkg)->location() ).asString();
      } else {
        localRpm = (*itPkg)->location();
      }
      unsigned result = rpm.checkPackage( localRpm );
      if ( result != 0 ) {
        ERR << "Signature check failed for " << localRpm << endl;
        return PMError( YouError::E_bad_sig_rpm );
      }
    }
  }

  PMError error = patchProgress( 98 );
  if ( error ) return error;
  
  Pathname scriptPath;
  if ( !patch->preScript().empty() ) {
    PMError error = retrieveScript( patch->preScript(), checkSig );
    if ( error ) {
      ERR << "Error retrieving preScript." << endl;
      return error;
    }
  }
  
  error = patchProgress( 99 );
  if ( error ) return error;
  
  if ( !patch->postScript().empty() ) {
    PMError error = retrieveScript( patch->postScript(), checkSig );
    if ( error ) {
      ERR << "Error retrieving postScript." << endl;
      return error;
    }
  }

  error = patchProgress( 100 );
  if ( error ) return error;
  
  return PMError();
}

PMError InstYou::retrieveScript( const string &script, bool checkSig )
{
  Pathname scriptPath = _paths->scriptPath( script );
  PMError error = _media.provideFile( scriptPath );
  if ( error ) {
    ERR << "Error downloading script from '"
        << _paths->patchUrl() << "/" << scriptPath << "'" << endl;
    return error;
  }

  int e = PathInfo::assert_dir( _paths->localScriptPath( "" ) );
  if ( e ) {
    ERR << "Can't create " << _paths->localScriptPath( "" ) << " (errno: "
        << e << ")" << endl;
    return PMError( InstSrcError::E_error );
  }

  string sourceScript = _media.localPath( scriptPath ).asString();
  string destScript = _paths->localScriptPath( script ).asString();

  GPGCheck gpg;
  bool ok = gpg.check_file( sourceScript, destScript );

  if ( checkSig && !ok ) {
    ERR << "Signature check failed for script " << sourceScript << endl;
    return PMError( YouError::E_bad_sig_file );
  }
  
  return PMError();
}

PMError InstYou::retrievePackage( const PMPackagePtr &pkg, bool noExternal )
{
  DBG << "InstYou::retrievePackage: '" << pkg->name() << "'" << endl;

  string externalUrl = pkg->externalUrl();

  if ( !externalUrl.empty() ) {
    Url url( externalUrl );
    if ( !url.isValid() ) {
      ERR << "Invalid URL: '" << url.asString() << "'" << endl;
      return InstSrcError::E_bad_url;
    }

    int e = PathInfo::assert_dir( _paths->externalRpmDir() );
    if ( e ) {
      ERR << "Can't create " << _paths->externalRpmDir() << " (errno: "
          << e << ")" << endl;
      return PMError( InstSrcError::E_error );
    }

    Pathname path = url.getPath();
    url.setPath( "" );

    MediaAccess media;

    PMError err = media.open( url, _paths->externalRpmDir() );
    if ( err ) return err;

    if ( !noExternal ) {
      err = media.attach();
      if ( err ) return err;

      err = media.provideFile( path );
      if ( err ) return err;
    }

    _info->packageDataProvider()->setLocation( pkg, media.localPath( path ).asString() );

    return PMError();
  }

  list<PkgArch> archs = _paths->archs();

  if ( pkg->hasInstalledObj() ) {
    archs.push_front( pkg->getInstalledObj()->arch() );
  }

  PMError error( YouError::E_error );
  Pathname rpmPath;
  list<PkgArch>::const_iterator it;
  for( it = archs.begin(); it != archs.end(); ++it ) {
    D__ << "ARCH: " << *it << endl;
    
    // If the package has a version installed, try to get patch RPM first.
    bool pkgHasInstalledObj = pkg->hasInstalledObj();

    bool patchRpm = false;
    if ( pkgHasInstalledObj ) {
      PkgEdition installedVersion = pkg->getInstalledObj()->edition();
      D__ << "Installed: " << installedVersion.asString() << endl;
      list<PkgEdition> baseVersions = pkg->patchRpmBaseVersions();
      list<PkgEdition>::const_iterator it2;
      for( it2 = baseVersions.begin(); it2 != baseVersions.end(); ++it2 ) {
        if ( *it2 == installedVersion ) {
          patchRpm = true;
          break;
        }
      }
    }

    rpmPath = _paths->rpmPath( pkg, *it, patchRpm );
    D__ << "Trying downloading '" << _paths->patchUrl() << "/" << rpmPath
        << "'" << endl;
    error = _media.provideFile( rpmPath );
    if ( error ) {
      D__ << "Downloading RPM '" << pkg->name() << "' failed: " << error
          << endl;
          
      // If patch RPM was requested first, try to get full RPM now.
      if ( patchRpm ) {
        rpmPath = _paths->rpmPath( pkg, *it, false );
        D__ << "Trying downloading '" << _paths->patchUrl() << "/" << rpmPath
            << "'" << endl;
        error = _media.provideFile( rpmPath );
        if ( error ) {
          D__ << "Download failed." << endl;
        }
      }
    }

    // If download was successful store path to RPM and return.
    if ( !error ) {
      _info->packageDataProvider()->setLocation( pkg, rpmPath.asString() );
      DBG << "RPM: " << pkg->name() << ": " << pkg->location() << endl;
      return PMError();
    }
  }

  ERR << "Error downloading RPM " << pkg->name() << endl;
  return error;
}

PMError InstYou::removePackages()
{
  PMYouPatchPtr patch;
  for( patch = firstPatch(); patch; patch = nextPatch() ) {
    list<PMPackagePtr> packages = patch->packages();
    list<PMPackagePtr>::const_iterator itPkg;
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
      string externalUrl = (*itPkg)->externalUrl();
      if ( externalUrl.empty() ) {
        PMError error = _media.releaseFile( (*itPkg)->location() );
        if ( error ) {
          ERR << "Can't release " << (*itPkg)->location() << endl;
          return error;
        }
      } else {
        PathInfo::unlink( (*itPkg)->location() );
      }
    }
    if ( !patch->preScript().empty() ) {
      Pathname scriptPath = _paths->scriptPath( patch->preScript() );
      PMError error = _media.releaseFile( scriptPath );
      if ( error ) {
        ERR << "Can't release " << scriptPath.asString() << endl;
        return error;
      }
      PathInfo::unlink( _paths->localScriptPath( patch->preScript() ) );
    }
    if ( !patch->postScript().empty() ) {
      Pathname scriptPath = _paths->scriptPath( patch->postScript() );
      PMError error = _media.releaseFile( scriptPath );
      if ( error ) {
        ERR << "Can't release " << scriptPath.asString() << endl;
        return error;
      }
      PathInfo::unlink( _paths->localScriptPath( patch->postScript() ) );
    }
  }

  return PMError();
}

void InstYou::showPatches( bool verbose )
{
  list<PMYouPatchPtr>::const_iterator it;
  for( it = _patches.begin(); it != _patches.end(); ++it ) {
    PMSelectablePtr sel = (*it)->getSelectable();
    if ( sel ) {
      if ( sel->has_installed() ) cout << "I";
      else cout << " ";
      if ( sel->to_install() ) cout << "S";
      else cout << " ";
      cout << " ";
    }
    cout << (*it)->name() << " (" << (*it)->kindLabel( (*it)->kind() ) << "): "
         << (*it)->shortDescription() << endl;
    if ( verbose ) {
    list<PMPackagePtr> packages = (*it)->packages();
    list<PMPackagePtr>::const_iterator itPkg;
      for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
        cout << "     RPM: " << (*itPkg)->name() << " " << (*itPkg)->edition()
             << " (";
        if ( (*itPkg)->hasInstalledObj() ) {
          PkgEdition instEd = (*itPkg)->getInstalledObj()->edition();
          cout << "installed: " << instEd << " ";
          if ( instEd < (*itPkg)->edition() ) cout << "[older]";
          else if ( instEd > (*itPkg)->edition() ) cout << "[newer]";
          else cout << "[same]";
        } else {
          cout << "not installed";
        }
        cout << ")" << endl;
      }
    }
  }
}

bool InstYou::hasNewPackages( const PMYouPatchPtr &patch,
                              bool requireInstalled )
{
  bool install = false;

  // Check, if patch contains at least one package which is newer than the
  // correpsonding package on the system. If yes, trigger install.
  // If a package from the patch is older than the installed package. don't
  // trigger install.
  list<PMPackagePtr> packages = patch->packages();
  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    PkgEdition candEd = (*itPkg)->edition();
    D__ << "  PKG: " << (*itPkg)->name() << "-" << candEd << endl;
    if ( (*itPkg)->hasInstalledObj() ) {
      PkgEdition instEd = (*itPkg)->getInstalledObj()->edition();
      if ( instEd > candEd ) {
        install = false;
        break;
      } else if ( instEd < candEd ) {
        install = true;
      }
    } else if ( !requireInstalled ) {
      install = true;
    }
  }

  D__ << "hasNewPackages: " << patch->fullName() << " "
      << ( install ? "yes" : "no" ) << endl;

  return install;
}

bool InstYou::firesPackageTrigger( const PMYouPatchPtr &patch )
{
  return false;
}

bool InstYou::firesScriptTrigger( const PMYouPatchPtr &patch )
{
  return false;
}

void InstYou::setCallbacks( InstYou::Callbacks *callbacks )
{
  _callbacks = callbacks;
}

PMError InstYou::progress( int i )
{
  bool ret = true;

  if ( _callbacks ) {
    ret = _callbacks->progress( i );
  }

  INT << "ret: " << ret << endl;

  if ( ret ) return PMError();
  else return YouError::E_user_abort;
}

PMError InstYou::patchProgress( int i, const string &pkg )
{
  bool ret = true;

  if ( _callbacks ) {
    ret = _callbacks->patchProgress( i, pkg );
  }

  INT << "ret: " << ret << endl;

  if ( ret ) return PMError();
  else return YouError::E_user_abort;
}
