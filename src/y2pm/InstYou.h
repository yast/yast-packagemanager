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
    */
    PMError retrievePatchInfo();
    
    /**
      Process selected patches. Patches are downloaded or mounted as required
      and then installed. The settings of the PMYouSettings object are taken
      into account.

      The function uses InstYou::Callbacks to report errors, log progress and
      handle other events which require information or action from the user.
    */
    PMError processPatches();

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
    PMYouSettingsPtr settings() const { return _settings; }

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

    /**
      Show YOU progress. Calls callbacks.
    */
    PMError progress( int );

    /**
      Show YOU patch progress. Calls callbacks.
    */
    PMError patchProgress( int, const std::string &pkg = std::string() );

    /**
      Show error. Calls callbacks.
    */
    PMError showError( const std::string &type, const std::string &text,
                       const std::string &details );

    /**
      Show error. Calls callbacks.
    */
    PMError showError( const PMError &error );

    /**
      Show patch message.
    */
    PMError showMessage( const std::string &type, std::list<PMYouPatchPtr> & );

    /**
      Add line to log. Calls callbacks.
    */
    void log( const std::string &text );

    /**
      Returns how many days ago the last update was performed. Returns -1
      when no update has occured yet.
    */
    int lastUpdate();

    /**
      Return how many patches have been installed.
    */
    int installedPatches() { return _installedPatches; }

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

          virtual PMError showError( const std::string &type,
                                     const std::string &text,
                                     const std::string &details ) = 0;
          virtual PMError showMessage( const std::string &type,
                                       const std::list<PMYouPatchPtr> & ) = 0;
          virtual void log( const std::string &text ) = 0;

          virtual bool executeYcpScript( const std::string &script ) = 0;
    };

    static void setCallbacks( Callbacks * );

  private:
    static Callbacks *_callbacks;

  private:
    void init();

  protected:
    /**
     * Attach source of patches.
     *
     */
    PMError attachSource();

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
     * @param reload    If true reload all patch from server.
     * @param checkSig  If true check signatures of downloaded packages and
     *                  scripts.
     */
    PMError retrieveCurrentPatch();

    /**
     * Install next patch in list of selected patches to target system.
     */
    PMError installCurrentPatch();
    
    /**
      Close connection to server.
    */
    PMError disconnect();

    /**
      Writes current date as date of last update.
    */
    PMError writeLastUpdate();

    PMError installPatch( const PMYouPatchPtr & );
    PMError retrievePatch( const PMYouPatchPtr & );
    PMError retrievePackage( const PMPackagePtr &pkg, const PMYouProductPtr & );
    PMError retrieveScript( const std::string &script,
                            const PMYouProductPtr & );
    PMError InstYou::retrieveFile( const PMYouFile &file );

    PMError executeScript( const std::string &, const PMYouProductPtr & );

    /**
      Check, if patch has only packages which are at least as new as the
      installed version. If the patch contains a package which is older than the
      installed version or the patch doesn't contain a package which is newer
      than the installed version false is returned. 
     
      @param requireInstalled if true, it is required that at least one
                              package from the patch is already installed
                              in an older version.
    */
    bool hasOnlyNewPackages( const PMYouPatchPtr &patch );
    /**
      Check, if the patch has at least one package which is newer than the
      instakked version.
    */
    bool hasNewPackages( const PMYouPatchPtr &patch );

    /**
      Check, if package is newer than the installed version. Return false if the
      package isn't installed yet.
    */
    bool isNewerPackage( const PMPackagePtr &pkg );

    /**
      Return if the package is to be installed on the system.
    */
    bool packageToBeInstalled( const PMYouPatchPtr &patch,
                               const PMPackagePtr &pkg );

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

    int _installedPatches;
};

///////////////////////////////////////////////////////////////////

#endif // InstYou_h
