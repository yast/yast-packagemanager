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

extern "C" {
#include <unistd.h>
#include <sys/statvfs.h>
}

#include <iostream>
#include <fstream>

#include <string.h>

#include <y2util/Y2SLog.h>
#include <y2pm/InstTarget.h>
#include <y2pm/InstTargetProdDB.h>
#include <y2pm/InstTargetSelDB.h>
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
    _patchesInitialized( false ),
    _proddb( new InstTargetProdDB ),
    _seldb( new InstTargetSelDB )
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

    _proddb->open( _rootdir, createnew );
    _seldb->open( _rootdir, createnew );

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


/**
 * get target base architecture
 *
 *
 */
PkgArch
InstTarget::baseArch ()
{
    if (((const std::string)_base_arch).empty())
    {
	char *argv[3] = { "uname", "-m", 0 };
	ExternalProgram process (argv, ExternalProgram::Stderr_To_Stdout, false, -1, true);
	string output = process.receiveLine ();
	if (output.length() == 0)
	{
	    ERR << "No output from 'uname -m'" << endl;
	}
	else
	{
	    string::size_type endpos = output.find_first_of ("\n");
	    if (endpos != string::npos)
		_base_arch = PkgArch (output.substr (0, endpos));
	    else
		_base_arch = PkgArch (output);
	}
	MIL << "_base_arch '" << _base_arch << "'" << endl;

	// some CPUs report i686 but dont implement cx8 and cmov
	// check for both flags in /proc/cpuinfo and downgrade
	// to i586 if either is missing (cf bug #18885)
	if (_base_arch == "i686")
	{
	    std::ifstream cpuinfo ("/proc/cpuinfo");
	    if (!cpuinfo)
	    {
		ERR << "Cant open /proc/cpuinfo" << endl;
	    }
	    else
	    {
		char infoline[1024];
		while (cpuinfo.good())
		{
		    if (!cpuinfo.getline (infoline, 1024, '\n'))
		    {
			if (cpuinfo.eof())
			    break;
		    }
		    if (strncmp (infoline, "flags", 5) == 0)
		    {
			string flagsline (infoline);
			if ((flagsline.find("cx8") == string::npos)
			    || (flagsline.find("cmov") == string::npos))
			{
			    _base_arch = PkgArch("i586");
			}
			break;
		    } // flags found
		} // read proc/cpuinfo
	    } // proc/cpuinfo opened
	} // i686 extra flags check

    } // _base_arch empty

    return _base_arch;
}

//-----------------------------
// target content access


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
        PMYouPatchPaths paths;
        PMYouPatchInfoPtr patchInfo( new PMYouPatchInfo );

        Url u( "dir://" + ( getRoot() + paths.installDir() ).asString() );
        Pathname path;
        PMError error = patchInfo->readDir( u, path, _patches, false, false );
        if ( error ) {
            E__ << "Error reading patch info for installed patches." << endl;
        }

        _patchesInitialized = true;
    }

    return _patches;
}

//--------------------------------------------------------------------
// target specific functions

void
InstTarget::setBackupPath (const Pathname& path)
{
    _rpmdb->setBackupPath (path);
}

Pathname
InstTarget::getBackupPath ()
{
    return _rpmdb->getBackupPath ();
}

void
InstTarget::createPackageBackups(bool yes)
{
    _rpmdb->createPackageBackups(yes);
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

PMError InstTarget::installPackage (const Pathname& filename, unsigned flags)
{
    return _rpmdb->installPackage(filename,flags?flags:_rpminstflags);
}

PMError InstTarget::installPackages (const std::list<Pathname>& filenames, unsigned flags)
{
    PMError err;
    for(list<Pathname>::const_iterator it= filenames.begin(); it != filenames.end(); ++it)
    {
	err = installPackage(*it,flags);
        if ( err ) break;
    }
    return err;
}

PMError InstTarget::removePackage (const std::string& label, unsigned flags)
{
    return _rpmdb->removePackage (label, flags?flags:_rpminstflags);
}

PMError InstTarget::removePackage (constPMPackagePtr package, unsigned flags)
{
    return _rpmdb->removePackage (package, flags?flags:_rpminstflags);
}

PMError InstTarget::removePackages (const std::list<std::string>& labels, unsigned flags)
{
    PMError err;
    for(list<string>::const_iterator it= labels.begin(); it != labels.end(); ++it)
    {
	err = removePackage(*it,flags);
        if ( err ) break;
    }
    return err;
}

PMError InstTarget::removePackages (const std::list<PMPackagePtr>& packages, unsigned flags)
{
    PMError err;
    for(list<PMPackagePtr>::const_iterator it= packages.begin(); it != packages.end(); ++it)
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

void InstTarget::setRebuildDBProgressCallback(void (*func)(int,void*), void* data)
{
    _rpmdb->setRebuildDBProgressCallback(func,data);
}

bool InstTarget::setInstallationLogfile(const std::string& logfile)
{
    return _rpmdb->setInstallationLogfile(logfile);
}

PMError InstTarget::installPatch( const Pathname &filename )
{
    PMYouPatchPaths paths;

    Pathname dest = getRoot();
    dest += paths.installDir();

    int err = PathInfo::assert_dir( dest );
    if ( err ) {
      E__ << "Can't create " << dest << " (errno: " << err
          << ")" << endl;
      return Error::E_error;
    }

    dest += filename.basename();

    D__ << "in: " << filename << " out: " << dest << endl;

    ifstream in( filename.asString().c_str() );
    if ( in.fail() ) {
        E__ << "Can't read " << filename << endl;
        return Error::E_error;
    }
    ofstream out( dest.asString().c_str() );
    if ( out.fail() ) {
        E__ << "Can't write " << dest << endl;
        return Error::E_error;
    }

    int size = 1000;
    char buf[size];
    while( !in.eof() ) {
      in.read( buf, size );
      out.write( buf, in.gcount() );
    }

    return Error::E_ok;
}

PMError InstTarget::executeScript( const Pathname &scriptname )
{
    ExternalProgram prg( ( "/bin/bash >/dev/null 2>/dev/null " +
                           scriptname.asString() ).c_str() );
    int result = prg.close();
    if ( result != 0 ) {
        E__ << "Script failed. Exit code " << result << endl;
        return Error::E_error;
    }

    return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
// Product related interface
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::mayAccessProducts
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool InstTarget::mayAccessProducts() const
{
  return _proddb->isOpen();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::getProducts
//	METHOD TYPE : const std::list<constInstSrcDescrPtr> &
//
//	DESCRIPTION :
//
const std::list<constInstSrcDescrPtr> & InstTarget::getProducts() const
{
  return _proddb->getProducts();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::isInstalledProduct
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool InstTarget::isInstalledProduct( const constInstSrcDescrPtr & isd_r ) const
{
  return _proddb->isInstalled( isd_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::installProduct
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTarget::installProduct( const constInstSrcDescrPtr & isd_r )
{
  return _proddb->install( isd_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::removeProduct
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTarget::removeProduct( const constInstSrcDescrPtr & isd_r )
{
  return _proddb->remove( isd_r );
}

///////////////////////////////////////////////////////////////////
// Selection related interface
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::mayAccessSelections
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool InstTarget::mayAccessSelections() const
{
  return _seldb->isOpen();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::getSelections
//	METHOD TYPE : const list<PMSelectionPtr> &
//
//	DESCRIPTION :
//
const list<PMSelectionPtr> & InstTarget::getSelections() const
{
  return _seldb->getSelections();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::isInstalledSelection
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool InstTarget::isInstalledSelection( const Pathname & selfile_r ) const
{
  return _seldb->isInstalled( selfile_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::installSelection
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTarget::installSelection( const Pathname & selfile_r )
{
  return _seldb->install( selfile_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::removeSelection
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstTarget::removeSelection( const Pathname & selfile_r )
{
  return _seldb->remove( selfile_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstTarget::getMountPoints
//	METHOD TYPE : std::set<PkgDuMaster::MountPoint>
//
//	DESCRIPTION :
//
std::set<PkgDuMaster::MountPoint> InstTarget::getMountPoints() const
{
  std::set<PkgDuMaster::MountPoint> ret;

  if ( _rootdir.empty() ) {
    INT << "InstTarget not yet initialized (empty rootdir)" << endl;
  } else {
    ifstream procmounts( "/proc/mounts" );

    if ( !procmounts ) {
      WAR << "Unable to read /proc/mounts" << endl;
    } else {

      string prfx;
      if ( _rootdir.asString() != "/" )
	prfx = _rootdir.asString(); // rootdir not /

      while ( procmounts ) {
	string l = stringutil::getline( procmounts );
	if ( !(procmounts.fail() || procmounts.bad()) ) {
	  // data to consume

	  // rootfs 	/ 		rootfs 		rw 0 0
	  // /dev/root 	/ 		reiserfs	rw 0 0
	  // proc 	/proc 		proc		rw 0 0
	  // devpts 	/dev/pts 	devpts		rw 0 0
	  // /dev/hda5 	/boot 		ext2		rw 0 0
	  // shmfs 	/dev/shm 	shm		rw 0 0
	  // usbdevfs 	/proc/bus/usb 	usbdevfs	rw 0 0

	  std::vector<std::string> words;
	  if ( stringutil::split( l, words ) < 3 ) {
	    WAR << "Suspicious entry in /proc/mounts: " << l << endl;
	    continue;
	  }

	  //
	  // Filter devices without '/' (porc,shmfs,..)
	  //
	  if ( words[0].find( '/' ) == string::npos ) {
	    DBG << "Discard mount point : " << l << endl;
	    continue;
	  }

	  //
	  // Filter mountpoints not at or below _rootdir
	  //
	  string mp = words[1];
	  if ( prfx.size() ) {
	    if ( mp.compare( 0, prfx.size(), prfx ) != 0 ) {
	      // mountpoint not below rootdir
	      DBG << "Unwanted mount point : " << l << endl;
	      continue;
	    }
	    // strip prfx
	    mp.erase( 0, prfx.size() );
	    if ( mp.empty() ) {
	      mp = "/";
	    } else if ( mp[0] != '/' ) {
	      // mountpoint not below rootdir
	      DBG << "Unwanted mount point : " << l << endl;
	      continue;
	    }
	  }

	  //
	  // Filter cdrom
	  //
	  if ( words[2] == "iso9660" ) {
	    DBG << "Discard cdrom : " << l << endl;
	    continue;
	  }

	  //
	  // Filter some common unwanted mountpoints
	  //
	  char * mpunwanted[] = {
	    "/mnt", "/media", "/mounts", "/floppy", "/cdrom",
	    "/suse", "/var/tmp", "/var/adm/mount", "/var/adm/YaST",
	    /*last*/0/*entry*/
	  };

	  char ** nomp = mpunwanted;
	  for ( ; *nomp; ++nomp ) {
	    string pre( *nomp );
	    if ( mp.compare( 0, pre.size(), pre ) == 0 // mp has prefix pre
		 && ( mp.size() == pre.size() || mp[pre.size()] == '/' ) ) {
	      break;
	    }
	  }
	  if ( *nomp ) {
	    DBG << "Filter mount point : " << l << endl;
	    continue;
	  }


	  //
	  // statvfs (full path!) and get the data
	  //
	  struct statvfs sb;
	  if ( statvfs( words[1].c_str(), &sb ) != 0 ) {
	    WAR << "Unable to statvfs(" << words[1] << "); errno " << errno << endl;
	    ret.insert( PkgDuMaster::MountPoint( mp ) );
	  } else {
	    ret.insert( PkgDuMaster::MountPoint( mp,
						 sb.f_bsize,
						 FSize(sb.f_blocks)*sb.f_bsize,
						 FSize(sb.f_blocks - sb.f_bfree)*sb.f_bsize ) );

	  }
	}
      }
      DBG << ret << endl;
    }
  }

  return ret;
}

