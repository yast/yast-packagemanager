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

#include <iostream>

#include <y2pm/PMSelectionDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectionDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER(PMSelectionDataProvider, PMDataProvider, PMDataProvider );

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


const std::string
PMSelectionDataProvider::summary() const
{
    return "";
}

const std::list<std::string>
PMSelectionDataProvider::description() const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMSelectionDataProvider::insnotify() const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMSelectionDataProvider::delnotify() const
{
    return std::list<std::string>();
}

