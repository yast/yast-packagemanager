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
  _paths = new PMYouPatchPaths("Dummy","1.0","i386");
#warning FIXME: get language information
  _info = new PMYouPatchInfo( "german" );
}

InstYou::InstYou( const PMYouPatchInfoPtr &info,
                  const PMYouPatchPathsPtr &paths )
{
  _info = info;
  _paths = paths;
}

InstYou::~InstYou()
{
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

PMError InstYou::retrievePatches( const Url &url )
{
  _patches.clear();

  _paths->setPatchUrl( url );

  PMError error = _info->getPatches( _paths, _patches );
  if ( error ) {
    E__ << "Error downloading patchinfos: " << error << endl;
    return error;
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
}

PMError InstYou::retrievePackages()
{
  PMError error = _media.open( _paths->patchUrl(), _paths->localDir() );
  if ( error ) {
    E__ << "Error opening URL '" << _paths->patchUrl() << "'" << endl;
    return error;
  }
  error = _media.attach();
  if ( error ) {
    E__ << "Error attaching media." << endl;
    return error;
  }

  list<PMYouPatchPtr>::const_iterator itPatch;
  for( itPatch = _selectedPatches.begin(); itPatch != _selectedPatches.end();
       ++itPatch ) {
//    D__ << "PATCH: " << (*itPatch)->name() << endl;
    list<PMPackagePtr> packages = (*itPatch)->packages();
    list<PMPackagePtr>::const_iterator itPkg;
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
      Pathname rpmPath = _paths->rpmPath( *itPkg );
//      D__ << "  RPM: " << (*itPkg)->name() << ": " << rpmPath.asString() << endl;
      error = _media.provideFile( rpmPath );
      if ( error ) {
        E__ << "Error downloading RPM '" << (*itPkg)->name() << "' from '"
            << _paths->patchUrl() << "/" << rpmPath << "'" << endl;
        return error;
      }
    }
  }

  return PMError();
}

PMError InstYou::installPatches()
{
  list<PMYouPatchPtr>::const_iterator itPatch;
  for( itPatch = _selectedPatches.begin(); itPatch != _selectedPatches.end();
       ++itPatch ) {
    D__ << "INSTALL: " << (*itPatch)->name() << endl;
    list<PMPackagePtr> packages = (*itPatch)->packages();
    list<PMPackagePtr>::const_iterator itPkg;
    for ( itPkg = packages.begin(); itPkg != packages.end(); ++itPkg ) {
      D__ << "  rpm -i --force --nodeps "
          << _media.localPath( _paths->rpmPath( *itPkg ) ) << endl;
    }
  }

  return PMError();
}
