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
#include <y2pm/PMYouPatchInfo.h>

#include <y2util/YRpmGroupsTree.h>

class PMYouPackageDataProvider : virtual public Rep, public PMPackageDataProvider {
	REP_BODY(PMYouPackageDataProvider);

        PMYouPatchInfoPtr _patchInfo;

    public:

	PMYouPackageDataProvider( const PMYouPatchInfoPtr & );

	virtual ~PMYouPackageDataProvider();

    public:

        /**
	 * Looks like nothing but location is provided as PMYouPackage data
	 * @see PMPackageDataProvider
	 **/
	virtual std::string location( const PMPackage & pkg_r ) const;

        FSize size( const PMPackage & pkg_r ) const;
};

#endif // PMYouPackageDataProvider_h
