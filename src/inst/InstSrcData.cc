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

   File:       InstSrcData.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2pm/InstSrcData.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcData
//
///////////////////////////////////////////////////////////////////

IMPL_HANDLES(InstSrcData);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::InstSrcData
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
    /**
     * constructor
     * initialization with new media
     */
InstSrcData::InstSrcData (const MediaAccess *media)
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::InstSrcData
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
    /**
     * constructor
     * initialization with known media
     */
InstSrcData::InstSrcData (const Pathname & contentcachefile)
{
    return;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::~InstSrcData
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcData::~InstSrcData()
{
}

/**
 * write media content data to cache file
 * @param pathname of corresponding InstSrcDescr cache file
 * @return pathname of written cache
 * writes content cache data to an ascii file
 */
const Pathname
InstSrcData::writeCache (const Pathname &descrpathname)
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
InstSrcData::numSelections() const
{
    return 0;
}


/**
 * return the number of packages on this source
 */
int
InstSrcData::numPackages() const
{
    return 0;
}


/**
 * return the number of patches on this source
 */
int
InstSrcData::numPatches() const
{
    return 0;
}


/**
 * generate PMSolvable objects for each selection on the source
 * @return list of PMSolvablePtr on this source
 */
std::list<PMSolvablePtr>
InstSrcData::getSelections()
{
    std::list<PMSolvablePtr> x;
    return x;
}


/**
 * generate PMPackage objects for each Item on the source
 * @return list of PMPackagePtr on this source
 * */
std::list<PMPackagePtr>
InstSrcData::getPackages()
{
    std::list<PMPackagePtr> x;
    return x;
}


/**
 * generate PMSolvable objects for each patch on the source
 * @return list of PMSolvablePtr on this source
 */
std::list<PMSolvablePtr>
InstSrcData::getPatches()
{
    std::list<PMSolvablePtr> x;
    return x;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrcData::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

