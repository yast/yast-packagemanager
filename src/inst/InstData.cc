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

#include <y2pm/InstData.h>

#include <y2pm/DataOldSuSE.h>

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
    /**
     * constructor
     * initialization with new media
     */
InstData::InstData (MediaAccessPtr media)
{
    return;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstData::InstData
//	METHOD TYPE : Constructor
//
//	DESCRIPTION : initialization with known media
//
InstData::InstData (const Pathname & contentcachefile)
{
    return;
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
 * generate PMSolvable objects for each selection on the source
 * @return list of PMSolvablePtr on this source
 */
const std::list<PMSolvablePtr> *
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
 * generate PMSolvable objects for each patch on the source
 * @return list of PMSolvablePtr on this source
 */
const std::list<PMSolvablePtr> *
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

