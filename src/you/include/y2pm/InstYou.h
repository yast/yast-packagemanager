/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:       InstYou.h

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: YaST Online Update

/-*/
#ifndef InstYou_h
#define InstYou_h

#include <list>
#include <string>

#include <y2util/Url.h>
#include <y2util/Pathname.h>

#include <y2pm/PMError.h>
#include <y2pm/MediaAccess.h>

#include <y2pm/PMYouPatchPtr.h>
#include <y2pm/PMYouPatchInfo.h>
#include <y2pm/PMYouPatchPaths.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstYou
//
class InstYou {

  public:
    /**
     * Constructor
     *
     **/
    InstYou();

    /**
     * Custom constructor
     *
     **/
    InstYou( const PMYouPatchInfoPtr &, const PMYouPatchPathsPtr & );

    /**
     * Destructor
     **/
    ~InstYou();

    /**
      Init you with product information from installed system.
    */
    PMError initProduct();

    /**
     * Get list of patch servers.
     */
    PMError servers( std::list<Url> & );

    /**
     * Check authorization for server access.
     */
    PMError checkAuthorization( const Url &url, const std::string &regcode,
                                const std::string &password );
    
    /**
     * Read patch information files.
     *
     * @param url       URL of patch server.
     * @param checkSig  If true check signatures of patch info files.
     */
    PMError retrievePatchInfo( const Url &url, bool checkSig = true );
    
    /**
     * Attach source of patches.
     *
     */
    PMError attachSource();
    
    /**
     * Download packages and scripts belonging to selected patches.
     *
     * @param checkSig  If true check signatures of downloaded packages and
     *                  scripts.
     */
    PMError retrievePatches( bool checkSig = true );
    
    /**
     * Get first selected patch.
     */
    PMYouPatchPtr firstPatch();

    /**
     * Get next selected patch.
     */
    PMYouPatchPtr nextPatch();

    /**
     * Download next patch in list of selected patches.
     *
     * @param checkSig  If true check signatures of downloaded packages and
     *                  scripts.
     */
    PMError retrieveCurrentPatch( bool checkSig = true );

    /**
     * Install next patch in list of selected patches to target system.
     */
    PMError installCurrentPatch();

    /**
     * Install patches to target system.
     *
     * @param dryrun Don't actually install packages. Just print debug output. 
     */
    PMError installPatches( bool dryrun = false );

    /**
     * Select Patches.
     *
     * If there are YaST2 patches, only the newest YaST2 patch
     * will be selected.
     * 
     * If there are no YaST2 patches, all patches will be selected which
     * match the given kind and contain packages which have an older version
     * already installed.
     *
     * Patches which contain packages which are older than the corresponding
     * installed version will not be selected.
     *
     * @param kinds Ored list of kinds to be installed.
     */
    void selectPatches( int kinds );

    /**
     * Get object holding path information.
     */
    PMYouPatchPathsPtr paths() const { return _paths; }

    /**
     * Get object holding patch information.
     */
    PMYouPatchInfoPtr patchInfo() const { return _info; }

    /**
     * Remove downloaded packages.
     */
    PMError removePackages();

    /**
     * Print list of patches to stdout.
     *
     * @param verbose show more detailed information.
     */
    void showPatches( bool verbose = false );

  private:
    void init();

  protected:
    PMError installPatch( const PMYouPatchPtr &, bool dryrun = false );
    PMError retrievePatch( const PMYouPatchPtr &, bool checkSig = true );
    PMError retrievePackage( const PMPackagePtr &pkg );

    /**
     * check, if patch has new packages.
     *
     * @param requireInstalled if true, it is required that at least one
     *                         package from the patch is already installed
     *                         in an older version.
     */
    bool hasNewPackages( const PMYouPatchPtr &patch, bool requireInstalled );
    bool firesPackageTrigger( const PMYouPatchPtr &patch );
    bool firesScriptTrigger( const PMYouPatchPtr &patch );

  private:
    PMYouPatchPtr nextSelectedPatch();

  private:
    PMYouPatchInfoPtr _info;
    PMYouPatchPathsPtr _paths;
    
    std::list<PMYouPatchPtr> _patches;
    
    std::list<PMYouPatchPtr>::const_iterator _selectedPatchesIt;

    MediaAccess _media;
};

///////////////////////////////////////////////////////////////////

#endif // InstYou_h
