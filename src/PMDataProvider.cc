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

  File:       PMDataProvider.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Common interface to be realized by all DataProvider.

/-*/

#include <y2pm/PMDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMDataProvider);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMDataProvider::PMDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMDataProvider::PMDataProvider()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMDataProvider::~PMDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMDataProvider::~PMDataProvider()
{
}


const std::string
PMDataProvider::summary () const
{
    return "";
}

const std::list<std::string>
PMDataProvider::description () const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMDataProvider::insnotify () const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMDataProvider::delnotify () const
{
    return std::list<std::string>();
}

const FSize
PMDataProvider::size () const
{
    return FSize(0);
}
