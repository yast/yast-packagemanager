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

#include <y2pm/RpmDbPtr.h>
#include <y2pm/PMRpmPackageDataProviderPtr.h>
#include <y2pm/PMPackageDataProvider.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMObject.h>

class REP_CLASS(PMRpmPackageDataProvider), public PMPackageDataProvider {

	REP_BODY(PMRpmPackageDataProvider)

	RpmDbPtr _rpmdb;

    public:

	PMRpmPackageDataProvider(RpmDbPtr rpmdb);

	virtual ~PMRpmPackageDataProvider();

	/** see PMObject */
	virtual std::string getAttributeValue(
	    PMPackagePtr pkg, PMObject::PMObjectAttribute attr);
	/** see PMObject */
	virtual std::string getAttributeValue(
	    PMPackagePtr pkg, PMPackage::PMPackageAttribute attr);

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

#endif // PMRpmPackageDataProvider_h

// vim:sw=4
