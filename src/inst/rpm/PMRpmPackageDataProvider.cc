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

IMPL_DERIVED_POINTER(PMRpmPackageDataProvider, PMPackageDataProvider, PMDataProvider );

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

PkgAttributeValue PMRpmPackageDataProvider::getAttributeValue( constPMObjectPtr obj_r,
							       PMObject::PMObjectAttribute attr )
{
    PMPackagePtr pkg( PMPackagePtr::cast_away_const( obj_r ) );
    const char* queryformat = NULL;
    PkgAttributeValue ret;

    switch(attr)
    {
#warning FIXME PMSolvable::ATTR_ARCH
	case PMObject::ATTR_ARCH:
	    queryformat = "%{ARCH}\n";
	    break;
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
	    return PkgAttributeValue("invalid query");
	    break;
	// FIXME
	case PMObject::ATTR_INSNOTIFY:
	case PMObject::ATTR_DELNOTIFY:
	    // invalid
	    return PkgAttributeValue();
	    break;
    }

    // check if cached
    AttrVecPosition pos = pkgattr2pos(attr);
    if(pos != AV_POS_INVALID)
    {
	PkgMap::iterator it = _pkgmap.find(pkg);

	// is package in cache?
	if(it != _pkgmap.end())
	{
//	    D__ << "found " << pkg->name() << " in cache" << endl;

	    AttrVec& vec = it->second;
	    ret = vec[pos];
	    if(!ret.empty())
		return ret;
	}
    }

    ret = _rpmdb->queryPackage(queryformat,string(pkg->name())+"-"+pkg->edition().as_string());

    // if attribute is to be cached but not stored yet, do it now
    if(pos != AV_POS_INVALID)
    {
//	D__ << "insert attribute " << pkg->getAttributeName(attr) << " to cache" << endl;
	setAttributeValue(pkg,attr,ret);
    }

    return ret;
}

PkgAttributeValue PMRpmPackageDataProvider::getAttributeValue( constPMPackagePtr pkg_r,
							       PMPackage::PMPackageAttribute attr )
{
    PMPackagePtr pkg( PMPackagePtr::cast_away_const( pkg_r ) );
    const char* queryformat = NULL;

    switch (attr)
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
    "[* %{CHANGELOGTIME:day} %{CHANGELOGNAME}\\n\\n%{CHANGELOGTEXT}\\n\\n]";
	    break;
	case PMPackage::ATTR_URL:
	    queryformat = "%{URL}";
	    break;
	case PMPackage::ATTR_OS:
	    queryformat = "%{OS}";
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
	case PMPackage::ATTR_AUTHORS:
	    // not available as rpm tag
	    return PkgAttributeValue("");
	case PMPackage::ATTR_FILENAMES:
	    queryformat = "[%{FILENAMES}\\n]";
	    break;
	case PMPackage::PMPKG_NUM_ATTRIBUTES:
	    // invalid
	    return PkgAttributeValue("invalid query");
	// FIXME
	case PMPackage::ATTR_RECOMMENDS:
	case PMPackage::ATTR_SUGGESTS:
	case PMPackage::ATTR_LOCATION:
	case PMPackage::ATTR_KEYWORDS:
	    return PkgAttributeValue();
    }

    PkgAttributeValue ret;
    // check if cached
    AttrVecPosition pos = pkgattr2pos(attr);
    if(pos != AV_POS_INVALID)
    {
	PkgMap::iterator it = _pkgmap.find(pkg);

	// is package in cache?
	if(it != _pkgmap.end())
	{
//	    D__ << "found " << pkg->name() << " in cache" << endl;

	    AttrVec& vec = it->second;
	    ret = vec[pos];
	    if(!ret.empty())
		return ret;
	}
    }

    ret = _rpmdb->queryPackage(queryformat,string(pkg->name())+"-"+pkg->edition().as_string());

    // if attribute is to be cached but not stored yet, do it now
    if(pos != AV_POS_INVALID)
    {
//	D__ << "insert attribute " << pkg->getAttributeName(attr) << " to cache" << endl;
	setAttributeValue(pkg,attr,ret);
    }

    return ret;
}

/** compute vector position from attribute
 *
 * @return position or AV_POS_INVALID if this item is not to be cached
 * */
PMRpmPackageDataProvider::AttrVecPosition PMRpmPackageDataProvider::pkgattr2pos(unsigned attr)
{
    switch(attr)
    {
	case PMObject::ATTR_SIZE:
	    return AV_SIZE;
	case PMObject::ATTR_SUMMARY:
	    return AV_SUMMARY;
	case PMPackage::ATTR_GROUP:
	    return AV_GROUP;
	default:
	    return AV_POS_INVALID;
    };
}

/** inject attibute to cache */
void PMRpmPackageDataProvider::setAttributeValue(
    PMPackagePtr pkg, PMPackage::PMPackageAttribute attr, const PkgAttributeValue& value)
{
    _setAttributeValue(pkg,attr,value);
}

/** inject attibute to cache */
void PMRpmPackageDataProvider::setAttributeValue(
    PMPackagePtr pkg, PMObject::PMObjectAttribute attr, const PkgAttributeValue& value)
{
    _setAttributeValue(pkg,attr,value);
}

/** inject attibute to cache */
void PMRpmPackageDataProvider::_setAttributeValue(
    PMPackagePtr pkg, unsigned attr, const PkgAttributeValue& value)
{
    AttrVecPosition pos = pkgattr2pos(attr);
    if(pos == AV_POS_INVALID)
	return;

    PkgMap::iterator it = _pkgmap.find(pkg);

    AttrVec vec;

    // package not known yet
    if(it == _pkgmap.end())
    {
	vec = AttrVec(AV_NUM_ITEMS,PkgAttributeValue());
    }
    else
	vec = it->second;

    vec[pos]=value;

    _pkgmap[pkg]=vec;
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

