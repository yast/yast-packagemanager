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

   File:       InstData.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>

#include <y2pm/InstData.h>

#include <y2pm/PMPackage.h>
#include <y2pm/PMSelection.h>
#include <y2pm/PMYouPatch.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstData
//
///////////////////////////////////////////////////////////////////

//IMPL_HANDLES(InstSrcData);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstData::InstData
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstData::InstData()
    : _selections( 0 )
    , _packages( 0 )
    , _patches( 0 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstData::~InstData
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstData::~InstData()
{
#warning CHECKIT must detach packages to get empty refcounts
  delete _selections;
  delete _packages;
  delete _patches;
}

/**
 * write media content data to cache file
 * @param pathname of corresponding InstSrcDescr cache file
 * @return pathname of written cache
 * writes content cache data to an ascii file
 */
const Pathname
InstData::writeCache (const Pathname &descrpathname)
{
    Pathname datacachename = descrpathname.dirname() + "content.cache";
    return datacachename;
}

//-----------------------------
// source content access

/**
 * return the number of selections on this source
 */
int
InstData::numSelections() const
{
    return _selections->size();
}


/**
 * return the number of packages on this source
 */
int
InstData::numPackages() const
{
    return _packages->size();
}


/**
 * return the number of patches on this source
 */
int
InstData::numPatches() const
{
    return _patches->size();
}


/**
 * generate PMSelection objects for each selection on the source
 * @return list of PMSelectionPtr on this source
 */
const std::list<PMSelectionPtr> *
InstData::getSelections() const
{
    return _selections;
}


/**
 * generate PMPackage objects for each Item on the source
 * @return list of PMPackagePtr on this source
 */
const std::list<PMPackagePtr> *
InstData::getPackages() const
{
    return _packages;
}

/**
 * find list of packages
 * @return list of PMPackagePtr matching name ,[version] ,[release] ,[architecture]
 */
const std::list<PMPackagePtr>
InstData::findPackages (const std::list<PMPackagePtr> *packagelist, const string& name, const string& version, const string& release, const string& arch)
{
    std::list<PMPackagePtr> hits;
    if (packagelist == 0)
    {
	ERR << "InstData::findPackages (NULL, ...)" << endl;
	return hits;
    }

    typedef std::list<PMPackagePtr>::const_iterator PkgLI;
    for (PkgLI pkg = packagelist->begin(); pkg != packagelist->end(); ++pkg)
    {
	if (!name.empty()
	    && ((*pkg)->name() != name))
	{
	    continue;
	}
	if (!version.empty()
	    && ((*pkg)->edition().version() != version))
	{
	    continue;
	}
	if (!release.empty()
	    && ((*pkg)->edition().release() != release))
	{
	    continue;
	}
	if (!arch.empty()
	    && ((*pkg)->arch() != arch))
	{
	    continue;
	}
	hits.push_back (*pkg);
    }
    return hits;
}

/**
 * generate PMSolvable objects for each patch on the source
 * @return list of PMSolvablePtr on this source
 */
const std::list<PMYouPatchPtr> *
InstData::getPatches() const
{
    return _patches;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstData::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstData::dumpOn( ostream & str ) const
{
//  Rep::dumpOn( str );
  return str;
}

