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

std::string PMYouPackageDataProvider::location( const PMPackage & pkg_r ) const {
  return _patchInfo->location( mkPtr(pkg_r) );
}

FSize PMYouPackageDataProvider::size( const PMPackage & pkg_r ) const {
  return _patchInfo->size( mkPtr(pkg_r) );
}
