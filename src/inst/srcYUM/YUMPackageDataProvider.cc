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

   File:       YUMPackageDataProvider.cc
   Purpose:    Implements PackageDataProvider for YUM
   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include "YUMPackageDataProvider.h"

using namespace std;

///////////////////////////////////////////////////////////////////
namespace YUM
{ /////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  //	CLASS NAME : PackageDataProviderPtr
  //	CLASS NAME : constPackageDataProviderPtr
  ///////////////////////////////////////////////////////////////////
  IMPL_DERIVED_POINTER(PackageDataProvider, PMPackageDataProvider );

  ///////////////////////////////////////////////////////////////////
  //
  //
  //	METHOD NAME : PackageDataProvider::PackageDataProvider
  //	METHOD TYPE : Constructor
  //
  PackageDataProvider::PackageDataProvider( const VarPtr<Impl> impl_r )
  : _impl( impl_r )
  , _attr_GROUP( NULL )
  {
  }

  ///////////////////////////////////////////////////////////////////
  //
  //
  //	METHOD NAME : PackageDataProvider::~PackageDataProvider
  //	METHOD TYPE : Destructor
  //
  PackageDataProvider::~PackageDataProvider()
  {
  }

  /////////////////////////////////////////////////////////////////
} // namespace YUM
///////////////////////////////////////////////////////////////////

