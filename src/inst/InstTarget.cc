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

/**
 * constructor
 * @param rootpath, path to root ("/") of target system
 * Usually "/" if the InstTarget object is actually running
 * inside the target. But might be "/mnt" during installation
 * (running in inst-sys) or "/whatever" if installing into
 * a directory
 */
InstTarget::InstTarget ( ) :
    _rpminstflags(RpmDb::RPMINST_NODEPS|RpmDb::RPMINST_FORCE|RpmDb::RPMINST_IGNORESIZE),
    _rpmremoveflags(RpmDb::RPMINST_NODEPS|RpmDb::RPMINST_FORCE)
{
    _rpmdb = new RpmDb();
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

PMError InstTarget::init (const Pathname & rootpath, bool createnew)
{
    _rootdir = rootpath;
    return _rpmdb->initDatabase(_rootdir.asString(), createnew);
}

PMError InstTarget::bringIntoCleanState()
{
    return _rpmdb->rebuildDatabase();
}

//-----------------------------
// general functions

/**
 * clean up, e.g. remove all caches
 */
bool
InstTarget::Erase()
{
    //TODO
    D__ << std::endl;
    return false;
}
#if 0
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

#endif

//-----------------------------
// target content access


/**
 * generate PMSelection objects for each selection on the target
 * @return list of PMSelectionPtr on this target
 */
const std::list<PMSelectionPtr>&
InstTarget::getSelections (void) const
{
    return InstData::getSelections();
}

/**
 * generate PMPackage objects for each Item on the target
 * @return list of PMPackagePtr on this target
 * */
const std::list<PMPackagePtr>&
InstTarget::getPackages (void) const
{
    return _rpmdb->getPackages();
}

/**
 * generate PMSolvable objects for each patch on the target
 * @return list of PMSolvablePtr on this target
 */
const std::list<PMYouPatchPtr>&
InstTarget::getPatches (void) const
{
    return InstData::getPatches();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
std::ostream &
InstTarget::dumpOn( std::ostream & str ) const
{
    Rep::dumpOn( str );
    return str;
}

//--------------------------------------------------------------------
// target specific functions

void InstTarget::setPkgInstFlags(unsigned flags)
{
    _rpminstflags = flags;
}

unsigned InstTarget::getPkgInstFlags() const
{
    return _rpminstflags;
}

void InstTarget::setPkgRemoveFlags(unsigned flags)
{
    _rpmremoveflags = flags;
}

unsigned InstTarget::getPkgRemoveFlags() const
{
    return _rpmremoveflags;
}

PMError InstTarget::installPackage (const std::string& filename, unsigned flags)
{
    return _rpmdb->installPackage(filename,flags?flags:_rpminstflags);
}

PMError InstTarget::installPackages (const std::list<std::string>& filenames, unsigned flags)
{
    PMError err;    
    for(list<string>::const_iterator it= filenames.begin(); it != filenames.end(); ++it)
    {
	err = installPackage(*it,flags);
        if ( err ) break;
    }
    return err;
}

PMError InstTarget::removePackage(const std::string& label, unsigned flags)
{
    return _rpmdb->removePackage(label,flags?flags:_rpminstflags);
}

PMError InstTarget::removePackages(const std::list<std::string>& labels, unsigned flags)
{
    PMError err;    
    for(list<string>::const_iterator it= labels.begin(); it != labels.end(); ++it)
    {
	err = removePackage(*it,flags);
        if ( err ) break;
    }
    return err;
}

const std::string& InstTarget::getRoot() const
{
    return _rootdir.asString();
}


void InstTarget::setPackageInstallProgressCallback(void (*func)(int,void*), void* data)
{
    _rpmdb->setProgressCallback(func,data);
}
