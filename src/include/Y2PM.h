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

  File:       Y2PM.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Core class providing access to all components of the
  Package Management creating them on demand.

/-*/
#ifndef Y2PM_h
#define Y2PM_h

#include <iosfwd>
#include <string>
#include <list>
#include <y2util/LangCode.h>
#include <y2util/FSize.h>
#include <y2pm/PkgArch.h>
#include <y2pm/InstSrcManager.h>

class InstTarget;
class PMPackageManager;
class PMYouPatchManager;
class PMSelectionManager;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : Y2PM
/**
 * Core class providing access to all components of the
 * Package Management creating them on demand.
 **/
class Y2PM {

  private:

    ///////////////////////////////////////////////////////////////////
    // global settings
    ///////////////////////////////////////////////////////////////////

    static Pathname _instTarget_rootdir;

    static Pathname _system_rootdir;

    static bool _cache_to_ramdisk;

    // preferred locale (i.e. user interface and default system locale)
    // used in retrieval of locale-dependant data
    static LangCode _preferred_locale;

    // list of requested locales (must include _preferred_locale !)
    // used when selecting/deselecting locale dependant
    // packages of selections
    static std::list<LangCode> _requested_locales;

    // the current base architecture of the target
    static PkgArch _base_arch;
    // the current list of allowed architectures
    static std::list<PkgArch> _allowed_archs;

  private:

    ///////////////////////////////////////////////////////////////////
    // components provided
    ///////////////////////////////////////////////////////////////////

    static InstTarget * _instTarget;

    static InstSrcManager * _instSrcManager;

    static PMPackageManager * _packageManager;

    static PMSelectionManager * _selectionManager;

    static PMYouPatchManager * _youPatchManager;

  public:

    /**
     * Access to the preferred locale
     **/
    static const LangCode & getPreferredLocale () { return _preferred_locale; }
    static void setPreferredLocale (const LangCode & preferred_locale) { _preferred_locale = preferred_locale; }

    /**
     * Access to the requested localed
     **/
    static const std::list<LangCode> & getRequestedLocales () { return _requested_locales; }
    static void setRequestedLocales (const std::list<LangCode> & requested_locales) { _requested_locales = requested_locales; }

    /**
     * Access to the (target) base architecture
     **/
    static PkgArch baseArch(void);

    /**
     * Access to the list of allowed architectures
     **/
    static const std::list<PkgArch> & allowedArchs(void) { return _allowed_archs; }
    static void setAllowedArchs(const std::list<PkgArch>& allowed_archs) { _allowed_archs = allowed_archs; }

    /**
     * The local machine's rootdir. "/" if running from system.
     * Something like "/mnt" during installation/update or "" (empty),
     * if system root was not yet mounted.
     *
     * Not necessarily the same as _instTarget_rootdir (e.g. if we'd install into a
     * local directory).
     **/
    //static const Pathname & systemRoot() { return _system_rootdir; }

    /**
     *
     **/
    //static bool haveSystem() { return ! _system_rootdir.empty(); }

    /**
     * If false, root is ramdisk, and system to install is (or will be) mounted
     * below _system_rootdir. Need to know this e.g. in InstSrc to determine,
     * wheter to write a chache on 'enable' or to wait until system to be installed
     * is available below _system_rootdir.
     **/
    static bool runningFromSystem()       { return( _system_rootdir == "/" ); }
    static void setNotRunningFromSystem() {_system_rootdir = Pathname(); }

    /**
     * If false, InstSrc will not create a data cache in ramdisk, if not
     * running from system.
     **/
    static bool cacheToRamdisk()                      { return _cache_to_ramdisk; }
    static void setCacheToRamdisk( const bool val_r ) { _cache_to_ramdisk = val_r; }

  public:

    /**
     * Access to the installation target
     * if do_start == true, start the target at root
     * and also !!START THE packageManager!!
     **/
    static InstTarget & instTarget(bool do_start = false, Pathname root = Pathname("/"));

    /**
     * Special call to create the instSrcManager (if it does not
     * already exist), but prevent autoenabling of any InstSrc'es
     * found in the cache.
     *
     * Returns true, if the call succeeded. Otherwise the instSrcManager
     * has already been constructed, and may or may not contain enabled
     * InstSrc'es.
     **/
    static bool noAutoInstSrcManager();

    /**
     * Access to the installation source manager
     **/
    static InstSrcManager & instSrcManager();

    /**
     * Access to the Package manager
     **/
    static PMPackageManager & packageManager();

    /**
     * Access to the Selection manager
     **/
    static PMSelectionManager & selectionManager();

    /**
     * Access to the YouPatch  manager
     **/
    static PMYouPatchManager & youPatchManager();

  private:

    ///////////////////////////////////////////////////////////////////
    // CallBacks
    ///////////////////////////////////////////////////////////////////

	/**
	 * callbacks and their data
	 * */
	struct CallBacks
	{
	    CallBacks();

	    /**
	     * called right before package 'name' is provided
	     * */
	    void (*_provide_start_func)(const std::string& server,
					const FSize& size, bool remote, void* data);
	    void* _provide_start_data;

	    /**
	     * called multiple times during package providal, 'progress' is the
	     * already provided percentage
	     * */
	    void (*_provide_progress_func)(int progress, void* data);
	    void* _provide_progress_data;

	    /**
	     * called after package 'name' was provided
	     * */
	    void (*_provide_done_func)(PMError err, const std::string& errdata, void* data);
	    void* _provide_done_data;

	    /**
	     * called right before package 'name' is installed or deleted
	     * should return 'true' to continue, 'false' to abort
	     * */
	    bool (*_package_start_func)(const std::string& name,
						     const std::string& summary,
						     const FSize& size, bool is_delete, void* data);
	    void* _package_start_data;

	    /**
	     * called multiple times during package installation, 'progress' is the
	     * already installed percentage
	     * */
	    void (*_package_progress_func)(int progress, void* data);
	    void* _package_progress_data;

	    /**
	     * called after package 'name' got installed or deleted
	     * */
	    void (*_package_done_func)(PMError err, const std::string& errdata, void* data);
	    void* _package_done_data;

	    /**
	     * called multiple times during rpm rebuilddb, 'progress' is the progress 0..100
	     * */
	    void (*_rebuilddb_progress_func)(int progress, void* data);
	    void* _rebuilddb_progress_data;

	    /**
	     * called when switching the source or media number during package commit (install loop)
	     * informal callback for user interface, no user interaction necessary
	     * */
	    void (*_source_change_func)(InstSrcManager::ISrcId srcid, int medianr, void* data);
	    void* _source_change_data;

	};

	static CallBacks _callbacks;

    public:

	/**
	 * called right before package 'name' is provided
	 * */
	static void setProvideStartCallback(void (*func)(const std::string& name, const FSize&, bool, void*), void* data);

	/**
	 * called while package providal is in progress
	 * */
	static void setProvideProgressCallback(void (*func)(int percent, void*), void* data);

	/**
	 * called right after package 'name' was provided
	 * */
	static void setProvideDoneCallback(void (*func)(PMError error, const std::string& reason, void*), void* data);

	/**
	 * called right before package 'name' is installed or deleted
	 * */
	static void setPackageStartCallback(bool (*func)(const std::string& name, const std::string& summary, const FSize& size, bool is_delete, void*), void* data);

	/**
	 * called multiple times during package installation, 'progress' is the
	 * already installed percentage
	 * */
	static void setPackageProgressCallback(void (*func)(int percent, void*), void* data);

	/**
	 * called after package 'name' got installed or deleted
	 * */
	static void setPackageDoneCallback(void (*func)(PMError err, const std::string& reason, void*), void* data);

	/**
	 * called multiple times during rpm rebuilddb, 'progress' is the
	 * rebuild progress
	 * */
	static void setRebuildDBProgressCallback(void (*func)(int percent, void*), void* data);

	/**
	 * called when switching sources during package commit (install loop)
	 * informal callback for user interface, no user interaction necessary
	 * */
	static void setSourceChangeCallback(void (*func)(InstSrcManager::ISrcId srcid, int medianr, void*), void* data);

	/**
	 * package deletion/installation main loop
	 * deletes/installs all packages currently marked in packageManager()
	 * if media_nr == 0, install everything regardless of media nr
	 * if media_nr > 0, install only from this media nr
	 * return list of failed package names in errors_r
	 * return list of unavailable packages (due to InstSrc errors or wrong media nr) in remaining_r
	 * return list of unavailable source packages (due to InstSrc errors or wrong media nr) in srcremaining_r
	 * return number of sucessfully installed packages
	 * */
	static int commitPackages (unsigned int media_nr, std::list<std::string>& errors_r,
		std::list<std::string>& remaining_r, std::list<std::string>& srcremaining_r,
		InstSrcManager::ISrcIdList installrank = InstSrcManager::ISrcIdList());

	/**
	 * install a single rpm file
	 * uses callbacks !
	 */
	static PMError installFile (const Pathname& path);

	/**
	 * remove a single package by name
	 * uses callbacks !
	 */
	static PMError removePackage (const std::string& pkgname);
};

///////////////////////////////////////////////////////////////////

#endif // Y2PM_h
