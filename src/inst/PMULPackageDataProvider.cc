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

  File:       PMULPackageDataProvider.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Realize PackageDataProvider for UnitedLinux packages format

/-*/

#include <iostream>

#include <y2pm/PMULPackageDataProvider.h>
#include <y2pm/PMULPackageDataProviderPtr.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMULPackageDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER(PMULPackageDataProvider, PMPackageDataProvider, PMPackageDataProvider );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::PMULPackageDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMULPackageDataProvider::PMULPackageDataProvider()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::~PMULPackageDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMULPackageDataProvider::~PMULPackageDataProvider()
{
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::~PMULPackageDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :Package attribute retrieval.
//
PkgAttributeValue
PMULPackageDataProvider::getAttributeValue( constPMPackagePtr pkg_r,
					 PMPackage::PMPackageAttribute attr_r )
{
    if (attrpos[attr_r].size < 0)
	return attrval[attr_r];
    /*
	FIXME

	re-read value from cache file
    */

    return PkgAttributeValue("**undef**");
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::setAttributeValue
//	METHOD TYPE : attribute set
//
//	DESCRIPTION : inject some SINGLE object attribute by value
//
void
PMULPackageDataProvider::setAttributeValue(
	PMPackagePtr pkg, PMObject::PMObjectAttribute attr,
	const PkgAttributeValue value)
{
    attrpos[attr].size = -1;
    attrval[attr] = PkgAttributeValue (value);
    return;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::setAttributeValue
//	METHOD TYPE : attribute set
//
//	DESCRIPTION :inject some package attribute by file offset
//
void
PMULPackageDataProvider::setAttributeValue(
	PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	std::streampos pos, int size)
{
    attrpos[attr].pos = pos;
    attrpos[attr].size = size;
    return;
}

