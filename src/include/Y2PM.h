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

#include <y2pm/InstTarget.h>
#include <y2pm/InstSrcManager.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectionManager.h>
#include <y2pm/PMYouPatchManager.h>

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
     * Access to the installation target
     **/
    static InstTarget & instTarget();

    /**
     * Access to the installation source manager
     **/
    static InstSrcManager & instSrcManager();

    /**
     * Access to the Package manager
     * if with_target == true ( the default)
     * also start up InstTarget at "/"
     * if with_target == false, we're running in the inst-sys
     * and there is no target yet.
     **/
    static PMPackageManager & packageManager(bool with_target = true);

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
	     * called right before package 'name' is installed
	     * */
	    void (*_installation_package_start_func)(const std::string& name);
	    void* _installation_package_start_data;

	    /**
	     * called multiple times during package installation, 'progress' is the
	     * already installed percentage
	     * */
	    void (*_installation_package_progress_func)(const std::string& name, int progress);
	    void* _installation_package_progress_data;

	    /**
	     * called after package 'name' got installed
	     * */
	    void (*_installation_package_done_func)(const std::string& name);
	    void* _installation_package_done_data;

	};

	static CallBacks _callbacks;

    public:

	/**
	 * called right before package 'name' is installed
	 * */
	static void setInstallationPackageStartCallback(void (*func)(const std::string& name), void* data);

	/**
	 * called multiple times during package installation, 'progress' is the
	 * already installed percentage
	 * */
	static void setInstallationPackageProgressCallback(void (*func)(const std::string& name, int progress), void* data);

	/**
	 * called after package 'name' got installed
	 * */
	static void setInstallationPackageDoneCallback(void (*func)(const std::string& name), void* data);
};

///////////////////////////////////////////////////////////////////

#endif // Y2PM_h
