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

  File:       PMYouPatchPaths.h

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Class providing path information for YOU patches.

/-*/
#ifndef PMYouPatchPaths_h
#define PMYouPatchPaths_h

#include <list>
#include <string>

#include <y2util/Url.h>
#include <y2util/Pathname.h>
#include <y2util/LangCode.h>

#include <y2pm/PMError.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PkgArch.h>

#include <y2pm/PMYouServers.h>

#include <y2pm/PMYouPatchPathsPtr.h>

class SysConfig;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchPaths
/**
 * Paths for patches.
 **/
class PMYouPatchPaths : virtual public Rep {
  REP_BODY(PMYouPatchPaths);

  public:
    /**
      Constructor.
    */
    PMYouPatchPaths();
    /**
      Constructor. Initialise object with values based on the given product,
      version and baseArch.
    */
    PMYouPatchPaths( const std::string &product, const std::string &version,
                     const std::string &baseArch );

    /**
      Destructor
    */
    ~PMYouPatchPaths();

    /**
      Init object.
      
      @param product  Product name
      @param version  Product version
      @param baseArch Base architecture of product
      @param youUrl   URL used to get YOU server list
      @param path     Path on server to directory containing patches, rpms and
                      scripts directories
      @param business True, if product is a business product, needing
                      authentification on the server
    */
    void init( const std::string &product, const std::string &version,
               const std::string &baseArch, const std::string &youUrl,
               const std::string &path, bool business );
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
      Set path on server to directory containing the "patches", "rpms" and
      "scripts" directories. This path depends on the product.
    */
    void setPatchPath( const Pathname & );
    /**
      Return path to patches on server.
    */
    Pathname patchPath();

    /**
      Set patch server where patches are read from.
    */
    void setPatchServer( const PMYouServer & );
    /**
      Return patch server where patches are read from.
    */  
    PMYouServer patchServer();

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
      Return path to RPM for base architecture.
      
      @param pkg      Pointer to package
      @param patchRpm If true, return path to patch RPM, if false, return path
                      to full RPM.
    */
    Pathname rpmPath( const PMPackagePtr &pkg, bool patchRpm = false );
    /**
      Return path to RPM.
      
      @param pkg      Pointer to package
      @param arch     Architecture of RPM.
      @param patchRpm If true, return path to patch RPM, if false, return path
                      to full RPM.
    */
    Pathname rpmPath( const PMPackagePtr &pkg, const std::string & arch,
                      bool patchRpm = false );

    /**
      Return path of script.
      
      @param scriptName Name of script
    */
    Pathname scriptPath( const std::string &scriptName );

    /**
      Return path of script at download location.
      
      @param scriptName Name of script
    */
    
    Pathname localScriptPath( const std::string &scriptName );

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
      Return name of product.
    */
    std::string product();
    
    /**
      Return name of distribution.
    */
    std::string distProduct();
    
    /**
      Return version of product.
    */
    std::string version();
    
    /**
      Return base architecture.
    */
    PkgArch baseArch();

    /**
      Return list of compatible architectures.
    */
    std::list<PkgArch> archs();

    /**
      Return architecture.
    */
    PkgArch arch();

    /**
      Return whether the product is a business product or not. Business products
      require authentification on the server.
    */
    bool businessProduct();

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
      Return base URL where list of YOU servers is read from.

      Return default URL, if the information can't be read from the product
      information.
    */
    std::string youUrl();

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

  protected:
    void init( const std::string &path );

  private:
    Pathname _pathPrefix;

    Pathname _patchPath;
    Pathname _rpmPath;
    Pathname _scriptPath;

    PMYouServer _patchServer;
    
    std::string _product;
    std::string _version;
    PkgArch _baseArch;

    std::string _distProduct;

    std::list<PkgArch> _archs;
    PkgArch _arch;
    
    std::string _youUrl;
    bool _businessProduct;

    SysConfig *_config;

    LangCode _lang;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchPaths_h
