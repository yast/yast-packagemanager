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

#include <y2pm/PMError.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PkgArch.h>

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
    PMYouPatchPaths();
    PMYouPatchPaths( const std::string &product, const std::string &version,
                     const std::string &baseArch );

    ~PMYouPatchPaths();

    void init( const std::string &product, const std::string &version,
               const std::string &baseArch, const std::string &youUrl,
               const std::string &path, bool business );
    void init( const std::string &product, const std::string &version,
               const std::string &baseArch );

    PMError initProduct();

    void setPatchPath( const Pathname & );
    Pathname patchPath();

    void setPatchUrl( const Url & );
    Url patchUrl();

    Pathname rpmPath( const PMPackagePtr &pkg, bool patchRpm = false );
    Pathname rpmPath( const PMPackagePtr &pkg, const std::string & arch,
                      bool patchRpm = false );

    Pathname scriptPath( const std::string &scriptName );

    Pathname localScriptPath( const std::string &scriptName );

    /**
      Return local base directory for you related files.
    */
    Pathname localDir();

    /**
      Return directory used as attach point for media interface, i.e. for
      mounting CDs or NFS servers or for downloading files via FTP, HTTP etc.
    */
    Pathname attachPoint();

    /**
      Return directory where information about installed patches is stored.
    */
    Pathname installDir();

    Pathname externalRpmDir();

    Pathname localSuseServers();

    std::string directoryFileName();

    std::string product();
    
    std::string version();
    
    PkgArch baseArch();

    std::list<PkgArch> archs();

    bool businessProduct();

    PMError requestServers( const std::string &url = std::string() );

    std::list<Url> servers();
    
    Url defaultServer();

    std::string mirrorList();

    std::string defaultMirrorList();

    Pathname cookiesFile();
    
    Pathname configFile();

    SysConfig *config();

  protected:
    void init( const std::string &path );

    std::string encodeUrl( const std::string &url );

  private:
    Pathname _patchPath;
    Pathname _rpmPath;
    Pathname _scriptPath;
    
    Url _patchUrl;
    
    std::string _product;
    std::string _version;
    PkgArch _baseArch;

    std::list<PkgArch> _archs;
    
    std::list<Url> _servers;

    std::string _youUrl;
    bool _businessProduct;

    SysConfig *_config;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchPaths_h
