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
     */
    PMError retrievePatches( const Url & );
    
    /**
     * Select Patches. If there are YaST2 patches, only the newest YaST2 patch
     * will be selected.
     *
     * @param kinds Ored list of kinds to be installed.
     */
    void selectPatches( int kinds );

    /**
     * Download packages belonging to selected patches.
     */
    PMError retrievePackages();
    
    /**
     * Install patches to target system.
     */
    PMError installPatches();
  
  private:
    PMYouPatchInfoPtr _info;
    PMYouPatchPathsPtr _paths;
    
    std::list<PMYouPatchPtr> _patches;
    std::list<PMYouPatchPtr> _selectedPatches;

    MediaAccess _media;
};

///////////////////////////////////////////////////////////////////

#endif // InstYou_h
