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

#include <y2pm/InstTargetPtr.h>		// pointer to self
#include <y2pm/MediaAccessPtr.h>		// physical media access class
//#include <y2pm/InstDescr.h>		// target description
//#include <y2pm/InstData.h>		// target content

//#include <y2pm/PMSolvable.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMYouPatchPtr.h>

#include <y2pm/RpmDb.h> // InstTarget is tied to RpmDb atm

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstTarget
/**
 *
 **/
class InstTarget: virtual public Rep {
  REP_BODY(InstTarget);

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

        /**
	 * default error class
	 **/
        typedef InstTargetError Error;

    protected:

#if 0

    /**
     * direct media access
     * these denote the source medias actually in use
     * on the target. They only share the URL with
     * the installation sources but run in a different
     * environment and are attached to different directories
     */
    std::list <MediaAccess> *_medias;

    /**
     * description of target
     * implemented as a list since it basically is a copy
     * of all source descriptions of sources which are
     * installed on the target.
     */
    std::list <InstDescr> *_descrs;

    /**
     * content of media
     * this describes the content of the target
     */
    InstData *_data;
#endif

    public:
	/**
	 * constructor
	 * @param rootpath, path to root ("/") of target system
	 * Usually "/" if the InstTarget object is actually running
	 * inside the target. But might be "/mnt" during installation
	 * (running in inst-sys) or "/whatever" if installing into
	 * a directory
	 */
	InstTarget ( const std::string & rootpath );

	/**
	 * destructor
	 */
	~InstTarget();

    public:

	/**
	 * Initialize target system. Takes necessary action to make installation,
	 * queries etc possible.
	 *
	 * @param createnew create a new (e.g. rpm-) database if none is present.
	 * It is safe to alwas use true here.
	 * */
	PMError init( bool createnew = true );

	/**
	 * bring target into a clean state e.g. by calling rpm --rebuilddb
	 * */
	PMError bringIntoCleanState();


	//-----------------------------
	// general functions

	/**
	 * clean up, e.g. remove all caches
	 */
	bool Erase();

	/**
	 * @return description of Installation target
	 */
    //    const InstDescr **getDescriptions() const;

	//-----------------------------
	// target content access

	/**
	 * return the number of selections on this source
	 */
	int numSelections() const;

	/**
	 * return the number of packages on this source
	 */
	int numPackages() const;

	/**
	 * return the number of patches on this source
	 */
	int numPatches() const;

	/**
	 * generate PMPackage objects for each Item on the source
	 *
	 * @param pkglist where to store newly created PMPackages
	 * */
	PMError getPackages (std::list<PMPackagePtr>& pkglist);

	/**
	 * generate PMSelection objects for each Item on the source
	 *
	 * @param sellist where to store newly created PMSelections
	 * */
	PMError getSelections (std::list<PMSelectionPtr>& sellist);

	/**
	 * generate PMYouPatch objects for each Item on the source
	 *
	 * @param youpatchlist where to store newly created PMYouPatches
	 * */
	PMError getYOUPatches (std::list<PMYouPatchPtr>& youpatchlist);

	//-----------------------------
	// package install / remove

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
	PMError installPackage (const std::string& filename, unsigned flags = 0);
	
	/**
	 * install packages to the target
	 *
	 * @param filename list of filenames of packages to install
	 * @param flags which flags to use, default flags are used if 0
	 * */
	PMError installPackages (const std::list<std::string>& filenames, unsigned flags = 0);

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
	
	/**
	 * remove package
	 *
	 * @see removePackage
	 * */
	PMError removePackages(const std::list<std::string>& labels, unsigned flags = 0);

	/**
	 * set callback function for reporting progress of package
	 * installation
	 *
	 * @param func callback function, must accept double as argument
	 * @param data arbitrary data to pass when function is called
	 * */
	void setPackageInstallProgressCallback(void (*func)(double,void*), void* data);

	std::ostream & dumpOn( std::ostream & str ) const;

	/**
	 * @return destination root directory of target system
	 * */
	const std::string& getRoot() const;


	// TODO: more function, like df, du etc.

    private:

	/** parameters to use on installation/update
	 * */
	unsigned _rpminstflags;

	/** parameters to use on removal
	 * */
	unsigned _rpmremoveflags;

	/**
	 * The name of the install root.
	 */
	Pathname _rootdir;


	/** rpm database */
	RpmDbPtr _rpmdb;

    private:
	// forbidden
	InstTarget ();
};

///////////////////////////////////////////////////////////////////

#endif // InstTarget_h
