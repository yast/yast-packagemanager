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
PMSelectionDataProvider::summary(const std::string& lang) const
{
    MIL << "PMSelectionDataProvider::summary()" << endl;
    return "";
}

const std::list<std::string>
PMSelectionDataProvider::description(const std::string& lang) const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMSelectionDataProvider::insnotify(const std::string& lang) const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMSelectionDataProvider::delnotify(const std::string& lang) const
{
    return std::list<std::string>();
}

const std::string
PMSelectionDataProvider::category () const
{
    return "";
}

const bool
PMSelectionDataProvider::visible () const
{
    return true;
}

const std::list<std::string>
PMSelectionDataProvider::suggests() const
{
    return std::list<std::string>();
}

const std::list<PMSelectionPtr>
PMSelectionDataProvider::suggests_ptrs()
{
    return std::list<PMSelectionPtr>();
}

const std::list<std::string>
PMSelectionDataProvider::recommends() const
{
    return std::list<std::string>();
}

const std::list<PMSelectionPtr>
PMSelectionDataProvider::recommends_ptrs()
{
    return std::list<PMSelectionPtr>();
}

const std::list<std::string>
PMSelectionDataProvider::inspacks(const std::string& lang) const
{
    return std::list<std::string>();
}

const std::list<PMPackagePtr>
PMSelectionDataProvider::inspacks_ptrs(const std::string& lang)
{
    return std::list<PMPackagePtr>();
}

const std::list<std::string>
PMSelectionDataProvider::delpacks(const std::string& lang) const
{
    return std::list<std::string>();
}

const std::list<PMPackagePtr>
PMSelectionDataProvider::delpacks_ptrs(const std::string& lang)
{
    return std::list<PMPackagePtr>();
}

const FSize
PMSelectionDataProvider::archivesize() const
{
    return 0;
}

const std::string
PMSelectionDataProvider::order() const
{
    return "000";
}

const bool
PMSelectionDataProvider::isBase() const
{
    return false;
}
