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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstData::InstData
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstData::InstData()
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
    ERR << "Dummy InstData::writeCache()" << endl;
    return datacachename;
}

//-----------------------------
// content access

/**
 * generate PMSelection objects for each selection on the source
 * @return list of PMSelectionPtr on this source
 */
const std::list<PMSelectionPtr>&
InstData::getSelections (void) const
{
    static std::list<PMSelectionPtr> selections;
    ERR << "InstData::getSelections()" << endl;
    return selections;
}


/**
 * generate PMPackage objects for each Item on the source
 * @return list of PMPackagePtr on this source
 */
const std::list<PMPackagePtr>& 
InstData::getPackages (void) const
{
    static std::list<PMPackagePtr> packages;
    ERR << "InstData::getPackages()" << endl;
    return packages;
}

/**
 * generate PMSolvable objects for each patch on the source
 * @return list of PMSolvablePtr on this source
 */
const std::list<PMYouPatchPtr>&
InstData::getPatches (void) const
{
    static std::list<PMYouPatchPtr> patches;
    ERR << "InstData::getPatches()" << endl;
    return patches;
}


const std::list<PMPackagePtr>
InstData::findPackages (const std::list<PMPackagePtr>& packages, const string& name, const string& version, const string& release, const string& arch)
{
    std::list<PMPackagePtr> hits;
//    MIL << "InstData::findPackages (" << packages << ", " << name << ", " << version << ", " << release << ", " << arch << ")" << endl;
//    MIL << "checking " << packagelist->size() << " packages" << endl;
    typedef std::list<PMPackagePtr>::const_iterator SolvLI;
    for (SolvLI package = packages.begin(); package != packages.end(); ++package)
    {
//    MIL << "?: " << (*package)->name() << "-" << (*package)->version() << "-" << (*package)->release() << "-" << (*package)->arch() << endl;
	if (!name.empty()
	    && ((*package)->name() != name))
	{
	    continue;
	}
	if (!version.empty()
	    && ((*package)->edition().version() != version))
	{
	    continue;
	}
	if (!release.empty()
	    && ((*package)->edition().release() != release))
	{
	    continue;
	}
	if (!arch.empty()
	    && ((*package)->arch() != arch))
	{
	    continue;
	}
	hits.push_back (*package);
    }
//    MIL << "returning " << hits.size() << " packages" << endl;
    return hits;
}


const std::list<PMSelectionPtr>
InstData::findSelections (const std::list<PMSelectionPtr>& selections, const string& name, const string& version, const string& release, const string& arch)
{
    std::list<PMSelectionPtr> hits;
//    MIL << "InstData::findSelections (" << selections << ", " << name << ", " << version << ", " << release << ", " << arch << ")" << endl;
//    MIL << "checking " << selectionlist->size() << " selections" << endl;
    typedef std::list<PMSelectionPtr>::const_iterator SolvLI;
    for (SolvLI selection = selections.begin(); selection != selections.end(); ++selection)
    {
//    MIL << "?: " << (*selection)->name() << "-" << (*selection)->version() << "-" << (*selection)->release() << "-" << (*selection)->arch() << endl;
	if (!name.empty()
	    && ((*selection)->name() != name))
	{
	    continue;
	}
	if (!version.empty()
	    && ((*selection)->edition().version() != version))
	{
	    continue;
	}
	if (!release.empty()
	    && ((*selection)->edition().release() != release))
	{
	    continue;
	}
	if (!arch.empty()
	    && ((*selection)->arch() != arch))
	{
	    continue;
	}
	hits.push_back (*selection);
    }
//    MIL << "returning " << hits.size() << " selections" << endl;
    return hits;
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

