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
     * Get list of patch servers.
     */
    PMError servers( std::list<Url> & );
    
    /**
     * Read patch information files.
     *
     * @param url URL of patch server.
     */
    PMError retrievePatchInfo( const Url &url );
    
    /**
     * Attach source of patches.
     *
     */
    PMError attachSource();
    
    /**
     * Download packages belonging to selected patches.
     */
    PMError retrievePatches();
    
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
     */
    PMError retrieveCurrentPatch();

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
     * Select Patches. If there are YaST2 patches, only the newest YaST2 patch
     * will be selected.
     *
     * @param kinds Ored list of kinds to be installed.
     */
    void selectPatches( int kinds );

    /**
     * Deselect all patches which only contain packages which aren't installed
     * or which contain packages which are older than the packages that are
     * installed.
     */
    void filterPatchSelection();

    /**
     * Get object holding path information.
     */
    PMYouPatchPathsPtr paths() const { return _paths; }

  private:
    void init();

  protected:
    PMError installPatch( const PMYouPatchPtr &, bool dryrun = false );
    PMError retrievePatch( const PMYouPatchPtr & );

  private:
    PMYouPatchInfoPtr _info;
    PMYouPatchPathsPtr _paths;
    
    std::list<PMYouPatchPtr> _patches;
    std::list<PMYouPatchPtr> _selectedPatches;
    
    std::list<PMYouPatchPtr>::const_iterator _selectedPatchesIt;

    MediaAccess _media;
};

///////////////////////////////////////////////////////////////////

#endif // InstYou_h
