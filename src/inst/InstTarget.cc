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
#include <fstream>

#include <y2util/Y2SLog.h>
#include <y2pm/InstTarget.h>
#include <y2pm/PMYouPatchPaths.h>
#include <y2pm/PMYouPatchInfo.h>

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
    _rpmremoveflags(RpmDb::RPMINST_NODEPS|RpmDb::RPMINST_FORCE),
    _patchesInitialized( false )
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
 * generate PMYouPatch objects for each patch on the target
 * @return list of PMYouPatchPtr on this target
 */
const std::list<PMYouPatchPtr>&
InstTarget::getPatches (void) const
{
    if ( !_patchesInitialized ) {
#warning FIXME: Get product info from InstTarget::descr
        PMYouPatchPaths paths;
        PMYouPatchInfo patchInfo;

        Url u( "dir://" + ( getRoot() + paths.installDir() ).asString() );
        Pathname path;

        PMError error = patchInfo.readDir( u, path, _patches );
        if ( error ) {
            E__ << "Error reading patch info for installed patches." << endl;
        }

        _patchesInitialized = true;
    }

    return _patches;
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

bool InstTarget::setInstallationLogfile(const std::string& logfile)
{
    return _rpmdb->setInstallationLogfile(logfile);
}

PMError InstTarget::installPatch( const Pathname &filename )
{
#warning FIXME: Get product info from InstTarget::descr
    PMYouPatchPaths paths;

    Pathname dest = getRoot();
    dest += paths.installDir();

    int err = PathInfo::assert_dir( dest );
    if ( err ) {
      E__ << "Can't create " << dest << " (errno: " << err
          << ")" << endl;
      return PMError( InstTargetError::E_error );
    }

    dest += filename.basename();

    D__ << "in: " << filename << " out: " << dest << endl;

    ifstream in( filename.asString().c_str() );
    if ( in.fail() ) {
        E__ << "Can't read " << filename << endl;
        return PMError( InstTargetError::E_error );
    }
    ofstream out( dest.asString().c_str() );
    if ( out.fail() ) {
        E__ << "Can't write " << dest << endl;
        return PMError( InstTargetError::E_error );
    }

    int size = 1000;
    char buf[size];
    while( !in.eof() ) {
      in.read( buf, size );
      out.write( buf, in.gcount() );
    }

    return PMError();
}

PMError InstTarget::executeScript( const Pathname &scriptname )
{
    int result = system( ( "/bin/bash " + scriptname.asString() ).c_str() );
    if ( result != 0 ) {
        E__ << "Script failed. Exit code " << result << endl;
        return PMError( InstTargetError::E_error );
    }

    return PMError();
}
