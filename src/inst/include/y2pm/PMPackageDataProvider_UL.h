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

  File:       PMPackageDataProvider_UL.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Realized PackageDataProvider for UnitedLinux packages format

/-*/
#ifndef PMPackageDataProvider_UL_h
#define PMPackageDataProvider_UL_h

#include <iosfwd>
#include <string>

#include <y2pm/PMPackageDataProviderPtr.h>
#include <y2pm/PMDataProvider.h>

#include <y2pm/PMPackage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProvider_UL
/**
 * @short Realizes PackageDataProvider for UnitedLinux packages format
 * @see DataProvider
 **/
class PMPackageDataProvider_UL : public PMPackageDataProvider  {
  REP_BODY(PMPackageDataProvider_UL);

  protected:

    PMPackageDataProvider_UL();

    ~PMPackageDataProvider_UL();

  public:

    /**
     * Package attribute retrieval.
     * @see PMPackage
     **/
    PkgAttributeValue getAttributeValue( constPMPackagePtr pkg_r,
					 PMPackage::PMPackageAttribute attr_r );

  public:

    /** inject some object attribute by value
     * */
    void setAttributeValue(
	PMPackagePtr pkg, PMObject::PMObjectAttribute attr,
	const std::string& value);

    /** inject some package attribute by file offset
     * */
    void setAttributeValue(
	PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	std::streampos pos, int size);
};

///////////////////////////////////////////////////////////////////

#endif // PMPackageDataProvider_UL_h
