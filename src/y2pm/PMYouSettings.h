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

#include <y2pm/PMYouSettingsPtr.h>

class SysConfig;

/**
  This class provides general settings for YOU.
*/
class PMYouSettings : virtual public Rep
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
      Set path prefix. This is the path read from the file "media.1/patches". It
      is prepended to the product dependent path to the patches.
    */
    void setPathPrefix( const Pathname & );
    
    /**
      Get patch prefix.
    */
    Pathname pathPrefix();

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

  protected:
    void init();

  private:
    Pathname _pathPrefix;

    PMYouServer _patchServer;
    
    SysConfig *_config;

    LangCode _lang;

    std::list<PMYouProductPtr> _products;
};

#endif
