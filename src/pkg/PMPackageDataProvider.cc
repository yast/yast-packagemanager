/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:       PMPackageDataProvider.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Common interface to be realized by all PackageDataProvider.

/-*/

#include <iostream>

#include <y2pm/PMPackageDataProvider.h>
#include <y2pm/PMPackage.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMPackageDataProvider);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageDataProvider::PMPackageDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMPackageDataProvider::PMPackageDataProvider()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageDataProvider::~PMPackageDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMPackageDataProvider::~PMPackageDataProvider()
{
}
