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
#include <y2util/SysConfig.h>

#include <Y2PM.h>

#include <y2pm/PMYouPatch.h>
#include <y2pm/PMPackage.h>
#include <y2pm/YouError.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/PMYouPackageDataProvider.h>
#include <y2pm/PMYouPatchManager.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/InstTarget.h>
#include <y2pm/InstTargetError.h>
#include <y2pm/PMYouServers.h>
#include <y2pm/MediaCurl.h>

#include <y2pm/InstYou.h>

using namespace std;

InstYou::Callbacks *InstYou::_callbacks = 0;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstYou
//
///////////////////////////////////////////////////////////////////

InstYou::InstYou()
{
  _paths = new PMYouPatchPaths();
  _info = new PMYouPatchInfo( _paths );

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
  if ( _media.isOpen() && _media.isAttached() ) {
    _media.release();
  }
}

void InstYou::init()
{
  _selectedPatchesIt = _patches.begin();
  _progressTotal = 0;
  _progressCurrent = 0;
}

PMError InstYou::initProduct()
{
  return _paths->initProduct();
}

PMError InstYou::servers( list<PMYouServer> &servers )
{
  PMYouServers youServers( _paths );

  PMError error = youServers.requestServers();

  if ( error ) {
    ERR << "Error fetching servers: " << error << endl;
    return error;
  }

  servers = youServers.servers();

  return PMError();
}

PMError InstYou::readUserPassword()
{
  SysConfig cfg( _paths->passwordFile() );
  _username = cfg.readEntry( "USERNAME_" + _paths->patchUrl().asString() );
  _password = cfg.readEntry( "PASSWORD_" + _paths->patchUrl().asString() );

  _paths->setUsernamePassword( _username, _password );

  return PMError();
}

PMError InstYou::setUserPassword( const string &username,
                                  const string &password,
                                  bool persistent )
{
  _username = username;
  _password = password;

  string u,p;
  if ( persistent ) {
    u = _username;
    p = _password;
  }

  SysConfig cfg( _paths->passwordFile() );
  cfg.writeEntry( "USERNAME_" + _paths->patchUrl().asString(), u );
  cfg.writeEntry( "PASSWORD_" + _paths->patchUrl().asString(), p );
  cfg.save();

  PathInfo::chmod( _paths->passwordFile(), 0600 );

  return PMError();
}

PMError InstYou::retrievePatchDirectory()
{
  if ( !_username.empty() && !_password.empty() ) {
    _paths->setUsernamePassword( _username, _password );
  }

  PMError error = _info->getDirectory( true );

  return error;
}

PMError InstYou::retrievePatchInfo( bool reload, bool checkSig )
{
  D__ << "retrievePatchInfo()" << endl;

  _patches.clear();

  if ( !_username.empty() && !_password.empty() ) {
    _paths->setUsernamePassword( _username, _password );
  }

  PMError error = _info->getPatches( _patches, reload, checkSig );
  if ( error ) {
    ERR << "Error downloading patchinfos: " << error << endl;
    return error;
  }


  Y2PM::youPatchManager().poolAddCandidates( _patches );

  list<PMYouPatchPtr>::iterator itPatch;
  for( itPatch = _patches.begin(); itPatch != _patches.end(); ++itPatch ) {
    filterArchitectures( *itPatch );
    Y2PM::packageManager().poolAddCandidates( (*itPatch)->packages() );
  }

  // Set correct size depending on if the patch RPM is used or the full one and
  // set the packagesInstalled flag
  for( itPatch = _patches.begin(); itPatch != _patches.end(); ++itPatch ) {
    D__ << "Patch: " << (*itPatch)->name() << endl;
    bool packagesInstalled = false;
    FSize patchSize;

    list<PMPackagePtr> packages = (*itPatch)->packages();
    list<PMPackagePtr>::const_iterator itPkg;
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
      D__ << "  Package: " << (*itPkg)->name() << endl;
      if ( hasPatchRpm( *itPkg ) ) {
        _info->packageDataProvider()->setArchiveSize( *itPkg, (*itPkg)->patchRpmSize() );
        D__ << "    Using patch RPM" << endl;
      }
      FSize archiveSize = (*itPkg)->archivesize();
      D__ << "    ArchiveSize: " << archiveSize << endl;
      patchSize += archiveSize;

      if ( (*itPkg)->hasInstalledObj() ) packagesInstalled = true;
    }

    list<PMYouFile> files = (*itPatch)->files();
    list<PMYouFile>::const_iterator itFile;
    for ( itFile = files.begin(); itFile != files.end(); ++itFile ) {
      D__ << "  File " << itFile->name() << ": Size: " << itFile->size()
          << endl;
      patchSize += itFile->size();
    }

    (*itPatch)->setPatchSize( patchSize );
    D__ << "  PatchSize: " << patchSize << endl;

    (*itPatch)->setPackagesInstalled( packagesInstalled );
  }

  return PMError();
}

void InstYou::selectPatches( int kinds )
{
  PMSelectablePtr yastPatch;

  list<PMYouPatchPtr>::const_iterator it;
  for( it = _patches.begin(); it != _patches.end(); ++it ) {
    if ( ( (*it)->kind() == PMYouPatch::kind_yast )
         && hasNewPackages( *it, true ) ) {
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
    DBG << "Select yast patch: " << yastPatch->name() << endl;
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
        if ( candidate ) {
          D__ << " has candidate." << endl;
          D__ << "Kind: " << candidate->kindLabel() << endl;
          if ( candidate->kind() & kinds ) {
            DBG << "Select patch: " << (*it)->fullName() << endl;
            selectable->user_set_install();
          }
        } else D__ << " has no candidate." << endl;
      }
    }
  }

  // Read Taboo states
  Y2PM::youPatchManager().readSettings();

  updatePackageStates();
}

void InstYou::updatePackageStates()
{
//  D__ << "updatePackageStates()" << endl;

  _totalDownloadSize = 0;

  list<PMYouPatchPtr>::const_iterator it;
  for ( it = _patches.begin(); it != _patches.end(); ++it ) {
//    D__ << "Patch: " << (*it)->name() << endl;

    bool toInstall = false;
    PMSelectablePtr selectable = (*it)->getSelectable();
    if ( selectable && selectable->to_install() &&
         *it == selectable->candidateObj() ) {
//      D__ << "toInstall: true" << endl;
      toInstall = true;
    }

    list<PMPackagePtr> packages = (*it)->packages();
    list<PMPackagePtr>::const_iterator itPkg;
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
//      D__ << "  Package: " << (*itPkg)->name() << endl;

      bool pkgToInstall = toInstall;

      if ( (*it)->updateOnlyInstalled() && !(*itPkg)->hasInstalledObj() ) {
//        D__ << "pkgToInstall: false" << endl;
        pkgToInstall = false;
      }

      PMSelectablePtr selectablePkg = (*itPkg)->getSelectable();
      if ( selectablePkg ) {
        if ( pkgToInstall ) {
          selectablePkg->setUserCandidate( *itPkg );
          selectablePkg->user_set_install();
          _totalDownloadSize += (*itPkg)->archivesize();
        } else {
          selectablePkg->clrUserCandidate();
          selectablePkg->user_unset();
        }
      } else {
        INT << "Package has no Selectable." << endl;
      }
    }

    if ( toInstall ) {
      list<PMYouFile> files = (*it)->files();
      list<PMYouFile>::const_iterator itFile;
      for( itFile = files.begin(); itFile != files.end(); ++itFile ) {
        _totalDownloadSize += itFile->size();
      }
    }
  }

  D__ << _totalDownloadSize << endl;
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

PMError InstYou::retrievePatches( bool reload, bool checkSig, bool noExternal )
{
  D__ << "Retrieve patches." << endl;

  PMError error = attachSource();
  if ( error ) return error;

  PMYouPatchPtr patch;
  for( patch = firstPatch(); patch; patch = nextPatch() ) {
    error = retrievePatch( patch, reload, checkSig, noExternal );
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
  ++_progressCurrent;

  int p;
  if ( _progressTotal == 0 ) p = 0;
  else p = _progressCurrent * 100 / _progressTotal;

  PMError err = progress( p );

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
    ERR << "No more patches." << endl;
    return PMError( YouError::E_error );
  }

  return installPatch( *_selectedPatchesIt );
}

PMError InstYou::retrieveCurrentPatch( bool reload, bool checkSig )
{
  if ( _selectedPatchesIt == _patches.end() ) {
    ERR << "No more patches." << endl;
    return PMError( YouError::E_error );
  }

  return retrievePatch( *_selectedPatchesIt, reload, checkSig );
}

PMError InstYou::installPatches( bool dryrun )
{
  PMYouPatchPtr patch;
  for( patch = firstPatch(); patch; patch = nextPatch() ) {
    PMError error = installPatch( patch, dryrun );
    if ( error ) return error;
  }

  writeLastUpdate();

  return PMError();
}

PMError InstYou::installPatch( const PMYouPatchPtr &patch, bool dryrun )
{
  D__ << "INSTALL PATCH: " << patch->name() << endl;

  PMError error = patchProgress( 0 );
  if ( error ) return error;

  list<PMPackagePtr> packages = patch->packages();

  int progressTotal = packages.size() + 2; // number of packages plus pre and post script
  int progressCurrent = 0;

  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    if ( (*itPkg)->location().empty() ) {
      if ( !patch->updateOnlyInstalled() || (*itPkg)->hasInstalledObj() ) {
        return YouError::E_empty_location;
      }
    }
  }

  error = executeScript( patch->preScript(), dryrun );
  D__ << "Prescript: " << error << endl;
  if ( error ) {
    if ( error == YouError::E_user_abort ) return error;
    else return PMError( YouError::E_prescript_failed, error.details() );
  }

  error = patchProgress( 1 );
  if ( error ) return error;

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
        return PMError( YouError::E_rpm_failed, fileName.asString() );
      }
    }
  }

  error = patchProgress( 99 );
  if ( error ) return error;

  if ( !dryrun ) {
    error = Y2PM::instTarget().installPatch( patch->localFile() );
    if ( error ) {
      E__ << "Error installing patch info." << endl;
      return PMError( YouError::E_install_failed,
                      patch->localFile().asString() );
    }
  }

  error = executeScript( patch->postScript(), dryrun );
  D__ << "Postscript: " << error << endl;
  if ( error ) {
    if ( error == YouError::E_user_abort ) return error;
    else return PMError( YouError::E_postscript_failed, error.details() );
  }

  error = patchProgress( 100 );
  if ( error ) return error;

  return error;
}

PMError InstYou::executeScript( const string &script, bool dryrun )
{
  if ( script.empty() ) return PMError();

  Pathname scriptPath = _paths->localScriptPath( script );
  if ( dryrun ) {
    cout << "SCRIPT: " << scriptPath << endl;
  } else {
    string extension;
    int pos = script.find( '.' );
    if ( pos >= 0 ) {
      extension = script.substr( pos + 1, script.length() - pos );
    }

    D__ << "Script extension: " << extension << endl;

    PMError error;
    if ( extension == "ycp" ) {
      if ( !_callbacks ) return PMError( YouError::E_callback_missing );

      if ( !_callbacks->executeYcpScript( scriptPath.asString() ) ) {
        return PMError( YouError::E_script_failed, scriptPath.asString() );
      }
    } else {
      PMError error = Y2PM::instTarget().executeScript( scriptPath );
      if ( error ) {
        if ( error == InstTargetError::E_user_abort ) {
          return PMError( YouError::E_user_abort );
        } else {
          return PMError( YouError::E_script_failed, scriptPath.asString() );
        }
      }
    }
  }

  return PMError();
}

class CurlCallbacks : public MediaCurl::Callbacks
{
  public:
    CurlCallbacks( InstYou *instYou, int total )
      : _total( total ), _current( 0 ), _instYou( instYou )
    {
    }

    bool progress( int percent )
    {
      D__ << "Curl Progress: " << percent << endl;

      PMError error = _instYou->patchProgress( ( _current + percent ) * 100 /
                                               _total );

      if ( error ) {
        return false;
      } else {
        return true;
      }
    }

    void setBaseProgress( int current )
    {
      _current = current;
    }

  private:
    int _total;
    int _current;
    InstYou *_instYou;
};

PMError InstYou::retrievePatch( const PMYouPatchPtr &patch, bool reload,
                                bool checkSig, bool noExternal )
{
  D__ << "PATCH: " << patch->name() << endl;

  RpmDb rpm;

  list<PMPackagePtr> packages = patch->packages();
  list<PMYouFile> files = patch->files();
  int progressTotal = ( packages.size() + files.size() ) * 100 * 100/98;
  int progressCurrent = 0;

  CurlCallbacks callbacks( this, progressTotal );

  MediaCurl::setCallbacks( &callbacks );

  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    callbacks.setBaseProgress( progressCurrent );
    PMError error = patchProgress( progressCurrent * 100 / progressTotal );
    if ( error ) {
      MediaCurl::setCallbacks( 0 );
      return error;
    }
    progressCurrent += 100;

    if ( patch->updateOnlyInstalled() ) {
      if ( !(*itPkg)->hasInstalledObj() ) {
        D__ << "Don't download '" << (*itPkg)->name()
            << "', no installed obj and UpdateOnlyInstalled=true." << endl;
        continue;
      }
    }

    error = retrievePackage( *itPkg, reload, noExternal );
    if ( error ) {
      MediaCurl::setCallbacks( 0 );
      if ( error == MediaError::E_user_abort ) {
        return YouError::E_user_abort;
      } else {
        return error;
      }
    }
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
        MediaCurl::setCallbacks( 0 );
        ERR << "Signature check failed for " << localRpm << endl;
        return PMError( YouError::E_bad_sig_rpm );
      }
    }
  }

  list<PMYouFile>::const_iterator itFile;
  for( itFile = files.begin(); itFile != files.end(); ++itFile ) {
    callbacks.setBaseProgress( progressCurrent );
    PMError error = patchProgress( progressCurrent * 100 / progressTotal );
    if ( error ) {
      MediaCurl::setCallbacks( 0 );
      return error;
    }
    progressCurrent += 100;

    error = retrieveFile( *itFile, reload );
    if ( error ) {
      MediaCurl::setCallbacks( 0 );
      ERR << "Error retrieving file." << endl;
      return error;
    }
  }

  MediaCurl::setCallbacks( 0 );

  PMError error = patchProgress( 98 );
  if ( error ) return error;

  Pathname scriptPath;
  if ( !patch->preScript().empty() ) {
    PMError error = retrieveScript( patch->preScript(), reload, checkSig );
    if ( error ) {
      ERR << "Error retrieving preScript." << endl;
      return error;
    }
  }

  error = patchProgress( 99 );
  if ( error ) return error;

  if ( !patch->postScript().empty() ) {
    PMError error = retrieveScript( patch->postScript(), reload, checkSig );
    if ( error ) {
      ERR << "Error retrieving postScript." << endl;
      return error;
    }
  }

  error = patchProgress( 100 );
  if ( error ) return error;

  return PMError();
}

PMError InstYou::retrieveScript( const string &script, bool reload,
                                 bool checkSig )
{
  DBG << "Retrieve script '" << script << "'" << endl;

  Pathname scriptPath = _paths->scriptPath( script );

  PMError error = _media.provideFile( scriptPath, !reload );

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

  if ( !ok ) {
    if ( checkSig ) {
      ERR << "Signature check failed for script " << sourceScript << endl;
      return PMError( YouError::E_bad_sig_file, sourceScript );
    } else {
      WAR << "gpg call failed" << endl;
      PathInfo pi( destScript );
      if ( !pi.isExist() ) {
        ERR << "Missing signature." << endl;
        return PMError( YouError::E_missing_sig_file, sourceScript );
      }
    }
  }

  return PMError();
}

PMError InstYou::retrievePackage( const PMPackagePtr &pkg, bool reload,
                                  bool noExternal )
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

    Pathname path = url.path();
    url.setPath( "" );

    MediaAccess media;

    PMError err = media.open( url, _paths->externalRpmDir() );
    if ( err ) return err;

    if ( !noExternal ) {
      err = media.attach();
      if ( err ) return err;

      err = media.provideFile( path, !reload );
      if ( err ) return err;
    }

    _info->packageDataProvider()->setLocation( pkg, media.localPath( path ).asString() );

    return PMError();
  }

  list<PkgArch> archs = _paths->archs();

  if ( pkg->hasInstalledObj() ) {
    archs.push_front( pkg->getInstalledObj()->arch() );
  }

  bool patchRpm = hasPatchRpm( pkg );

  PMError error( YouError::E_error );
  Pathname rpmPath;
  list<PkgArch>::const_iterator it;
  for( it = archs.begin(); it != archs.end(); ++it ) {
    D__ << "ARCH: " << *it << endl;

    // If the package has a version installed, try to get patch RPM first.
    rpmPath = _paths->rpmPath( pkg, *it, patchRpm );
    D__ << "Trying downloading '" << _paths->patchUrl() << "/" << rpmPath
        << "'" << endl;
    error = _media.provideFile( rpmPath, !reload );
    if ( error ) {
      D__ << "Downloading RPM '" << pkg->name() << "' failed: " << error
          << endl;
      if ( error == MediaError::E_user_abort ) return error;

      // If patch RPM was requested first, try to get full RPM now.
      if ( patchRpm ) {
        rpmPath = _paths->rpmPath( pkg, *it, false );
        D__ << "Trying downloading '" << _paths->patchUrl() << "/" << rpmPath
            << "'" << endl;
        error = _media.provideFile( rpmPath, !reload );
        if ( error ) {
          D__ << "Download failed: " << error << endl;
          if ( error == MediaError::E_user_abort ) return error;
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

  string details = error.details();
  if ( !details.empty() ) details += "\n";
  details += pkg->nameEd();

  error.setDetails( details );

  return error;
}

PMError InstYou::retrieveFile( const PMYouFile &file, bool reload )
{
  DBG << "InstYou::retrieveFile: '" << file.name() << "'" << endl;

  Url url( file.name() );

  if ( !url.isValid() ) {
    ERR << "Invalid URL: '" << url.asString() << "'" << endl;
    return InstSrcError::E_bad_url;
  }

  int e = PathInfo::assert_dir( _paths->filesDir() );
  if ( e ) {
    ERR << "Can't create " << _paths->filesDir() << " (errno: "
        << e << ")" << endl;
    return PMError( InstSrcError::E_error );
  }

  Pathname path = url.path();
  url.setPath( "" );

  MediaAccess media;

  PMError err = media.open( url, _paths->filesDir() );
  if ( err ) return err;

  err = media.attach();
  if ( err ) return err;

  err = media.provideFile( path, !reload );
  if ( err ) return err;

  return PMError();
}

PMError InstYou::disconnect()
{
  return _media.disconnect();
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
             << " " << (*itPkg)->arch()
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
    D__ << "  PKG-CAND: " << (*itPkg)->name() << "-" << candEd << endl;
    if ( (*itPkg)->hasInstalledObj() ) {
      PMPackagePtr installed = (*itPkg)->getInstalledObj();
      PkgEdition instEd = installed->edition();
      D__ << "  PKG-INST: " << installed->name() << "-" << instEd << endl;
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

bool InstYou::hasPatchRpm( const PMPackagePtr &pkg )
{
  bool pkgHasInstalledObj = pkg->hasInstalledObj();

  if ( pkgHasInstalledObj ) {
    PkgEdition installedVersion = pkg->getInstalledObj()->edition();
    D__ << "Installed: " << installedVersion.asString() << endl;
    list<PkgEdition> baseVersions = pkg->patchRpmBaseVersions();
    list<PkgEdition>::const_iterator it2;
    for( it2 = baseVersions.begin(); it2 != baseVersions.end(); ++it2 ) {
      D__ << " Base version: " << (*it2).asString() << endl;
      if ( *it2 == installedVersion ) {
        D__ << "return true" << endl;
        return true;
      }
    }
  }

  D__ << "return false" << endl;
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

  if ( ret ) return PMError();
  else return YouError::E_user_abort;
}

PMError InstYou::patchProgress( int i, const string &pkg )
{
  bool ret = true;

  if ( _callbacks ) {
    ret = _callbacks->patchProgress( i, pkg );
  }

  if ( ret ) return PMError();
  else return YouError::E_user_abort;
}

int InstYou::lastUpdate()
{
  string date = _paths->config()->readEntry( "LastUpdate" );

  D__ << "LastUpdate: '" << date << "'" << endl;

  if ( date.empty() ) return -1;

  Date lastUpdate( date );
  DBG << "LastUpdate: " << lastUpdate << endl;

  int diff = Date::now() - lastUpdate;

  DBG << diff << endl;

  return diff / 60 / 60 / 24;
}

PMError InstYou::writeLastUpdate()
{
  MIL << "writeLastUpdate" << endl;

  _paths->config()->writeEntry( "LastUpdate", Date::toSECONDS( Date::now() ) );
  _paths->config()->save();

#warning Settings should probably written elsewhere
  // Settings can't be written in destructor, because writeSettings relies on
  // virtual methods.
  Y2PM::youPatchManager().writeSettings();

  return PMError();
}

int InstYou::quickCheckUpdates()
{
  PMYouServer server = _paths->patchServer();

  Url url = server.url();

  D__ << "InstYou::quickCheckUpdates(): " << url << endl;

  _info->processMediaDir();

  Pathname path = url.path();
  path += _paths->patchPath() + _paths->directoryFileName();
  url.setPath( path.asString() );

  Pathname dest = _paths->localWriteDir() + "quickcheck";

  PMError error = MediaAccess::getFile( url, dest );
  if ( error ) {
    ERR << "Quick check Updates: " << error << endl;
    return -1;
  }

  list<string> newPatchFiles;
  _info->readDirectoryFile( dest, newPatchFiles );

  Pathname dirFile = _paths->rootAttachPoint() + _paths->patchPath() +
                     _paths->directoryFileName();

  DBG << "Old directory file: " << dirFile << endl;

  list<string> oldPatchFiles;
  _info->readDirectoryFile( dirFile, oldPatchFiles );

  int patchCount = 0;
  list<string>::const_iterator newIt;
  for( newIt = newPatchFiles.begin(); newIt != newPatchFiles.end(); ++newIt ) {
    D__ << "newIt: " << *newIt << endl;
    list<string>::const_iterator oldIt;
    for( oldIt = oldPatchFiles.begin(); oldIt != oldPatchFiles.end(); ++oldIt ) {
      D__ << "oldIt: " << *oldIt << endl;
      if ( *newIt == *oldIt ) {
        break;
      }
    }
    if ( oldIt == oldPatchFiles.end() ) patchCount++;
  }

  return patchCount;
}

void InstYou::filterArchitectures( PMYouPatchPtr &patch )
{
  D__ << "filter " << patch->name() << endl;

  list<PMPackagePtr> packages = patch->packages();
  list<PMPackagePtr>::iterator it1 = packages.begin();
  while( it1 != packages.end() ) {
    D__ << "  PKG: " << (*it1)->nameEdArch() << endl;
  
    PMSelectablePtr s = Y2PM::packageManager().getItem( (*it1)->name() );

    // Handle installed packages. If the packages is already installed, all
    // instances of the packages in the patch which have a different
    // architecture are discarded.
    if ( s ) {
      PMPackagePtr installedPkg = s->installedObj();
      if ( installedPkg ) {
        D__ << "    Package is installed" << endl;
        if ( installedPkg->arch() == (*it1)->arch() ) {
          D__ << "    Same arch" << endl;
          ++it1;
        } else {
          D__ << "    Different arch" << endl;
          it1 = packages.erase( it1 );
        }
        continue;
      }
    }

    D__ << "    Package isn't installed." << endl;

    std::list<PkgArch> archs = _paths->archs();

    // Check if package instance has an allowed arch. If not, discard it.
    std::list<PkgArch>::const_iterator archIt;
    for( archIt = archs.begin(); archIt != archs.end(); ++archIt ) {
      if ( *archIt == (*it1)->arch() ) break;
    }
    if ( archIt == archs.end() ) {
      D__ << "    Discard package because of unknown arch." << endl;
      it1 = packages.erase( it1 );
      continue;
    }
    
    // Handle uninstalled packages. The instance of the package which has the
    // best architecture is used. All other instances are discarded.
    std::list<PMPackagePtr>::iterator bestArch = it1;
    list<PMPackagePtr>::iterator it2 = it1;
    ++it2;
    while( it2 != packages.end() ) {
      D__ << "      Iterating " << (*it2)->nameEdArch() << endl;
      if ( (*bestArch)->name() == (*it2)->name() ) {
        if ( (*bestArch)->arch() == (*it2)->arch() ) {
          ERR << "Two packages '" << (*bestArch)->name() << "' with same "
              << "architecture in the same patch is illegal." << endl;
          ++it2;
        } else {
          std::list<PkgArch>::const_iterator archIt;
          for( archIt = archs.begin(); archIt != archs.end(); ++archIt ) {
            if ( *archIt == (*bestArch)->arch() ) {
              D__ << "        already have best arch" << endl;
              it2 = packages.erase( it2 );
              break;
            } else if ( *archIt == (*it2)->arch() ) {
              D__ << "        better arch." << endl;
              if ( bestArch == it1 ) ++it1;
              packages.erase( bestArch );
              bestArch = it2;
              ++it2;
              break;
            }
          }
          if ( archIt == archs.end() ) {
            D__ << "        arch no allowed." << endl;
            if ( bestArch == it1 ) ++it1;
            packages.erase( bestArch );
            break;
          }
        }
      } else {
        ++it2;
      }
    }
  
    ++it1;
  }

  patch->setPackages( packages );
}
