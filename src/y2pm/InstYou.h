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
#include <y2util/FSize.h>

#include <y2pm/PMError.h>
#include <y2pm/MediaAccess.h>

#include <y2pm/PMYouPatch.h>
#include <y2pm/PMYouPatchInfo.h>
#include <y2pm/PMYouSettings.h>
#include <y2pm/PMYouServers.h>

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
    InstYou( const PMYouPatchInfoPtr &, const PMYouSettingsPtr & );

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
    PMError servers( std::list<PMYouServer> & );

    /**
      Read user name and password needed for authentification from configuration
      file.
    */
    PMError readUserPassword();

    /**
      Write user name and password to configuration file.
      
      @param username    username
      @param password    password
      @param persistent  if true, write username/password to disk
    */
    PMError setUserPassword( const std::string &username,
                             const std::string &password, bool persistent );

    /**
      Read list of all available patches. The server where the patches are
      retrieved from has to be set in the paths() object.
    */
    PMError retrievePatchDirectory();

    /**
      Read patch information files. The server where the patches are
      retrieved from has to be set in the paths() object.
     
      @param reload    If true reload all patches from server.
      @param checkSig  If true check signatures of patch info files.
    */
    PMError retrievePatchInfo( bool reload = true, bool checkSig = true );
    
    /**
     * Attach source of patches.
     *
     */
    PMError attachSource();
    
    /**
     * Download packages and scripts belonging to selected patches.
     *
     * @param reload    If true reload all patches from server.
     * @param checkSig  If true check signatures of downloaded packages and
     *                  scripts.
     * @param noExternal If true, external packages aren't downloaded.
     */
    PMError retrievePatches( bool reload = true, bool checkSig = true,
                             bool noExternal = false );
    
    /**
      Get first selected patch.
    
      @param resetProgress If true reset progress indicator. 
    */
    PMYouPatchPtr firstPatch( bool resetProgress = true );

    /**
      Get next selected patch.
     
      @param ok Pointer to bool variable which is set to false on error and
                true on success.
    */
    PMYouPatchPtr nextPatch( bool *ok = 0 );

    /**
     * Download next patch in list of selected patches.
     *
     * @param checkSig  If true check signatures of downloaded packages and
     *                  scripts.
     */
    PMError retrieveCurrentPatch( bool reload = true, bool checkSig = true );

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
      Update states of packages according to states of patches.
    */
    void updatePackageStates();

    /**
     * Get object holding path information.
     */
    PMYouSettingsPtr paths() const { return _settings; }

    /**
     * Get object holding patch information.
     */
    PMYouPatchInfoPtr patchInfo() const { return _info; }

    /**
      Close connection to server.
    */
    PMError disconnect();

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

    /**
      Show YOU progress. Calls callbacks.
    */
    PMError progress( int );

    /**
      Show YOU patch progress. Calls callbacks.
    */
    PMError patchProgress( int, const std::string &pkg = std::string() );

    /**
      Returns how many days ago the last update was performed. Returns -1
      when no update has occured yet.
    */
    int lastUpdate();

    /**
      Writes current date as date of last update.
    */
    PMError writeLastUpdate();

    /**
      Return total size of data to be downloaded based on current selection of
      patches.
    */
    FSize totalDownloadSize() { return _totalDownloadSize; }

    /**
      Check for updates. Returns number of new updates available or -1 on error.
      Doesn't check for type of updates and doesn't download any update
      information.
    */
    int quickCheckUpdates();

    /*
      Return currently set username used for authentificaton to the server.
    */
    std::string username() const { return _username; }

    /*
      Return currently set password used for authentificaton to the server.
    */
    std::string password() const { return _password; }

    /**
      Remove all packages which aren't to be installed because of their
      architecture.
    */
    void filterArchitectures( PMYouPatchPtr &patch );

  public:
    class Callbacks
    {
        public:
          Callbacks() {}
          virtual ~Callbacks() {}
    
          virtual bool progress( int percent ) = 0;
          virtual bool patchProgress( int percent, const std::string &str ) = 0;

          virtual bool executeYcpScript( const std::string &script ) = 0;
    };

    static void setCallbacks( Callbacks * );

  private:
    static Callbacks *_callbacks;

  private:
    void init();

  protected:
    PMError installPatch( const PMYouPatchPtr &, bool dryrun = false );
    PMError retrievePatch( const PMYouPatchPtr &, bool reload,
                           bool checkSig = true, bool noExternal = false );
    PMError retrievePackage( const PMPackagePtr &pkg, const PMYouProductPtr &,
                             bool reload = true, bool noExternal = false );
    PMError retrieveScript( const std::string &script, const PMYouProductPtr &,
                            bool reload, bool checkSig );
    PMError InstYou::retrieveFile( const PMYouFile &file, bool reload );

    PMError executeScript( const std::string &, const PMYouProductPtr &,
                           bool dryrun );

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

    bool hasPatchRpm( const PMPackagePtr &pkg );

  private:
    PMYouPatchPtr nextSelectedPatch();

  private:
    PMYouPatchInfoPtr _info;
    PMYouSettingsPtr _settings;
    
    std::list<PMYouPatchPtr> _patches;

    std::list<PMYouPatchPtr>::const_iterator _selectedPatchesIt;
    int _progressTotal;
    int _progressCurrent;
    
    MediaAccess _media;

    std::string _username;
    std::string _password;

    FSize _totalDownloadSize;
};

///////////////////////////////////////////////////////////////////

#endif // InstYou_h
