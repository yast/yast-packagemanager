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

   File:       PMSelectionDataProvider.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

  Purpose: Common interface to be realized by all SelectionDataProvider.

/-*/

#include <y2util/Y2SLog.h>
#include <iostream>

#include <y2pm/PMSelectionDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectionDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMSelectionDataProvider);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionDataProvider::PMSelectionDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMSelectionDataProvider::PMSelectionDataProvider()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMSelectionDataProvider::~PMSelectionDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMSelectionDataProvider::~PMSelectionDataProvider()
{
}
