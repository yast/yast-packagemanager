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

   File:       PMRpmPackageDataProvider.cc
   Purpose:    Implements PackageDataProvider for installed rpms
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/PMRpmPackageDataProvider.h>
#include <y2pm/PkgName.h>
#include <y2pm/RpmDb.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMRpmPackageDataProvider
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(PMRpmPackageDataProvider);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMRpmPackageDataProvider::PMRpmPackageDataProvider
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMRpmPackageDataProvider::PMRpmPackageDataProvider(RpmDbPtr rpmdb)
    : _rpmdb(rpmdb)
{
}

string PMRpmPackageDataProvider::getAttributeValue(PMPackagePtr pkg,
    PMObject::PMObjectAttribute attr)
{
    D__ << pkg->name() << pkg->edition().as_string() << endl;
    const char* queryformat = NULL;

    switch(attr)
    {
	case PMObject::ATTR_SUMMARY:
	    queryformat = "%{SUMMARY}\n";
	    break;
	case PMObject::ATTR_DESCRIPTION:
	    queryformat = "%{DESCRIPTION}\n";
	    break;
	case PMObject::ATTR_SIZE:
	    queryformat = "%{SIZE}\n";
	    break;
	case PMObject::PMOBJ_NUM_ATTRIBUTES:
	    // invalid
	    return "invalid query";
    }

    return _rpmdb->queryPackage(queryformat,string(pkg->name())+"-"+pkg->edition().as_string());
}

string PMRpmPackageDataProvider::getAttributeValue(PMPackagePtr pkg,
    PMPackage::PMPackageAttribute attr)
{
    D__ << pkg->name() << pkg->edition().as_string() << endl;
    const char* queryformat = NULL;

    switch(attr)
    {
	case PMPackage::ATTR_BUILDTIME:
	    queryformat = "%{BUILDTIME}";
	    break;
	case PMPackage::ATTR_BUILDHOST:
	    queryformat = "%{BUILDHOST}";
	    break;
	case PMPackage::ATTR_INSTALLTIME:
	    queryformat = "%{INSTALLTIME}";
	    break;
	case PMPackage::ATTR_DISTRIBUTION:
	    queryformat = "%{DISTRIBUTION}";
	    break;
	case PMPackage::ATTR_VENDOR:
	    queryformat = "%{VENDOR}";
	    break;
	case PMPackage::ATTR_LICENSE:
	    queryformat = "%{LICENSE}";
	    break;
	case PMPackage::ATTR_PACKAGER:
	    queryformat = "%{PACKAGER}";
	    break;
	case PMPackage::ATTR_GROUP:
	    queryformat = "%{GROUP}";
	    break;
	case PMPackage::ATTR_CHANGELOG:
	    queryformat =
    "[* %{CHANGELOGTIME:day} %{CHANGELOGNAME}\n\n%{CHANGELOGTEXT}\n\n]";
	    break;
	case PMPackage::ATTR_URL:
	    queryformat = "%{URL}";
	    break;
	case PMPackage::ATTR_OS:
	    queryformat = "%{OS}";
	    break;
	case PMPackage::ATTR_ARCH:
	    queryformat = "%{ARCH}";
	    break;
	case PMPackage::ATTR_PREIN:
	    queryformat = "%{PREIN}";
	    break;
	case PMPackage::ATTR_POSTIN:
	    queryformat = "%{POSTIN}";
	    break;
	case PMPackage::ATTR_PREUN:
	    queryformat = "%{PREUN}";
	    break;
	case PMPackage::ATTR_POSTUN:
	    queryformat = "%{POSTUN}";
	    break;
	case PMPackage::ATTR_SOURCERPM:
	    queryformat = "%{SOURCERPM}";
	    break;
	case PMPackage::ATTR_ARCHIVESIZE:
	    queryformat = "%{ARCHIVESIZE}";
	    break;
	case PMPackage::ATTR_AUTHOR:
	    // not available as rpm tag
	    return "";
	case PMPackage::PKG_NUM_ATTRIBUTES:
	    // invalid
	    return "invalid query";
    }

    return _rpmdb->queryPackage(queryformat,string(pkg->name())+"-"+pkg->edition().as_string());
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMRpmPackageDataProvider::~PMRpmPackageDataProvider
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMRpmPackageDataProvider::~PMRpmPackageDataProvider()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMRpmPackageDataProvider::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMRpmPackageDataProvider::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

