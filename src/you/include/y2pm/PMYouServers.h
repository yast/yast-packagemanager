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

  File:       PMYouServers.h

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Class providing path information for YOU patches.

/-*/
#ifndef PMYouServers_h
#define PMYouServers_h

#include <list>
#include <string>

#include <y2util/Url.h>
#include <y2util/Pathname.h>

#include <y2pm/PMError.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PkgArch.h>
#include <y2pm/PMYouPatchPathsPtr.h>

#include <y2pm/PMYouServersPtr.h>

class SysConfig;

/**
  This struct represents a YOU server.
*/
struct PMYouServer
{
  std::string url;
  std::string name;
  std::string directory;
};

/**
  This class provides access to the list of YOU servers.
*/
class PMYouServers : virtual public Rep {
  REP_BODY(PMYouServers);

  public:
    /**
      Constructor.
    */
    PMYouServers( const PMYouPatchPathsPtr & );

    /**
      Destructor
    */
    ~PMYouServers();

    /**
      Return path to local server list (suseservers). This file is obsolete, use
      localYouServers() instead.
    */
    Pathname localSuseServers();

    /**
      Return path to local server list configuration file.
    */
    Pathname localYouServers();

    /**
      Return path to file where the server list is downloaded to from the
      server.
    */
    Pathname cachedYouServers();

    /**
      Get list of YOU servers from CGI script on the master server.
      If check is true the server list is going to be used to check the
      availability of updates but not to axtually download them.
    */
    PMError requestServers( bool check = false );

    /**
      Return list of known servers.
    */
    std::list<PMYouServer> servers();
    
    /**
      Return default server, when no server list can be read at all.
    */
    PMYouServer defaultServer();

    /**
      Return server which is currently used for getting updates.
    */
    PMYouServer currentServer();

  protected:
    std::string encodeUrl( const std::string &url );

    void addPackageVersion( const std::string &pkgName, std::string &url );

    PMError readServers( const Pathname & );
    void addServer( const PMYouServer & );

    PMYouServer parseServerLine( const std::string & );

  private:
    PMYouPatchPathsPtr _patchPaths;

    std::list<PMYouServer> _servers;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouServers_h
