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

		Additionally, the InstTarget can also change package
		and patch lists, by installing or removing them.

/-*/
#ifndef InstTarget_h
#define InstTarget_h

#include <iosfwd>
#include <list>
#include <string>

#include <y2util/Pathname.h>

#include <y2pm/InstTarget.h>		// pointer to self
#include <y2pm/MediaAccess.h>		// physical media access class
#include <y2pm/InstSrcDescr.h>		// target description
#include <y2pm/InstSrcData.h>		// target content

#include <y2pm/PMSolvable.h>
#include <y2pm/PMPackage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstTarget
/**
 *
 **/
class InstTarget: virtual public Rep {
  REP_BODY(InstTarget)

  protected:

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
    std::list <InstSrcDescr> *_descrs;

    /**
     * content of media
     * this describes the content of the target
     */
    InstSrcData *_data;

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

    //-----------------------------
    // general functions

    /**
     * clean up, e.g. remove all caches
     */
    bool Erase();

    /**
     * @return description of Installation target
     */
    const InstSrcDescr **getDescriptions() const;

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
     * generate PMSolvable objects for each selection on the source
     * @return list of PMSolvablePtr on this source
     */
    const std::list<PMSolvablePtr> *getSelections() const;

    /**
     * generate PMPackage objects for each Item on the source
     * @return list of PMPackagePtr on this source
     * */
    const std::list<PMPackagePtr> *getPackages() const;

    /**
     * generate PMSolvable objects for each patch on the source
     * @return list of PMSolvablePtr on this source
     */
    const std::list<PMSolvablePtr> *getPatches() const;

    //-----------------------------
    // package install / remove

    /**
     * set package installation parameters
     * @param to be defined
     * set 'rpm' parameters used during package installation
     */
    int setPkgInstParam (const std::string & param);

    /**
     * get current package installation parameters
     * get 'rpm' parameters currently used for package installation
     */
    const std::string & getPkgInstParam (void) const;

    /**
     * install PMPackage object to the target
     * @param PMPackagePtr of package
     * @return status
     * this function retrieves package and installation source
     * information from PMPackagePtr, creates the required MediaAccess
     * (if not already done), attaches the media, calls provideFile
     * and then 'rpm'
     *
     * A sucessfull package installation is reflected in
     * numPackages() and getPackages()
     */
    int installPackage (PMPackagePtr package);

    /**
     * remove PMPackage object from the target
     * @return status
     *
     * A sucessfull package removal is reflected in
     * numPackages() and getPackages()
     */
    int removePackage (PMPackagePtr package);

    //-----------------------------
    // patch install / remove

    /**
     * set patch installation parameters
     * @param to be defined
     * set parameters used during patch installation
     */
    int setPatchInstParam (const std::string & param);

    /**
     * get current patch installation parameters
     * get parameters currently used for patch installation
     */
    const std::string & getPatchInstParam (void) const;

    /**
     * install PMSolvable object of patch to the target
     * @param PMSolvablePtr for patch
     * @return status
     * this function retrieves patch and installation source
     * information from PMSolvablePtr, creates the required MediaAccess
     * (if not already done), attaches the media, calls provideFile
     * and then does whatever is needed.
     *
     * A sucessfull patch installation is reflected in
     * numPatches () and getPatches ()
     */
    int installPatch (PMSolvablePtr patch);

    /**
     * remove PMPackage object from the target
     * @return status
     *
     * A sucessfull patch removal is reflected in
     * numPatches () and getPatches ()
     */
    int removePatch (PMSolvablePtr patch);

    std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstTarget_h
