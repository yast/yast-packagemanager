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

#include <y2util/Y2SLog.h>

#include <y2pm/PMULPackageDataProvider.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMULPackageDataProviderPtr
//	CLASS NAME : constPMULPackageDataProviderPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(PMULPackageDataProvider,PMPackageDataProvider,PMDataProvider);

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
//	METHOD NAME : PMULPackageDataProvider::getAttributeValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION : Object attribute retrieval.
//
PkgAttributeValue
PMULPackageDataProvider::getAttributeValue( constPMObjectPtr obj_r,
					PMObject::PMObjectAttribute attr )
{
    D__ << "PMULPackageDataProvider::getAttributeValue (" << obj_r->getAttributeName(attr) << ")" << endl;
    if (attr >= PMObject::PMOBJ_NUM_ATTRIBUTES)
	return PkgAttributeValue("invalid query");

    if (attrpos[attr].size < 0)
	return attrval[attr];
    /*
	FIXME

	re-read value from cache file
    */

    return PkgAttributeValue("**undef**");
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::getAttributeValue
//	METHOD TYPE : PkgAttributeValue
//
//	DESCRIPTION :Package attribute retrieval.
//
PkgAttributeValue
PMULPackageDataProvider::getAttributeValue( constPMPackagePtr pkg_r,
					PMPackage::PMPackageAttribute attr )
{
    D__ << "PMULPackageDataProvider::getAttributeValue (" << pkg_r->getAttributeName(attr) << ")" << endl;
    if (attr >= PMPackage::PKG_NUM_ATTRIBUTES)
	return PkgAttributeValue("invalid query");

    if (attrpos[attr].size < 0)
	return attrval[attr];
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
//	DESCRIPTION : inject some object attribute by value
//
void
PMULPackageDataProvider::setAttributeValue(
	PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	const PkgAttributeValue& value)
{
    D__ << "PMULPackageDataProvider::setAttributeValue (" << pkg->getAttributeName(attr) << ")" << endl;
    if (attr < PMPackage::PKG_NUM_ATTRIBUTES)
    {
	attrpos[attr].size = -1;
	attrval[attr] = PkgAttributeValue (value);
    }
    else
    {
	ERR << "PMULPackageDataProvider::setAttributeValue(" << attr << ", " << value << ")" << endl;
    }
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMULPackageDataProvider::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
PMULPackageDataProvider::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}


