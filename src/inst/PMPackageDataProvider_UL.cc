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

  Purpose: Realize PackageDataProvider for UnitedLinux packages format

/-*/

#include <iostream>

#include <y2pm/PMPackageDataProvider_UL.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProvider_UL
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER(PMPackageDataProvider_UL, PMPackageDataProvider, PMPackageDataProvider );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageDataProvider_UL::PMPackageDataProvider_UL
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMPackageDataProvider_UL::PMPackageDataProvider_UL()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageDataProvider_UL::~PMPackageDataProvider_UL
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMPackageDataProvider_UL::~PMPackageDataProvider_UL()
{
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageDataProvider_UL::~PMPackageDataProvider_UL
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :Package attribute retrieval.
//
PkgAttributeValue
PMPackageDataProvider_UL::getAttributeValue( constPMPackagePtr pkg_r,
					 PMPackage::PMPackageAttribute attr_r )
{
    return PkgAttributeValue("attribute");
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageDataProvider_UL::~PMPackageDataProvider_UL
//	METHOD TYPE : Destructor
//
//	DESCRIPTION : inject some object attribute by value
//
void
PMPackageDataProvider_UL::setAttributeValue(
	PMPackagePtr pkg, PMObject::PMObjectAttribute attr,
	const std::string& value)
{
    return;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageDataProvider_UL::~PMPackageDataProvider_UL
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :inject some package attribute by file offset
//
void
PMPackageDataProvider_UL::setAttributeValue(
	PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	std::streampos pos, int size)
{
    return;
}

