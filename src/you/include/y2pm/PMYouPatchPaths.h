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

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchPaths
/**
 * Pathes for patches.
 **/
class PMYouPatchPaths : virtual public Rep {
  REP_BODY(PMYouPatchPaths);

  public:
    PMYouPatchPaths();
    PMYouPatchPaths( const std::string &product, const std::string &version,
                     const std::string &baseArch );

    void init( const std::string &product, const std::string &version,
               const std::string &baseArch );

    void setPatchPath( const Pathname & );
    Pathname patchPath();

    void setPatchUrl( const Url & );
    Url patchUrl();

    Pathname rpmPath( const PMPackagePtr &pkg );

    Pathname scriptPath( const std::string &scriptName );

    /**
      Return local base directory for you related files.
    */
    Pathname localDir();

    /**
      Return directory where information about installed patches is stored.
    */
    Pathname installDir();

    Pathname localSuseServers();

    std::string product();
    
    std::string version();
    
    PkgArch baseArch();

    bool businessProduct();

    PMError requestServers( const std::string &url = "", bool addFile = false );

    std::list<Url> servers();
    
    Url defaultServer();

  private:
    Pathname _patchPath;
    Pathname _rpmPath;
    Pathname _scriptPath;
    
    Url _patchUrl;
    
    std::string _product;
    std::string _version;
    PkgArch _baseArch;
    
    std::list<Url> _servers;

    bool _businessProduct;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchPaths_h
