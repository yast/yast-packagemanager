/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       PMYouPackageDataProvider.cc
   Purpose:    Implements PackageDataProvider for YOU patches.
   Author:     Cornelius Schumacher <cschum@suse.de>
   Maintainer: Cornelius Schumacher <cschum@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/PMYouPackageDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPackageDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER(PMYouPackageDataProvider, PMPackageDataProvider, PMPackageDataProvider );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPackageDataProvider::PMYouPackageDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMYouPackageDataProvider::PMYouPackageDataProvider( const PMYouPatchInfoPtr &info )
  : _patchInfo( info )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPackageDataProvider::~PMYouPackageDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMYouPackageDataProvider::~PMYouPackageDataProvider()
{
}

FSize PMYouPackageDataProvider::size( const PMPackage &pkg ) const
{
  map<PMPackagePtr,FSize>::const_iterator it = _sizes.find( mkPtr( pkg ) );
  if ( it == _sizes.end() ) return FSize( 0 );
  else return it->second;
}

void PMYouPackageDataProvider::setSize( const PMPackagePtr &pkg, const FSize &size )
{
  _sizes[ pkg ] = size;
}

string PMYouPackageDataProvider::location( const PMPackage &pkg ) const
{
  map<PMPackagePtr,string>::const_iterator it = _locations.find( mkPtr( pkg ) );
  if ( it == _locations.end() ) return "";
  else return it->second;
}

void PMYouPackageDataProvider::setLocation( const PMPackagePtr &pkg, const string &str )
{
  _locations[ pkg ] = str;
}

const string PMYouPackageDataProvider::externalUrl( const PMPackagePtr &pkg ) const
{
  map<PMPackagePtr,string>::const_iterator it = _externalUrls.find( pkg );
  if ( it == _externalUrls.end() ) return "";
  else return it->second;
}

void PMYouPackageDataProvider::setExternalUrl( const PMPackagePtr &pkg, const string &str )
{
  _externalUrls[ pkg ] = str;
}

const list<PkgEdition> PMYouPackageDataProvider::patchRpmBaseVersions( const PMPackagePtr &pkg ) const
{
  map<PMPackagePtr,list<PkgEdition> >::const_iterator it = _patchRpmBaseVersions.find( pkg );
  if ( it == _patchRpmBaseVersions.end() ) return list<PkgEdition>();
  else return it->second;
}

void PMYouPackageDataProvider::setPatchRpmBaseVersions( const PMPackagePtr &pkg,
                                              const list<PkgEdition> &editions )
{
  _patchRpmBaseVersions[ pkg ] = editions;
}
