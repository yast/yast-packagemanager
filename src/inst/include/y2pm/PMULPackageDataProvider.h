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

  File:       PMULPackageDataProvider.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Realized PackageDataProvider for UnitedLinux packages format

/-*/
#ifndef PMULPackageDataProvider_h
#define PMULPackageDataProvider_h

#include <iosfwd>
#include <string>

#include <y2pm/PMPackageDataProvider.h>
#include <y2pm/PMPackageDataProviderPtr.h>
#include <y2pm/PMDataProvider.h>

#include <y2pm/PMPackage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMULPackageDataProvider
/**
 * @short Realizes PackageDataProvider for UnitedLinux packages format
 * @see DataProvider
 **/
class PMULPackageDataProvider : public PMPackageDataProvider  {
  REP_BODY(PMULPackageDataProvider);
  private:
    // save file position and size data for each attribute
    typedef struct {std::streampos pos; int size;} attrpos_t;
    attrpos_t attrpos[PMPackage::PKG_NUM_ATTRIBUTES];
    // cache retrieved attribute values here
    // these are set if attrpos[<attr>].size < 0
    PkgAttributeValue attrval[PMPackage::PKG_NUM_ATTRIBUTES];

  protected:

    PMULPackageDataProvider();

    ~PMULPackageDataProvider();

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
	const PkgAttributeValue value);

    /** inject some package attribute by file offset
     * */
    void setAttributeValue(
	PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	std::streampos pos, int size);
};

///////////////////////////////////////////////////////////////////

#endif // PMULPackageDataProvider_h
