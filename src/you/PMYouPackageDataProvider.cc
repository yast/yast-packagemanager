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
#include <y2pm/PMPackageManager.h>
#include <Y2PM.h>

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

void PMYouPackageDataProvider::setSrcLabel( const PMPackagePtr &pkg, const std::string &label )
{
  _srcLabels[ pkg ] = label;
}

string PMYouPackageDataProvider::instSrcLabel( const PMPackage & pkg ) const
{
  string label = "YOU";

  map<PMPackagePtr,string>::const_iterator it = _srcLabels.find( mkPtr( pkg ) );
  if ( it != _srcLabels.end() ) {
    label += ": " + it->second;
  }

  return label;
}

void PMYouPackageDataProvider::setSummary( const PMPackagePtr &pkg, const std::string &label )
{
  _summaries[ pkg ] = label;
}

string PMYouPackageDataProvider::summary( const PMPackage & pkg ) const
{
  map<PMPackagePtr,string>::const_iterator it = _summaries.find( mkPtr( pkg ) );
  if ( it == _summaries.end() ) return string();
  else return it->second;
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

string PMYouPackageDataProvider::externalUrl( const PMPackage &pkg ) const
{
  map<PMPackagePtr,string>::const_iterator it = _externalUrls.find( mkPtr( pkg ) );
  if ( it == _externalUrls.end() ) return "";
  else return it->second;
}

void PMYouPackageDataProvider::setExternalUrl( const PMPackagePtr &pkg, const string &str )
{
  _externalUrls[ pkg ] = str;
}

list<PkgEdition> PMYouPackageDataProvider::patchRpmBaseVersions( const PMPackage &pkg ) const
{
  map<PMPackagePtr,list<PkgEdition> >::const_iterator it = _patchRpmBaseVersions.find( mkPtr( pkg ) );
  if ( it == _patchRpmBaseVersions.end() ) return list<PkgEdition>();
  else return it->second;
}

void PMYouPackageDataProvider::setPatchRpmBaseVersions( const PMPackagePtr &pkg,
                                              const list<PkgEdition> &editions )
{
  _patchRpmBaseVersions[ pkg ] = editions;
}

std::string PMYouPackageDataProvider::group( const PMPackage & pkg_r ) const
{
  YStringTreeItem *item = group_ptr( pkg_r );
  if ( !item ) return string();
  else return Y2PM::packageManager().rpmGroup ( item );
}

YStringTreeItem *PMYouPackageDataProvider::group_ptr( const PMPackage & pkg_r ) const
{
  map<PMPackagePtr,YStringTreeItem *>::const_iterator it = _rpmGroups.find( mkPtr( pkg_r ) );
  if ( it == _rpmGroups.end() ) return 0;
  else return it->second;
}

void PMYouPackageDataProvider::setRpmGroup( const PMPackagePtr &pkg, const string &group )
{
  _rpmGroups[ pkg ] = Y2PM::packageManager().addRpmGroup( group );
}
