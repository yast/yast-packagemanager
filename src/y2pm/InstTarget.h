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

   File:       InstTarget.h

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

		Additionally, the InstTarget can also change packages,
		selections, and patches, by installing or removing them.

/-*/
#ifndef InstTarget_h
#define InstTarget_h

#include <iosfwd>
#include <list>
#include <set>
#include <string>

#include <y2util/Pathname.h>
#include <y2util/FSize.h>

#include <y2pm/PMError.h>

#include <y2pm/MediaAccessPtr.h>	// physical media access class
#include <y2pm/PkgArch.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMYouPatchPtr.h>
#include <y2pm/InstData.h>	 // InstTarget implements InstData

#include <y2pm/RpmDb.h>		      // InstTarget is tied to RpmDb atm
#include <y2pm/InstTargetProdDBPtr.h> // Installed Products
#include <y2pm/InstSrcDescrPtr.h>
#include <y2pm/InstTargetSelDBPtr.h>  // Installed Selections
#include <y2pm/PkgDu.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstTarget
/**
 *
 **/
class InstTarget: public CountedRep, public InstData {
    REP_BODY(InstTarget);

  public:

    /**
     * Default error class
     **/
    typedef class InstTargetError Error;

    ///////////////////////////////////////////////////////////////////
    // General interface
    ///////////////////////////////////////////////////////////////////

  private:

    /**
     * The name of the install root.
     **/
    Pathname _rootdir;

    /**
     * Rpm database
     ***/
    RpmDbPtr _rpmdb;

    /**
     * Products database
     **/
    InstTargetProdDBPtr _proddb;

    /**
     * Selection database
     **/
    InstTargetSelDBPtr _seldb;

  private:

    friend class Y2PM;
    // no parameters here since Y2PM creates it on first access
    // and there's no way to pass parameters -> see init()
    InstTarget();
    ~InstTarget();

    /**
     * Initialize target system. Takes necessary action to make installation,
     * queries etc possible.
     *
     * @param rootpath, path to root ("/") of target system
     * Usually "/" if the InstTarget object is actually running
     * inside the target. But might be "/mnt" during installation
     * (running in inst-sys) or "/whatever" if installing into
     * a directory
     **/
    PMError init( const Pathname & rootpath );

  public:

    /**
     * Determine target system architecture.
     **/
    static PkgArch baseArch();

    /**
     * @return The name of the install root. Empty path if not
     * initialized.
     **/
    const Pathname & rootdir() const { return _rootdir; }

    /**
     * @return Whether the target is initialized.
     **/
    bool initialized() const { return( ! _rootdir.empty() ); }

    /**
     * @return Whether some data on target changed and should be reread.
     * (e.g. rpm database after install/delete) (<B>NOTE:</B> returns
     * false, if not initialized).
     **/
    bool needsUpdate() const;

    /**
     * bring target into a clean state e.g. by calling rpm --rebuilddb
     **/
    PMError bringIntoCleanState();

    /**
     * generate PMPackage objects for each Item on the source/target
     * @return list of PMPackagePtr on this source
     **/
    virtual const std::list<PMPackagePtr> & getPackages() const;

    /**
     * generate PMYouPatch objects for each patch on the source
     * @return list of PMYouPatchPtr on this source
     **/
    virtual const std::vector<PMYouPatchPtr> & getPatches() const;

    ///////////////////////////////////////////////////////////////////
    // Package related interface
    ///////////////////////////////////////////////////////////////////

  private:

    /** parameters to use on installation/update
     **/
    unsigned _rpminstflags;

    /** parameters to use on removal
     **/
    unsigned _rpmremoveflags;

  public:

    /**
     * Installation options like force, nodeps ...
     * Has to be moved from RpmDB to here
     *
     * @see RpmDB::RpmInstFlag
     **/
    typedef enum RpmDb::RpmInstFlag InstFlag;

    /**
     * Bits of possible package corruptions
     * Has to be moved from RpmDB to here
     *
     * @see RpmDb::checkPackageResult
     **/
    typedef enum RpmDb::checkPackageResult checkPackageResult;

    //-----------------------------
    // package install / remove

    /**
     * set log file for installation
     *
     * @return false if opening the file failed
     **/
    bool setInstallationLogfile(const std::string& logfile);

    /**
     * set backup dir for rpm config files
     *
     * @param const Pathname
     **/
    void setBackupPath (const Pathname& path);

    /**
     * whether to create package backups during install or
     * removal
     *
     * @param yes true or false
     **/
    void createPackageBackups(bool yes);

    /**
     * get backup dir for rpm config files
     *
     **/
    Pathname getBackupPath (void);

    /**
     * set parameters to use on installation/update
     *
     * @param flags which parameters to use by default, eg.  * RPMINST_NODOCS|RPMINST_NOSCRIPTS
     **/
    void setPkgInstFlags(unsigned flags);

    /**
     * get current package installation parameters
     * get 'rpm' parameters currently used for package installation
     **/
    unsigned getPkgInstFlags() const;

    /**
     * set parameters to use on removal of packages
     *
     * @param flags which parameters to use by default, eg.  * RPMINST_FORCE|RPMINST_NODEPS
     **/
    void setPkgRemoveFlags(unsigned flags);

    /**
     * get current package removal parameters
     * get 'rpm' parameters currently used for package removal
     **/
    unsigned getPkgRemoveFlags() const;

    /**
     * install single Package to the target
     *
     * @param filename filename of package to install
     * @param flags which flags to use, default flags are used if 0
     **/
    PMError installPackage (const Pathname& filename, unsigned flags = 0);

    /**
     * install multiple packages to the target
     *
     * @param filename list of filenames of packages to install
     * @param flags which flags to use, default flags are used if 0
     **/
    PMError installPackages (const std::list<Pathname>& filenames, unsigned flags = 0);

    /**
     * remove single package
     *
     * @param name_r Name of the rpm package to remove.
     * @param flags which flags to use, default flags are used if 0
     **/
    PMError removePackage(const std::string & name_r, unsigned flags = 0);
    PMError removePackage(constPMPackagePtr package, unsigned flags = 0);

    /**
     * remove multiple packages
     *
     * @see removePackage
     **/
    PMError removePackages(const std::list<std::string>& names_r, unsigned flags = 0);
    PMError removePackages(const std::list<PMPackagePtr>& packages, unsigned flags = 0);

  public:
    ///////////////////////////////////////////////////////////////////
    //
    // RPM database public keys
    //
    ///////////////////////////////////////////////////////////////////

    /**
     * Import ascii armored public key in file pubkey_r into RPM database
     **/
    PMError importPubkey( const Pathname & pubkey_r ) { return _rpmdb->importPubkey( pubkey_r ); }

    /**
     * Import ascii armored public key keyname_r exported by keyring_r into RPM database
     **/
    PMError importPubkey( const Pathname & keyring_r, const std::string & keyname_r ) { return _rpmdb->importPubkey( keyring_r, keyname_r ); }

    /**
     * Return the editions of all installed public keys in RPM database.
     **/
    std::set<PkgEdition> pubkeys() const { return _rpmdb->pubkeys(); }

  public:
    ///////////////////////////////////////////////////////////////////
    //
    // Direct RPM database retrieval forwarded to RpmDb
    //
    ///////////////////////////////////////////////////////////////////

    /**
     * Return true if at least one package owns a certain file.
     **/
    bool hasFile( const std::string & file_r ) const { return _rpmdb->hasFile( file_r ); }

    /**
     * Return true if at least one package provides a certain tag.
     **/
    bool hasProvides( const std::string & tag_r ) const { return _rpmdb->hasProvides( tag_r ); }

    /**
     * Return true if at least one package requires a certain tag.
     **/
    bool hasRequiredBy( const std::string & tag_r ) const { return _rpmdb->hasRequiredBy( tag_r ); }

    /**
     * Return true if at least one package conflicts with a certain tag.
     **/
    bool hasConflicts( const std::string & tag_r ) const { return _rpmdb->hasConflicts( tag_r ); }

    /**
     * Return true if package is installed.
     **/
    bool hasPackage( const PkgName & name_r ) const { return _rpmdb->hasPackage( name_r ); }

    /**
     * Hack to let InstTarget lookup required and conflicting file relations.
     **/
    void traceFileRel( const PkgRelation & rel_r ) { _rpmdb->traceFileRel( rel_r ); }

    ///////////////////////////////////////////////////////////////////

    /**
     * Deprecated old style
     **/
    bool isProvided (const std::string& tag) { return hasProvides(tag); }
    /**
     * Deprecated old style
     **/
    bool isInstalled (const std::string& name) { return hasPackage(PkgName(name)); }

    ///////////////////////////////////////////////////////////////////
    // Patch related interface
    ///////////////////////////////////////////////////////////////////
  private:

      mutable std::vector<PMYouPatchPtr> _patches;
      mutable bool _patchesInitialized;

  public:

    /**
     * Install patch. This function does not install any packages. It just
     * registers the patch as installed.
     *
     * @param filename Filename of patch to be installed.
     **/
    PMError installPatch( const Pathname &filename );

    /**
     * Execute script.
     **/
    PMError executeScript( const Pathname &scriptname );

    ///////////////////////////////////////////////////////////////////
    // Product related interface
    ///////////////////////////////////////////////////////////////////
  public:

    /**
     * Return true if products database is accessible.
     **/
    bool mayAccessProducts() const;

    /**
     * Return list of installed Products (reverse sorted by
     * installation time).
     **/
    const std::list<constInstSrcDescrPtr> & getProducts() const;

    /**
     * Return true if Product isd_r is installed. Actually if
     * an InstSrcDescr with same product name and version
     * is installed.
     **/
    bool isInstalledProduct( const constInstSrcDescrPtr & isd_r ) const;

    /**
     * Install Product. That's nothing but copying the InstSrcDescr
     * into the local ProductDB.
     **/
    PMError installProduct( const constInstSrcDescrPtr & isd_r );

    /**
     * Removes Product from ProductDB. I.e. an installed Product with
     * the same product name and version as isd_r.
     **/
    PMError removeProduct( const constInstSrcDescrPtr & isd_r );


    ///////////////////////////////////////////////////////////////////
    // Selection related interface
    ///////////////////////////////////////////////////////////////////
  public:

    /**
     * Return true if selection database is accessible.
     **/
    bool mayAccessSelections() const;

    /**
     * generate PMSelection objects for each selection on the source
     * @return list of PMSelectionPtr on this source
     **/
    virtual const std::list<PMSelectionPtr> & getSelections() const;

    /**
     * Return true if Selection selfile_r is installed. Actually
     * only checks whether a file named $(basename selfile_r)
     * is installed.
     **/
    bool isInstalledSelection( const Pathname & selfile_r ) const;

    /**
     * Install Selection. That's nothing but copying the Selection
     * file into the local SelectionDB.
     **/
    PMError installSelection( const Pathname & selfile_r );

    /**
     * Removes a file named $(basename selfile_r) from the
     * SelectionDB.
     **/
    PMError removeSelection( const Pathname & selfile_r );


    ///////////////////////////////////////////////////////////////////
    //
    // Disk usage calculation.
    //
    ///////////////////////////////////////////////////////////////////
  public:

    /**
     * Fallback for Packagemanager in case no one initialized the
     * 'df' info for disk usage calculation. Returns an empty set
     * if <code>_rootdir</code> is not yet set.
     *
     * <table border=2>
     * <tr bgcolor="#BEEAE0"><th colspan=6><code>/proc/mounts</code></tr>
     * <tr bgcolor="#BEEAE0"><th>Filesystem<th>Mounted on<th>Type<th>Options<th>&nbsp;<th>&nbsp;</tr>
     * <tr><td>rootfs<td>/<td>rootfs<td>rw<td>0<td>0</tr>
     * <tr bgcolor="#BEEAE0"><td>/dev/root<td>/<td>reiserfs<td>rw<td>0<td>0</tr>
     * <tr><td>proc<td>/proc<td>proc<td>rw<td>0<td>0</tr>
     * <tr><td>devpts<td>/dev/pts<td>devpts<td>rw<td>0<td>0</tr>
     * <tr bgcolor="#BEEAE0"><td>/dev/hda5<td>/boot<td>ext2<td>rw<td>0<td>0</tr>
     * <tr><td>shmfs<td>/dev/shm<td>shm<td>rw<td>0<td>0</tr>
     * <tr><td>usbdevfs<td>/proc/bus/usb<td>usbdevfs<td>rw<td>0<td>0</tr>
     * <tr><td>automount(pid3743)<td>/suse<td>autofs<td>rw<td>0<td>0</tr>
     * <tr bgcolor="#BEEAE0"><td>wotan:/real-home/ma<td>/suse/ma<td>nfs<td>rw,nosuid,v2,rsize=8192,wsize=8192,hard,intr,udp,nolock,addr=wotan<td>0<td>0</tr>
     * </table>
     *
     * Taken into account are entries,
     * <ul>
     * <li>whose filesystem contains a <code>'/'</code>
     * <li>whose mountpoint is at or below the current <code>_rootdir</code>
     * </ul>
     **/
    std::set<PkgDuMaster::MountPoint> getMountPoints() const;
};

///////////////////////////////////////////////////////////////////

#endif // InstTarget_h
