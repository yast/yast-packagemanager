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

   File:       PMRpmPackageDataProvider.h
   Purpose:    Implements PackageDataProvider for installed rpms
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#ifndef PMRpmPackageDataProvider_h
#define PMRpmPackageDataProvider_h

#include <iosfwd>
#include <map>
#include <vector>

#include <y2pm/RpmDbPtr.h>
#include <y2pm/PMRpmPackageDataProviderPtr.h>
#include <y2pm/PMPackageDataProvider.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMObject.h>

class PMRpmPackageDataProvider : virtual public Rep, public PMPackageDataProvider {
	REP_BODY(PMRpmPackageDataProvider);

	RpmDbPtr _rpmdb;

	/** vector to hold cached data */
	typedef std::vector<std::string> AttrVec;

	/** meaning of vector positions
	 * pkgattr2pos must be adapted if changed
	 * */
	enum AttrVecPosition
	{
	    AV_POS_INVALID = -1,
	    AV_SIZE = 0,
	    AV_SUMMARY,
	    AV_GROUP,

	    AV_NUM_ITEMS
	};

	/** map associates packages with cached attributes */
	typedef std::map<PMPackagePtr,AttrVec> PkgMap;

	PkgMap _pkgmap;
	
	/** compute vector position from attribute
	 *
	 * @return position or AV_POS_INVALID if this item is not to be cached
	 * */
	AttrVecPosition pkgattr2pos(unsigned attr);

	/** inject attibute to cache */
	void _setAttributeValue(
	    PMPackagePtr pkg, unsigned attr, const std::string& value);

    public:

	PMRpmPackageDataProvider(RpmDbPtr rpmdb);

	virtual ~PMRpmPackageDataProvider();

	/** see PMObject */
	virtual std::string getAttributeValue(
	    PMPackagePtr pkg, PMObject::PMObjectAttribute attr);
	/** see PMObject */
	virtual std::string getAttributeValue(
	    PMPackagePtr pkg, PMPackage::PMPackageAttribute attr);

	/** inject attibute to cache */
	virtual void setAttributeValue(
	    PMPackagePtr pkg, PMObject::PMObjectAttribute attr,
	    const std::string& value);

	/** inject attibute to cache */
	virtual void setAttributeValue(
	    PMPackagePtr pkg, PMPackage::PMPackageAttribute attr,
	    const std::string& value);

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

#endif // PMRpmPackageDataProvider_h

// vim:sw=4
