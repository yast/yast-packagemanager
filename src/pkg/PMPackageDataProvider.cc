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

const Date
PMPackageDataProvider::buildtime () const
{
    return 0;
}

const std::string
PMPackageDataProvider::buildhost () const
{
    return "";
}

const Date
PMPackageDataProvider::installtime () const
{
    return 0;
}

const std::string
PMPackageDataProvider::distribution () const
{
    return "";
}

const std::string
PMPackageDataProvider::vendor () const
{
    return "";
}

const std::string
PMPackageDataProvider::license () const
{
    return "";
}

const std::string
PMPackageDataProvider::packager () const
{
    return "";
}

const std::string
PMPackageDataProvider::group () const
{
    return "";
}

const std::list<std::string>
PMPackageDataProvider::changelog () const
{
    return std::list<std::string>();
}

const std::string
PMPackageDataProvider::url () const
{
    return "";
}

const std::string
PMPackageDataProvider::os () const
{
    return "";
}

const std::list<std::string>
PMPackageDataProvider::prein () const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMPackageDataProvider::postin () const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMPackageDataProvider::preun () const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMPackageDataProvider::postun () const
{
    return std::list<std::string>();
}

const std::string
PMPackageDataProvider::sourcerpm () const
{
    return "";
}

const FSize
PMPackageDataProvider::archivesize () const
{
  return 0;
}

const std::list<std::string>
PMPackageDataProvider::authors () const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMPackageDataProvider::filenames () const
{
    return std::list<std::string>();
}

// suse packages values
const std::list<std::string>
PMPackageDataProvider::recommends () const
{
    return std::list<std::string>();
}

const std::list<std::string>
PMPackageDataProvider::suggests () const
{
    return std::list<std::string>();
}

const std::string
PMPackageDataProvider::location () const
{
    return "";
}

const int
PMPackageDataProvider::medianr () const
{
    return 1;
}

const std::list<std::string>
PMPackageDataProvider::keywords () const
{
    return std::list<std::string>();
}

