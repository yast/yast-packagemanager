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

#include <y2pm/PMPackageDataProviderUL.h>
#include <y2pm/PMPackageDataProviderULPtr.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProviderUL
//
///////////////////////////////////////////////////////////////////

IMPL_DERIVED_POINTER(PMPackageDataProviderUL, PMPackageDataProvider, PMPackageDataProvider );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageDataProviderUL::PMPackageDataProviderUL
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMPackageDataProviderUL::PMPackageDataProviderUL()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageDataProviderUL::~PMPackageDataProviderUL
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMPackageDataProviderUL::~PMPackageDataProviderUL()
{
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageDataProviderUL::~PMPackageDataProviderUL
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :Package attribute retrieval.
//
PkgAttributeValue
PMPackageDataProviderUL::getAttributeValue( constPMPackagePtr pkg_r,
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
//	METHOD NAME : PMPackageDataProviderUL::setAttributeValue
//	METHOD TYPE : attribute set
//
//	DESCRIPTION : inject some SINGLE object attribute by value
//
void
PMPackageDataProviderUL::setAttributeValue(
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
//	METHOD NAME : PMPackageDataProviderUL::setAttributeValue
//	METHOD TYPE : attribute set
//
//	DESCRIPTION :inject some package attribute by file offset
//
void
PMPackageDataProviderUL::setAttributeValue(
	PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	std::streampos pos, int size)
{
    attrpos[attr].pos = pos;
    attrpos[attr].size = size;
    return;
}

