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

/-*/

#include <iostream>

#include <y2pm/PMPackageDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER(PMPackageDataProvider, PMDataProvider, PMDataProvider );

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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageDataProvider::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMPackageDataProvider::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

