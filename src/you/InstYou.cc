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

#include <y2pm/InstYou.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstYou
//
///////////////////////////////////////////////////////////////////

InstYou::InstYou()
{
#warning FIXME: get information about product/version/arch
  _paths = new PMYouPatchPaths();
#warning FIXME: get language information
  _info = new PMYouPatchInfo( "german" );

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
}

void InstYou::init()
{
  _selectedPatchesIt = _patches.begin();
}

PMError InstYou::servers( list<Url> &servers )
{
  PMError error = _paths->requestServers();
  
  if ( error ) {
    E__ << "Error fetching servers." << endl;
    return error;
  }
  
  servers = _paths->servers();
  
  return PMError();
}

PMError InstYou::checkAuthorization( const Url &url, const string &regcode,
                                     const string &password )
{
  return PMError();
}


PMError InstYou::retrievePatchInfo( const Url &url, bool checkSig )
{
  _patches.clear();

  _paths->setPatchUrl( url );

  PMError error = _info->getPatches( _paths, _patches, checkSig );
  if ( error ) {
    E__ << "Error downloading patchinfos: " << error << endl;
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
        I__ << "Patch has no selectable." << endl;
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
          I__ << "Patch has no selectable." << endl;
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

PMError InstYou::attachSource()
{
  int err = PathInfo::assert_dir( _paths->localDir() );
  if ( err ) {
    E__ << "Can't create " << _paths->localDir() << " (errno: " << err << ")"
        << endl;
    return PMError( InstSrcError::E_error );
  }

  PMError error = _media.open( _paths->patchUrl(), _paths->localDir() );
  if ( error ) {
    E__ << "Error opening URL '" << _paths->patchUrl() << "'" << endl;
    return error;
  }
  error = _media.attach();
  if ( error ) {
    E__ << "Error attaching media." << endl;
  }

  return error;
}

PMError InstYou::retrievePatches( bool checkSig )
{
  D__ << "Retrieve patches." << endl;

  PMError error = attachSource();
  if ( error ) return error;

  PMYouPatchPtr patch;
  for( patch = firstPatch(); patch; patch = nextPatch() ) {
    error = retrievePatch( patch, checkSig );
    if ( error ) return error;
  }

  return error;
}

PMYouPatchPtr InstYou::firstPatch()
{
  _selectedPatchesIt = _patches.begin();

  return nextSelectedPatch();
}

PMYouPatchPtr InstYou::nextPatch()
{
  ++_selectedPatchesIt;

  return nextSelectedPatch();
}

PMYouPatchPtr InstYou::nextSelectedPatch()
{
  while ( _selectedPatchesIt != _patches.end() ) {
    PMSelectablePtr selectable = (*_selectedPatchesIt)->getSelectable();
    if ( selectable && selectable->to_install() ) {
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

  PMError error;

  Pathname scriptPath;
  if ( !patch->preScript().empty() ) {
    scriptPath = _media.localPath( _paths->scriptPath( patch->preScript() ) );
    if ( dryrun ) {
      cout << "PRESCRIPT: " << scriptPath << endl;
    } else {
      Y2PM::instTarget().executeScript( scriptPath );
    }
  }

  list<PMPackagePtr> packages = patch->packages();
  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    Pathname fileName = _media.localPath( (*itPkg)->location() );
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

  if ( !dryrun ) {
    error = Y2PM::instTarget().installPatch( patch->localFile() );
    if ( error ) {
      E__ << "Error installing patch info." << endl;
      return error;
    }
  }

  if ( !patch->postScript().empty() ) {
    scriptPath = _media.localPath( _paths->scriptPath( patch->postScript() ) );
    if ( dryrun ) {
      cout << "POSTSCRIPT: " << scriptPath << endl;
    } else {
      Y2PM::instTarget().executeScript( scriptPath );
    }
  }
  
  return error;
}

PMError InstYou::retrievePatch( const PMYouPatchPtr &patch, bool checkSig )
{
  D__ << "PATCH: " << patch->name() << endl;

  RpmDb rpm;

  list<PMPackagePtr> packages = patch->packages();
  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    PMError error = retrievePackage( *itPkg );
    if ( error ) return error;
    if ( checkSig ) {
      string localRpm = _media.localPath( (*itPkg)->location() ).asString();
      unsigned result = rpm.checkPackage( localRpm );
      if ( result != 0 ) {
        E__ << "Signature check failed for " << localRpm << endl;
        return PMError( YouError::E_bad_sig_rpm );
      }
    }
  }

  GPGCheck gpg;

  Pathname scriptPath;
  if ( !patch->preScript().empty() ) {
    scriptPath = _paths->scriptPath( patch->preScript() );
    PMError error = _media.provideFile( scriptPath );
    if ( error ) {
      E__ << "Error downloading pre script from '"
          << _paths->patchUrl() << "/" << scriptPath << "'" << endl;
      return error;
    }
    if ( checkSig ) {
      string localScript = _media.localPath( scriptPath ).asString();
      if ( !gpg.check_file( localScript ) ) {
        E__ << "Signature check failed for script " << localScript << endl;
        return PMError( YouError::E_bad_sig_file );
      }
    }
  }
  
  if ( !patch->postScript().empty() ) {
    scriptPath = _paths->scriptPath( patch->postScript() );
    PMError error = _media.provideFile( scriptPath );
    if ( error ) {
      E__ << "Error downloading post script from '"
          << _paths->patchUrl() << "/" << scriptPath << "'" << endl;
      return error;
    }
    if ( checkSig ) {
      string localScript = _media.localPath( scriptPath ).asString();
      if ( !gpg.check_file( localScript ) ) {
        E__ << "Signature check failed for script " << localScript << endl;
        return PMError( YouError::E_bad_sig_file );
      }
    }
  }
  
  return PMError();
}

PMError InstYou::retrievePackage( const PMPackagePtr &pkg )
{
  list<string> archs;

  if ( pkg->hasInstalledObj() ) {
    archs.push_back( pkg->getInstalledObj()->arch() );
  }
  
  archs.push_back( _paths->baseArch() );
  archs.push_back( "noarch" );

  PMError error;
  Pathname rpmPath;
  list<string>::const_iterator it;
  for( it = archs.begin(); it != archs.end(); ++it ) {
    D__ << *it << endl;
    
    // If the package has a version installed, try to get patch RPM first.
    bool pkgHasInstalledObj = pkg->hasInstalledObj();
    rpmPath = _paths->rpmPath( pkg, *it, pkgHasInstalledObj );
    D__ << "Trying downloading '" << _paths->patchUrl() << "/" << rpmPath
        << "'" << endl;
    error = _media.provideFile( rpmPath );
    if ( error ) {
      D__ << "Downloading RPM '" << pkg->name() << "' failed: " << error
          << endl;
          
      // If patch RPM was requested first, try to get full RPM now.
      if ( pkgHasInstalledObj ) {
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
      _info->setLocation( pkg, rpmPath.asString() );
      D__ << "RPM: " << pkg->name() << ": " << pkg->location() << endl;
      return PMError();
    }
  }

  E__ << "Error downloading RPM " << pkg->name() << endl;
  return error;
}

PMError InstYou::removePackages()
{
  PMYouPatchPtr patch;
  for( patch = firstPatch(); patch; patch = nextPatch() ) {
    list<PMPackagePtr> packages = patch->packages();
    list<PMPackagePtr>::const_iterator itPkg;
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
      PMError error = _media.releaseFile( (*itPkg)->location() );
      if ( error ) {
        E__ << "Can't release " << (*itPkg)->location() << endl;
        return error;
      }
    }
    if ( !patch->preScript().empty() ) {
      Pathname scriptPath = _paths->scriptPath( patch->preScript() );
      PMError error = _media.releaseFile( scriptPath );
      if ( error ) {
        E__ << "Can't release " << scriptPath.asString() << endl;
        return error;
      }
    }
    if ( !patch->postScript().empty() ) {
      Pathname scriptPath = _paths->scriptPath( patch->postScript() );
      PMError error = _media.releaseFile( scriptPath );
      if ( error ) {
        E__ << "Can't release " << scriptPath.asString() << endl;
        return error;
      }
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
