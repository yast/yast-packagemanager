/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       PMYouPackageDataProvider.h
   Purpose:    Implements PackageDataProvider for YOU patches.
   Author:     Cornelius Schumacher <cschum@suse.de>
   Maintainer: Cornelius Schumacher <cschum@suse.de>

/-*/

#ifndef PMYouPackageDataProvider_h
#define PMYouPackageDataProvider_h

#include <y2pm/PMYouPackageDataProviderPtr.h>
#include <y2pm/PMPackageDataProvider.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMObject.h>

class PMYouPackageDataProvider : virtual public Rep, public PMPackageDataProvider {
	REP_BODY(PMYouPackageDataProvider);

    public:

	PMYouPackageDataProvider();

	virtual ~PMYouPackageDataProvider();

	/**
	 * access functions for PMObject attributes
	 */

	const std::string summary() const;
	const std::list<std::string> description() const;
	const std::list<std::string> insnotify() const;
	const std::list<std::string> delnotify() const;

	/**
	 * Object attribute retrieval. (DataProvider interface)
	 * @see PMDataProvider
	 * @see PMObject
	 **/
	virtual PkgAttributeValue getAttributeValue( constPMObjectPtr obj_r,
						     PMObject::PMObjectAttribute attr_r );
	/**
	 * Package attribute retrieval. (PackageDataProvider interface)
	 * @see PMPackageDataProvider
	 * @see PMPackage
	 **/
	virtual PkgAttributeValue getAttributeValue( constPMPackagePtr pkg_r,
						     PMPackage::PMPackageAttribute attr_r );

#if 0
       /** inject attibute to cache */
	virtual void setAttributeValue(
	    PMPackagePtr pkg, PMObject::PMObjectAttribute attr,
	    const PkgAttributeValue& value);

	/** inject attibute to cache */
	virtual void setAttributeValue(
	    PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	    const PkgAttributeValue& value);
#endif

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

#endif // PMYouPackageDataProvider_h
