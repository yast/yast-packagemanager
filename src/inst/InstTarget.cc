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

IMPL_BASE_POINTER(InstTarget);

/**
 * constructor
 * @param rootpath, path to root ("/") of target system
 * Usually "/" if the InstTarget object is actually running
 * inside the target. But might be "/mnt" during installation
 * (running in inst-sys) or "/whatever" if installing into
 * a directory
 */
InstTarget::InstTarget ( const std::string & rootpath ) :
    _rpminstflags(RpmDb::RPMINST_NODEPS|RpmDb::RPMINST_FORCE|RpmDb::RPMINST_IGNORESIZE),
    _rpmremoveflags(RpmDb::RPMINST_NODEPS|RpmDb::RPMINST_FORCE),
    _rootdir(rootpath)
{
    _rpmdb = new RpmDb(_rootdir.asString());
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

PMError InstTarget::init( bool createnew)
{
    return _rpmdb->initDatabase(createnew);
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

//-----------------------------
// source content access
#endif

/**
 * return the number of selections on this source
 */
int
InstTarget::numSelections() const
{
    //TODO
    D__ << std::endl;
    return 0;
}


/**
 * return the number of packages on this source
 */
int
InstTarget::numPackages() const
{
    //TODO
    D__ << std::endl;
    return 0;
}


/**
 * return the number of patches on this source
 */
int
InstTarget::numPatches() const
{
    D__ << std::endl;
    return 0;
}



PMError InstTarget::getPackages (std::list<PMPackagePtr>& pkglist)
{
    return _rpmdb->getPackages(pkglist);
}

PMError InstTarget::getSelections (std::list<PMSelectionPtr>& sellist)
{
    //TODO
    return 0;
}

PMError InstTarget::getYOUPatches (std::list<PMYouPatchPtr>& youpatchlist)
{
    //TODO
    return 0;
}

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
    for(list<string>::const_iterator it= filenames.begin(); it != filenames.end(); ++it)
    {
	installPackage(*it,flags);
    }
    return 0;
}

PMError InstTarget::removePackage(const std::string& label, unsigned flags)
{
    return _rpmdb->removePackage(label,flags?flags:_rpminstflags);
}

PMError InstTarget::removePackages(const std::list<std::string>& labels, unsigned flags)
{
    for(list<string>::const_iterator it= labels.begin(); it != labels.end(); ++it)
    {
	removePackage(*it,flags);
    }
    return 0;
}

const std::string& InstTarget::getRoot() const
{
    return _rootdir.asString();
}


ostream &
InstTarget::dumpOn( ostream & str ) const
{
    Rep::dumpOn( str );
    return str;
}

void InstTarget::setPackageInstallProgressCallback(void (*func)(double,void*), void* data)
{
    _rpmdb->setProgressCallback(func,data);
}
