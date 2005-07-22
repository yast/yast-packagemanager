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
#include <y2pm/PMYouSettingsPtr.h>

#include <y2pm/PMYouServersPtr.h>

class SysConfig;

/**
  This class provides information about a YOU server.
*/
class PMYouServer
{
  public:
    /**
      Type of server. This depends on how the URL to the server was provided:

      Custom   Put in by the user
      Local    From /etc/youservers
      Remote   From URL provided by product
      Slp      Detected via the SLP protocol
      Unknown  Unknown
    */
    enum Type { Default, Custom, Local, Remote, Slp, Unknown };
  
    PMYouServer();
    PMYouServer( const std::string &, Type type = Unknown );
    PMYouServer( const Url &url, const std::string &name,
                 const std::string &directory, Type type )
      : _url( url ), _name( name ), _directory( directory ), _type( type ) {}

    void setUrl( const Url &url ) { _url = url; }
    void setUrl( const std::string &url ) { _url = Url( url ); }
    Url url() const { return _url; }

    void setUsernamePassword( const std::string &username,
                              const std::string &password );
  
    void setName( const std::string &name ) { _name = name; }
    std::string name() const { return _name; }
    
    void setDirectory( const std::string &dir ) { _directory = dir; }
    std::string directory() const { return _directory; }

    void setType( Type type ) { _type = type; }
    Type type() const { return _type; }
    std::string typeAsString() const;

    static std::string typeToString( Type );
    static Type typeFromString( std::string );

    bool operator==( const PMYouServer &server ) const;

    bool fromString( const std::string & );
    std::string toString() const;

  private:
    Url _url;
    std::string _name;
    std::string _directory;
    Type _type;
};

/**
  This class provides access to the list of YOU servers.
*/
class PMYouServers : public CountedRep
{
  REP_BODY(PMYouServers);

  public:
    /**
      Constructor.
    */
    PMYouServers( const PMYouSettingsPtr & );

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

    /**
     * Add the SLP servers to the list of known servers
     */ 
    bool addSLPServers( );

    /**
     * Add a PMYouServer to the list of known servers
     */ 
    void addServer( const PMYouServer & );

  protected:
    std::string encodeUrl( const std::string &url );

    void addPackageVersion( const std::string &pkgName, std::string &url );

    PMError readServers( const Pathname &, PMYouServer::Type );

  private:
    PMYouSettingsPtr _settings;

    std::list<PMYouServer> _servers;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouServers_h
