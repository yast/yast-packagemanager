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

#include <y2pm/PMError.h>
#include <y2pm/PkgArch.h>
#include <y2pm/InstSrcManager.h>

class InstTarget;
class PMPackageManager;
class PMYouPatchManager;
class PMSelectionManager;

class ProgressCounter;

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

    static void cleanupAtExit();

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
     * Access to the installation target. Target is uninitialized
     * untill @ref instTargetInit and after @ref instTargetClose.
     **/
    static InstTarget & instTarget();

    /**
     * Initialize the installation target to use the system located
     * below root_r.
     * If the installation target is already initialized, a provided
     * root_r must match the root path in use, otherwise an error is
     * returned.
     * If no root_r (or an empty path) is provided, an already initialized
     * target is used, otherwise it's initialized to use "/".
     *
     * If the installation target actually gets initialized, packages
     * and selections are loaded into already existing managers. Otherwise
     * managers will retrieve their data as they are created.
     *
     * Use @ref instTargetUpdate if you want to make shure, that
     * installation target and managers exist and are up to date.
     **/
    static PMError instTargetInit( Pathname root_r = "" );

     /**
      * Make shure, that installation target is initialized
      * and managers exist. Check whether databases on target
      * need to be reread, and load the updated content into
      * the managers.
      *
      * <B>NOTE:</B> If the installation target has to use some
      * other location than "/", make shure you explicitly call
      * @ref instTargetInit before.
      **/
    static PMError instTargetUpdate();

     /**
      * Shutdown access to the installation target. Provided packages
      * and selections will be withdrawn from existing managers.
      **/
    static PMError instTargetClose();


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

    /**
     * [package/selectionManager] Save current selection
     **/
    static void packageSelectionSaveState();
    /**
     * [package/selectionManager] Restore previously saved selection.
     **/
    static bool packageSelectionRestoreState();
    /**
     * [package/selectionManager] Return true if current selection differs from previously saved selection.
     **/
    static bool packageSelectionDiffState();
    /**
     * [package/selectionManager] Forgett a previously saved selection.
     **/
    static void packageSelectionClearSaveState();

  public:

    /**
     * package deletion/installation main loop
     * deletes/installs all packages currently marked in packageManager()
     * if media_nr == 0, install everything regardless of media nr
     * if media_nr > 0, install only from this media nr
     * return list of failed package names in errors_r
     * return list of unavailable packages (due to InstSrc errors or wrong media nr) in remaining_r
     * return list of unavailable source packages (due to InstSrc errors or wrong media nr) in srcremaining_r
     * return number of sucessfully installed packages
     **/
    static int commitPackages( unsigned int media_nr,
			       std::list<std::string>& errors_r,
			       std::list<std::string>& remaining_r,
			       std::list<std::string>& srcremaining_r,
			       InstSrcManager::ISrcIdList installrank = InstSrcManager::ISrcIdList() );

    /**
     * Install a single rpm file.
     **/
    static PMError installFile( const Pathname & path_r );

    /**
     * Remove a single package by name.
     **/
    static PMError removePackage( const std::string & pkgname_r );
};

///////////////////////////////////////////////////////////////////

#endif // Y2PM_h
