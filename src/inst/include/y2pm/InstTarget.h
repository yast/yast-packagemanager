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
#include <string>

#include <y2util/Pathname.h>

#include <y2pm/PMError.h>

#include <y2pm/MediaAccessPtr.h>	// physical media access class
#include <y2pm/PkgArch.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMYouPatchPtr.h>
#include <y2pm/InstTargetError.h>
#include <y2pm/InstData.h>	 // InstTarget implements InstData

#include <y2pm/RpmDb.h>		      // InstTarget is tied to RpmDb atm
#include <y2pm/InstTargetProdDBPtr.h> // Installed Products
#include <y2pm/InstSrcDescrPtr.h>
#include <y2pm/InstTargetSelDBPtr.h>  // Installed Selections

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstTarget
/**
 *
 **/
class InstTarget: virtual public Rep, public InstData {
    REP_BODY(InstTarget);

    public:

        /**
	 * default error class
	 **/
        typedef InstTargetError Error;


      ///////////////////////////////////////////////////////////////////
      // General interface
      ///////////////////////////////////////////////////////////////////

    private:

        friend class Y2PM;
	// no parameters here since Y2PM creates it on first access
	// and there's no way to pass parameters -> see init()
        InstTarget();
        ~InstTarget();

	PkgArch _base_arch;

	/**
	 * The name of the install root.
	 */
	Pathname _rootdir;


	/** rpm database */
	RpmDbPtr _rpmdb;

    public:

	/**
	 * Initialize target system. Takes necessary action to make installation,
	 * queries etc possible.
	 *
	 * @param rootpath, path to root ("/") of target system
	 * Usually "/" if the InstTarget object is actually running
	 * inside the target. But might be "/mnt" during installation
	 * (running in inst-sys) or "/whatever" if installing into
	 * a directory
	 * @param createnew create a new (e.g. rpm-) database if none is present.
	 * It is safe to alwas use true here.
	 * */
	PMError init( const Pathname & rootpath, bool createnew = true );

	/**
	 * @return destination root directory of target system
	 * */
	const std::string& getRoot() const;

	/**
	 * determine target system architecture
	 */
	PkgArch baseArch ();

	/**
	 * bring target into a clean state e.g. by calling rpm --rebuilddb
	 * */
	PMError bringIntoCleanState();


	//-----------------------------
	// general functions

	/**
	 * set log file for installation
	 *
	 * @return false if opening the file failed
	 * */
	bool setInstallationLogfile(const std::string& logfile);

	/**
	 * clean up, e.g. remove all caches
	 */
	bool Erase();

	//-----------------------------
	// InstData interface
	// target content access

	/**
	 * generate PMPackage objects for each Item on the source/target
	 * @return list of PMPackagePtr on this source
	 * */
	virtual const std::list<PMPackagePtr>& getPackages (void) const;

	/**
	 * generate PMYouPatch objects for each patch on the source
	 * @return list of PMYouPatchPtr on this source
	 */
	virtual const std::list<PMYouPatchPtr>& getPatches (void) const;


      ///////////////////////////////////////////////////////////////////
      // Package related interface
      ///////////////////////////////////////////////////////////////////

    private:

	/** parameters to use on installation/update
	 * */
	unsigned _rpminstflags;

	/** parameters to use on removal
	 * */
	unsigned _rpmremoveflags;

    public:

	/**
	 * Installation options like force, nodeps ...
	 * Has to be moved from RpmDB to here
	 *
	 * @see RpmDB::RpmInstFlag
	 * */
	typedef enum RpmDb::RpmInstFlag InstFlag;

	/**
	 * Bits of possible package corruptions
	 * Has to be moved from RpmDB to here
	 *
	 * @see RpmDb::checkPackageResult
	 * */
	typedef enum RpmDb::checkPackageResult checkPackageResult;

	//-----------------------------
	// package install / remove

	/**
	 * set backup dir for rpm config files
	 *
	 * @param const Pathname
	 * */
	void setBackupPath (const Pathname& path);

	/**
	 * whether to create package backups during install or
	 * removal
	 *
	 * @param yes true or false
	 * */
	void createPackageBackups(bool yes);

	/**
	 * get backup dir for rpm config files
	 *
	 * */
	Pathname getBackupPath (void);

	/**
	 * set parameters to use on installation/update
	 *
	 * @param flags which parameters to use by default, eg.  * RPMINST_NODOCS|RPMINST_NOSCRIPTS
	 * */
	void setPkgInstFlags(unsigned flags);

	/**
	 * get current package installation parameters
	 * get 'rpm' parameters currently used for package installation
	 */
	unsigned getPkgInstFlags() const;

	/**
	 * set parameters to use on removal of packages
	 *
	 * @param flags which parameters to use by default, eg.  * RPMINST_FORCE|RPMINST_NODEPS
	 * */
	void setPkgRemoveFlags(unsigned flags);

	/**
	 * get current package removal parameters
	 * get 'rpm' parameters currently used for package removal
	 */
	unsigned getPkgRemoveFlags() const;

	/**
	 * install single Package to the target
	 *
	 * @param filename filename of package to install
	 * @param flags which flags to use, default flags are used if 0
	 */
	PMError installPackage (const Pathname& filename, unsigned flags = 0);

	/**
	 * install multiple packages to the target
	 *
	 * @param filename list of filenames of packages to install
	 * @param flags which flags to use, default flags are used if 0
	 * */
	PMError installPackages (const std::list<Pathname>& filenames, unsigned flags = 0);

	/**
	 * remove single package
	 *
	 * @param label label of the rpm package to remove. always specify the
	 * full label (name-version-release) as multiple packages with same
	 * name but different versions could be installed
	 * @param flags which flags to use, default flags are used if 0
	 *
	 * */
	PMError removePackage(const std::string& label, unsigned flags = 0);
	PMError removePackage(constPMPackagePtr package, unsigned flags = 0);

	/**
	 * remove multiple packages
	 *
	 * @see removePackage
	 * */
	PMError removePackages(const std::list<std::string>& labels, unsigned flags = 0);
	PMError removePackages(const std::list<PMPackagePtr>& packages, unsigned flags = 0);

	/**
	 * set callback function for reporting progress of package
	 * installation
	 *
	 * @param func callback function, must accept int as argument for percentage
	 * @param data arbitrary data to pass when function is called
	 * */
	void setPackageInstallProgressCallback(void (*func)(int,void*), void* data);

	/**
	 * set callback function for reporting progress of rebuildding the
	 * package database (rpm --rebuilddb)
	 *
	 * @param func callback function, must accept int as argument for percentage
	 * @param data arbitrary data to pass when function is called
	 * */
	void setRebuildDBProgressCallback(void (*func)(int,void*), void* data);

	/**
	 * query system for provided tag (rpm -q --whatprovides)
	 */
	bool isProvided (const std::string& tag) { return _rpmdb->isProvided (tag); }

	/**
	 * query system for installed package (rpm -q)
	 */
	bool isInstalled (const std::string& name) { return _rpmdb->isInstalled (name); }

	/**
	 * query system for package the given file belongs to
	 * (rpm -qf)
	 * report 'name-version-release' if full_name, else 'name' only
	 */
	std::string belongsTo (const Pathname& name, bool full_name = true) { return _rpmdb->belongsTo (name, full_name); }


      ///////////////////////////////////////////////////////////////////
      // Patch related interface
      ///////////////////////////////////////////////////////////////////

    private:
        mutable std::list<PMYouPatchPtr> _patches;
        mutable bool _patchesInitialized;

    public:
        /**
         * Install patch. This function does not install any packages. It just
         * registers the patch as installed.
         *
         * @param filename Filename of patch to be installed.
         */
        PMError installPatch( const Pathname &filename );

        /**
         * Execute script.
         */
        PMError executeScript( const Pathname &scriptname );

	// TODO: more function, like df, du etc.



      ///////////////////////////////////////////////////////////////////
      // Product related interface
      ///////////////////////////////////////////////////////////////////


    private:
      /**
       * Selection database
       **/
      InstTargetProdDBPtr _proddb;

    public:

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

    private:
      /**
       * Selection database
       **/
      InstTargetSelDBPtr _seldb;

    public:

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
};

///////////////////////////////////////////////////////////////////

#endif // InstTarget_h
