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

  File:       PMYouPatchDataProvider.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Common interface to be realized by all YouPatchDataProvider.

/-*/

#include <iostream>

#include <y2pm/PMYouPatchDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER(PMYouPatchDataProvider, PMDataProvider, PMDataProvider );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchDataProvider::PMYouPatchDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMYouPatchDataProvider::PMYouPatchDataProvider()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMYouPatchDataProvider::~PMYouPatchDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMYouPatchDataProvider::~PMYouPatchDataProvider()
{
}
