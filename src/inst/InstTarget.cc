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

   File:       InstTarget.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

   Purpose:	Class for installation target
		It provides access to the installation target
		and shares some API calls with InstSrc, esp.
		- selection information
		- package information
		- patch information
		but does not report about installable but installed
		selections, packages, or patches.

		Additionally, the InstTarget can also change package
		and patch lists, by installing or removing them.

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/InstTarget.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstTarget
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::InstTarget
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
    /**
     * constructor
     * @param rootpath, path to root ("/") of target system
     * Usually "/" if the InstTarget object is actually running
     * inside the target. But might be "/mnt" during installation
     * (running in inst-sys) or "/whatever" if installing into
     * a directory
     */
InstTarget::InstTarget ( const std::string & rootpath )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::~InstTarget
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstTarget::~InstTarget()
{
}

//-----------------------------
// general functions

/**
 * clean up, e.g. remove all caches
 */
bool
InstTarget::Erase()
{
    D__ << __FUNCTION__ << std::endl;
    return false;
}

/**
 * @return description of Installation source
 * This is needed by the InstTargetMgr
 */
const InstDescr *
InstTarget::getDescription() const
{
    D__ << __FUNCTION__ << std::endl;
    return _descr;
}

/**
 * register this source (store cache files etc)
 * return pathname of saved content file
 */
const Pathname
InstTarget::registerSource (void) const
{
    D__ << __FUNCTION__ << std::endl;
    return _descr->writeCache ();
}

//-----------------------------
// activation status

/**
 * return activation status
 */
bool
InstTarget::getActivation() const
{
    D__ << __FUNCTION__ << std::endl;
    return _descr->getActivation();
}


/**
 * temporary (de)activate source
 */
void
InstTarget::setActivation (bool yesno)
{
    D__ << __FUNCTION__ << std::endl;
    return _descr->setActivation (yesno);
}

//-----------------------------
// source content access

/**
 * return the number of selections on this source
 */
int
InstTarget::numSelections() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->numSelections();
}


/**
 * return the number of packages on this source
 */
int
InstTarget::numPackages() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->numPackages();
}


/**
 * return the number of patches on this source
 */
int
InstTarget::numPatches() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->numPatches();
}


/**
 * generate PMSolvable objects for each selection on the source
 * @return list of PMSolvablePtr on this source
 */
const std::list<PMSolvablePtr> *
InstTarget::getSelections() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->getSelections();
}

/**
 * generate PMPackage objects for each Item on the source
 * @return list of PMPackagePtr on this source
 * */
const std::list<PMPackagePtr> *
InstTarget::getPackages() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->getPackages();
}

/**
 * generate PMSolvable objects for each patch on the source
 * @return list of PMSolvablePtr on this source
 */
const std::list<PMSolvablePtr> *
InstTarget::getPatches() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->getPatches();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
InstTarget::dumpOn( ostream & str ) const
{
    Rep::dumpOn( str );
    return str;
}

