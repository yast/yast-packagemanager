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

#include <Y2PM.h>

#include <y2pm/PMYouPatch.h>
#include <y2pm/PMPackage.h>

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
  _selectedPatchesIt = _selectedPatches.begin();
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

PMError InstYou::retrievePatchInfo( const Url &url )
{
  _patches.clear();

  _paths->setPatchUrl( url );

  PMError error = _info->getPatches( _paths, _patches );
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
  _selectedPatches.clear();

  bool yastPatch = false;

  list<PMYouPatchPtr>::const_iterator itPatch;
  for( itPatch = _patches.begin(); itPatch != _patches.end(); ++itPatch ) {
    if ( (*itPatch)->kind() == PMYouPatch::kind_yast ) {
      _selectedPatches.push_back( *itPatch );
      yastPatch = true;
    }
  }

  if ( !yastPatch ) {
    for( itPatch = _patches.begin(); itPatch != _patches.end(); ++itPatch ) {
      int kind = (*itPatch)->kind();
      if ( kind & kinds ) {
        _selectedPatches.push_back( *itPatch );
      }
    }
  }

  _selectedPatchesIt = _selectedPatches.begin();
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

PMError InstYou::retrievePatches()
{
  PMError error = attachSource();
  if ( error ) return error;

  list<PMYouPatchPtr>::const_iterator itPatch;
  for( itPatch = _selectedPatches.begin(); itPatch != _selectedPatches.end();
       ++itPatch ) {
    PMError error = retrievePatch( *itPatch );
    if ( error ) return error;
  }

  return PMError();
}

PMYouPatchPtr InstYou::firstPatch()
{
  _selectedPatchesIt = _selectedPatches.begin();

  if ( _selectedPatchesIt == _selectedPatches.end() ) {
    return PMYouPatchPtr();
  }

  return *_selectedPatchesIt;
}

PMYouPatchPtr InstYou::nextPatch()
{
  ++_selectedPatchesIt;

  if ( _selectedPatchesIt == _selectedPatches.end() ) {
    return PMYouPatchPtr();
  }

  return *_selectedPatchesIt;
}

PMError InstYou::installCurrentPatch()
{
  D__ << "Install current Patch" << endl;

  if ( _selectedPatchesIt == _selectedPatches.end() ) {
    E__ << "No more patches." << endl;
    return PMError( InstSrcError::E_error );
  }

  return installPatch( *_selectedPatchesIt );
}

PMError InstYou::retrieveCurrentPatch()
{
  if ( _selectedPatchesIt == _selectedPatches.end() ) {
    E__ << "No more patches." << endl;
    return PMError( InstSrcError::E_error );
  }

  return retrievePatch( *_selectedPatchesIt );
}

PMError InstYou::installPatches( bool dryrun )
{
  list<PMYouPatchPtr>::const_iterator itPatch;
  for( itPatch = _selectedPatches.begin(); itPatch != _selectedPatches.end();
       ++itPatch ) {
    PMError error = installPatch( *itPatch, dryrun );
    if ( error ) return error;
  }

  return PMError();
}

PMError InstYou::installPatch( const PMYouPatchPtr &patch, bool dryrun )
{
  D__ << "INSTALL PATCH: " << patch->name() << endl;
    
  list<string> packageFileNames;

  list<PMPackagePtr> packages = patch->packages();
  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    Pathname fileName = _media.localPath( _paths->rpmPath( *itPkg ) );
    packageFileNames.push_back( fileName.asString() );
    D__ << "INSTALL PKG " << fileName << endl;      
    if ( dryrun ) {
      cout << "INSTALL: " << fileName << endl;
    }
  }

  if ( !dryrun ) {
    PMError error = Y2PM::instTarget().installPackages( packageFileNames );
    if ( error ) {
      E__ << "Installation of RPMs of patch " << patch->name()
          << "failed" << endl;
      return error;
    }
    
    error = Y2PM::instTarget().installPatch( patch->localFile() );
    if ( error ) {
      E__ << "Error installing patch info." << endl;
      return error;
    }
  }
  
  return PMError();
}

PMError InstYou::retrievePatch( const PMYouPatchPtr &patch )
{
//  D__ << "PATCH: " << (*itPatch)->name() << endl;
  list<PMPackagePtr> packages = patch->packages();
  list<PMPackagePtr>::const_iterator itPkg;
  for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
    Pathname rpmPath = _paths->rpmPath( *itPkg );
//    D__ << "  RPM: " << (*itPkg)->name() << ": " << rpmPath.asString() << endl;
    PMError error = _media.provideFile( rpmPath );
    if ( error ) {
      E__ << "Error downloading RPM '" << (*itPkg)->name() << "' from '"
          << _paths->patchUrl() << "/" << rpmPath << "'" << endl;
      return error;
    }
  }
  
  return PMError();
}

void InstYou::filterPatchSelection()
{
  list<PMYouPatchPtr> filteredPatches;

  list<PMYouPatchPtr>::const_iterator itPatch;
  for( itPatch = _selectedPatches.begin(); itPatch != _selectedPatches.end();
       ++itPatch ) {
    D__ << "PATCH: " << (*itPatch)->name() << endl;

    list<PMPackagePtr> packages = (*itPatch)->packages();
    list<PMPackagePtr>::const_iterator itPkg;

    bool install = false;
    
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
      D__ << "  PKG: " << (*itPkg)->name() << "-" << (*itPkg)->edition() << endl;
      if ( (*itPkg)->hasInstalledObj() ) {
        if ( (*itPkg)->getInstalledObj()->edition() >= (*itPkg)->edition() ) {
          D__ << "    is older than installed" << endl;
          install = false;
          break;
        } else {
          D__ << "    is newer than installed" << endl;
          install = true;
        }
      } else {
        D__ << "    not installed" << endl;
      }
    }
    
    if ( install ) filteredPatches.push_back( *itPatch );
  }

  _selectedPatches = filteredPatches;

  for( itPatch = _selectedPatches.begin(); itPatch != _selectedPatches.end();
       ++itPatch ) {
    cerr << "Install: " << (*itPatch)->name() << endl;
  }

  _selectedPatchesIt = _selectedPatches.begin();
}

PMError InstYou::removePackages()
{
  list<PMYouPatchPtr>::const_iterator itPatch;
  for( itPatch = _selectedPatches.begin(); itPatch != _selectedPatches.end();
       ++itPatch ) {
    list<PMPackagePtr> packages = (*itPatch)->packages();
    list<PMPackagePtr>::const_iterator itPkg;
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
      PMError error = _media.releaseFile( _paths->rpmPath( *itPkg ) );
      if ( error ) {
        E__ << "Can't release " << _paths->rpmPath( *itPkg ).asString() << endl;
        return error;
      }
    }
  }

  return PMError();
}
