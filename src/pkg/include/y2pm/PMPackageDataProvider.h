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

  File:       PMPackageDataProvider.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Common interface to be realized by all PackageDataProvider.

/-*/
#ifndef PMPackageDataProvider_h
#define PMPackageDataProvider_h

#include <iosfwd>
#include <string>

#include <y2pm/PMPackageDataProviderPtr.h>
#include <y2pm/PMDataProvider.h>

#include <y2pm/PMPackage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProvider
/**
 * @short Common interface to be realized by all PackageDataProvider.
 * @see DataProvider
 **/
class PMPackageDataProvider : virtual public Rep, public PMDataProvider  {
  REP_BODY(PMPackageDataProvider);

  protected:

    PMPackageDataProvider();

    virtual ~PMPackageDataProvider();

  public:

    /**
     * Package attribute retrieval.
     * @see PMPackage
     **/
    virtual PkgAttributeValue getAttributeValue( constPMPackagePtr pkg_r,
						 PMPackage::PMPackageAttribute attr_r ) = 0;

#if 0
  public: // most probabely this should not be here

    /** inject some object attribute. The actual implementation may or may not
     * choose to store the value in a cache for faster access.
     * */
    virtual void setAttributeValue(
	PMPackagePtr pkg, PMObject::PMObjectAttribute attr,
	const std::string& value) = 0;

    /** inject some package attribute. The actual implementation may or may not
     * choose to store the value in a cache for faster access.
     * */
    virtual void setAttributeValue(
	PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	const std::string& value) = 0;
#endif
};

///////////////////////////////////////////////////////////////////

#endif // PMPackageDataProvider_h

