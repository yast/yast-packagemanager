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

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Class providing path information for YOU patches.

/-*/
#ifndef PMYOUSETTINGS_H
#define PMYOUSETTINGS_H

#include <list>
#include <string>

#include <y2util/Url.h>
#include <y2util/Pathname.h>
#include <y2util/LangCode.h>

#include <y2pm/PMError.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PkgArch.h>

#include <y2pm/PMYouServers.h>
#include <y2pm/PMYouProductPtr.h>
#include <y2pm/PMYouMediaPtr.h>

#include <y2pm/PMYouSettingsPtr.h>

class SysConfig;

/**
  This class provides general settings for YOU.
*/
class PMYouSettings : public CountedRep
{
  REP_BODY(PMYouSettings);

  public:
    /**
      Constructor.
    */
    PMYouSettings();
    /**
      Constructor. Initialise object with values based on the given product,
      version and baseArch.
    */
    PMYouSettings( const std::string &product, const std::string &version,
                   const std::string &baseArch );

    /**
      Destructor
    */
    ~PMYouSettings();

    /**
      Init object with given product, version and baseArch values.
    */
    void init( const std::string &product, const std::string &version,
               const std::string &baseArch );

    /**
      Read information about product (name, version, architecture, YOU URL,
      if the product is a business product, path on server and name of
      distproduct) and initialize object accordingly.
    */
    PMError initProduct();

    /**
      Set master media. This is used as reference for testing other media
      instances.
    */
    void setMasterMedia( const PMYouMediaPtr & );
    /**
      Return master media.
    */
    PMYouMediaPtr masterMedia();

    /**
      Set path prefix. This is the path read from the file "media.1/patches". It
      is prepended to the product dependent path to the patches.
    */
    void setPathPrefix( const Pathname & );
    
    /**
      Get patch prefix.
    */
    Pathname pathPrefix();

    /**
      Filter exclusive products. Remove all products which are not contained in
      the argument list. If the list is empty no products are removed.
    */
    void filterExclusiveProducts( const std::list<std::string> & );

    /**
      Set patch server where patches are read from.
    */
    void setPatchServer( const PMYouServer & );
    /**
      Return patch server where patches are read from.
    */  
    PMYouServer patchServer();

    /**
      Set username and password for current server.
    */
    void setUsernamePassword( const std::string &username,
                              const std::string &password );

    /**
      Return base URL to patches. This doesn't include the product dependent
      path.
    */
    Url patchUrl();

    /**
      Return path to patch meta information file (aka media.1/patches).
    */
    Pathname mediaPatchesFile();

    /**
      Return local base directory for you related files.
    */
    Pathname localDir();

    /**
      Return local base directory for you related files suited for writing to
      it. This depends on the identity of the user. root gets a directory
      under /var, non-root users get a directory in their home directory.
    */
    Pathname localWriteDir();

    /**
      Return directory used as attach point for media interface, i.e. for
      mounting CDs or NFS servers or for downloading files via FTP, HTTP etc.
      This function returns a writable location.
    */
    Pathname attachPoint();

    /**
      Return directory used as attach point for media interface, i.e. for
      mounting CDs or NFS servers or for downloading files via FTP, HTTP etc.
      This function returns the location used by root.
    */
    Pathname rootAttachPoint();

    /**
      Return directory where information about installed patches is stored.
    */
    Pathname installDir();

    /**
      Return path to directory where external RPMs are stored.
    */
    Pathname externalRpmDir();

    /**
      Return path to directory where extra files are stored.
    */
    Pathname filesDir();

    /**
      Return name of file holding the list of patches in the "patches"
      directory.
    */
    std::string directoryFileName();

    /**
      Return name of file holding the media map for the patches.
    */
    std::string mediaMapFileName();

    /**
      Return path to file used for storing cookies.
    */
    Pathname cookiesFile();
    
    /**
      Return path to file used for storing configuration information.
    */
    Pathname configFile();

    /**
      Return path to file used for storing passwords.
    */
    Pathname passwordFile();

    /**
      Return path to file used for special YOU logs.
    */
    Pathname logFile();

    /**
      Return configuration object for file at configFile().
    */
    SysConfig *config();

    /**
      Set language code used for displaying messages to the user. If the
      language code argument is empty, the preferred YaST locale is set.
    */
    void setLangCode( const LangCode & );

    /**
      Return language code used for displaying messages to the user.
    */
    LangCode langCode() const { return _lang; }

    /**
      Return clear text locale as used by the patch file parser.
    */
    std::string locale() const;

    /**
      Return default locale. See locale().
    */
    std::string defaultLocale() const;

    /**
     * Translate standard lang code into full language name as used in patch
     * info files.
     */
    static std::string translateLangCode( const LangCode &lang );

    /**
      Return products relevant for YOU updates.
    */
    std::list<PMYouProductPtr> products() const;

    /**
      Return product which characterizes the installed system best.
    */
    PMYouProductPtr primaryProduct() const;

    /**
      Set if signatures should be checked or not.
    */
    void setCheckSignatures( bool );
    /**
      Return true, if signatures should be checked, otherwise return false.
    */
    bool checkSignatures() const;

    /**
      Set if patches should be reloaded from the server.
    */
    void setReloadPatches( bool );
    /**
      Return true if patches should be reloaded from the server, otherwise
      return false.
    */
    bool reloadPatches();

    /**
      Set if external packages should be downloaded.
    */
    void setNoExternalPackages( bool );
    /**
      Return true if external packages should be downloaded, otherwise return
      false.
    */
    bool noExternalPackages() const;

    /**
      Set if the you run should be performed as dry run, i.e. without actually
      installing the patches.
    */
    void setDryRun( bool );
    /**
      Return true, if the YOU run should be performed as dry run, i.e. without
      actually installing the patches, otherwise return false.
    */
    bool dryRun() const;

    /**
      Set if all packages should be retrieved, not only the ones which actually
      will be installed.
    */
    void setGetAll( bool );
    /**
      Return true if all packages should be retrieved, not only the ones which
      actually will be installed, otherwise return false.
    */
    bool getAll() const;

    /**
      Set if patches will only be retrived, but not installed.
    */
    void setGetOnly( bool );

    /**
      Get if patches will only be retrived, but not installed.
    */
    bool getOnly() const;

    /**
      Enable writing of separate YOU log.
    */
    void setLogEnabled( bool );
    /**
      Return if the separate YOU log is enabled.
    */
    bool isLogEnabled() const;

  protected:
    void init();

  private:
    PMYouMediaPtr _masterMedia;
    Pathname _pathPrefix;

    PMYouServer _patchServer;
    
    SysConfig *_config;

    LangCode _lang;

    std::list<PMYouProductPtr> _products;

    bool _checkSignatures;
    bool _reloadPatches;
    bool _noExternalPackages;
    bool _dryRun;
    bool _getAll;
    bool _getOnly;
    bool _isLogEnabled;
};

#endif
