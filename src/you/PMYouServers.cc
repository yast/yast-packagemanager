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

  File:       PMYouServers.cc

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Provide YOU server information.

/-*/

#include <iostream>
#include <fstream>
#include <sstream>

#include <y2util/Y2SLog.h>
#include <y2util/SysConfig.h>

#include <Y2PM.h>
#include <y2pm/InstSrcError.h>
#include <y2pm/PMPackage.h>
#include <y2pm/YouError.h>
#include <y2pm/InstTarget.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/PMPackageManager.h>
#include <y2pm/PMSelectable.h>
#include <y2pm/PMYouSettings.h>
#include <y2pm/PMYouProduct.h>

#include <y2pm/PMYouServers.h>

#include <slp.h>

using namespace std;


//////////////////////////////////////////////////////////////////
//
// Callback for SLPFindAttrs
//
SLPBoolean PMAttrCallback(SLPHandle hslp,
			  const char* attrlist,
			  SLPError errcode,
			  void* mydata )
{

    if ( errcode == SLP_OK )
    {
	string* data = static_cast<string *>(mydata);
	*data = attrlist;
    }

    return SLP_TRUE;
}

//////////////////////////////////////////////////////////////////
//
// Callback for SLPFindSrvs
//
SLPBoolean PMSLPSrvURLCallback( SLPHandle hslp, 
				const char* srvurl, 
				unsigned short lifetime, 
				SLPError errcode, 
				void* mydata ) 
{
    if ( errcode == SLP_OK || errcode == SLP_LAST_CALL ) 
    {
	if ( srvurl )
	{
	    y2milestone( "Service found: %s", srvurl );

	    string myattr;
	    SLPHandle handleslp;
	    SLPError err;

	    err = SLPOpen( NULL, SLP_FALSE, &handleslp);

	    if ( err == SLP_OK )
	    {
		// get attributes for this service
		err = SLPFindAttrs( handleslp,
				    srvurl,
				    "",			// use configured scopes
				    "basepath,name",	// attributes
				    PMAttrCallback,
				    (void *)&myattr );
	    }

	    string basepath = "/YOU";
	    string name = "YOU Server";
	    
	    if ( err == SLP_OK )
	    {
		// attributes e.g.:	(basepath=/YOU),(name=YOU Server Local)
		vector<string> splitattr;
		vector<string>::iterator it;
		stringutil::split(  myattr, splitattr, ",", true );

		for ( it = splitattr.begin(); it != splitattr.end(); *it++ )
		{
		    string attr = (*it).substr( 1, (*it).size()-2 );	// remove ( )
		    if ( attr.find("name=") != string::npos )
			name = attr.substr( 5 );
		    else if  ( attr.find("basepath=") != string::npos )
			basepath = attr.substr( 9 );
		}
	    }
	    else
	    {
		y2error ( "ERROR finding attributes - using defaults" );
	    }

	    SLPClose( handleslp );

	    // service url e.g.:	 service:you.suse:http://sturm.suse.de
	    vector<string> splitted;
	    stringutil::split( srvurl, splitted, ":", true );

	    // add URL of SLP server to the list of servers
	    static_cast<PMYouServers *>(mydata)->addServer( PMYouServer(splitted[2] + ":" + splitted[3] + basepath,
									name,		// name of server
									"",		// directory
									"slp") );	// type
	}
    } 

    /* return SLP_TRUE because we want to be called again */ 
    /* if more services were found                        */ 

    return SLP_TRUE; 
}



///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouServer
//
///////////////////////////////////////////////////////////////////

PMYouServer::PMYouServer( const std::string &line )
{
  fromString( line );
}

void PMYouServer::setUsernamePassword( const string &username,
                                       const string &password )
{
  _url.setUsername( username );
  _url.setPassword( password );
}

bool PMYouServer::operator==( const PMYouServer &server ) const
{
  return _url == server._url && _name == server._name &&
         _directory == server._directory;
}

bool PMYouServer::fromString( const std::string &line )
{
  _url = Url();
  _name = "";
  _directory = "";

  vector<string> splitted;
  stringutil::split( line, splitted, ";", true );

  if ( splitted.size() >= 1 ) {
    _url = Url( splitted[ 0 ] );
    if ( !_url.isValid() ) return false;
  } else {
    return false;
  }
  if ( splitted.size() >= 2 ) _name = splitted[ 1 ];
  if ( splitted.size() >= 3 ) _directory = splitted[ 2 ];

  return true;
}

std::string PMYouServer::toString() const
{
  Url u = _url;
  u.setUsername( "" );
  u.setPassword( "" );
  return _url.asString() + ";" + _name + ";" + _directory;
}


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouServers
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(PMYouServers);

PMYouServers::PMYouServers( const PMYouSettingsPtr &patchPaths )
{
  _patchPaths = patchPaths;
}

PMYouServers::~PMYouServers()
{
}

Pathname PMYouServers::localSuseServers()
{	
  return "/etc/suseservers";
}

Pathname PMYouServers::localYouServers()
{
  return "/etc/youservers";
}

Pathname PMYouServers::cachedYouServers()
{
  return _patchPaths->localWriteDir() + "youservers";
}

PMError PMYouServers::requestServers( bool check )
{
  string lastServer = _patchPaths->config()->readEntry( "LastServer" );

  D__ << "Last Server: " << lastServer << endl;

  if ( !lastServer.empty() ) {
    addServer( PMYouServer( lastServer ) );
  }

  PMError error = readServers( localYouServers() );
  if ( error ) return error;

  SysConfig cfg( "onlineupdate" );
  
  if ( cfg.readBoolEntry( "YAST2_LOADFTPSERVER", true ) ) {
    PMYouProductPtr product = _patchPaths->primaryProduct();
    string url;

    if ( product )
    {
	url = product->youUrl();
	url += "?product=" + product->product();
	url += "&version=" + product->version();
	url += "&basearch=" + string( product->baseArch() );
	url += "&arch=" + string( product->arch() );
    
	url += "&lang=" + string( _patchPaths->langCode() );
    
	url += "&business=";
	if ( product->businessProduct() ) url += "1";
	else url += "0";
	
	url += "&check=";
	if ( check ) url += "1";
	else url += "0";
    
	url += "&distproduct=" + product->distProduct();
    }
    addPackageVersion( "yast2-online-update", url );
    addPackageVersion( "yast2-packagemanager", url );
    addPackageVersion( "liby2util", url );

    url = encodeUrl( url );

    DBG << "url: '" << url << "'" << endl;

    Pathname writeDir = _patchPaths->localWriteDir();
    int ret = PathInfo::assert_dir( writeDir );
    if ( ret != 0 ) {
      ERR << "Unable to create " << writeDir << ": errno " << ret
          << endl;
    }
    
    PMError error = MediaAccess::getFile( Url( url ), cachedYouServers() );
    if ( error ) {
      if ( error == MediaError::E_write_error ) {
        return PMError( YouError::E_write_youservers_failed );
      } else {
        return PMError( YouError::E_get_youservers_failed, error.details() );
      }
    }

    // Remove obsolete file.
    PathInfo pi( localSuseServers() );
    if ( pi.isExist() ) {
      PathInfo::unlink( localSuseServers() );
    }
  } else {
    // Backwards compatibility with SL8.1/SLES8
    error = readServers( localSuseServers() );
    if ( error ) return error;
  }

  error = readServers( cachedYouServers() );

  SysConfig syscfg( "onlineupdate" );
  if ( syscfg.readBoolEntry( "SLP_ENABLED", true ) )
  {
      y2milestone ("Cecking for SLP servers" );
      addSLPServers( );
  }
  
  return error;
}

PMError PMYouServers::readServers( const Pathname &file )
{
  DBG << "Reading servers from " << file << endl;

  PathInfo pi( file );
  if ( !pi.isExist() ) {
    DBG << "File doesn't exist." << endl;
    return PMError();
  }
  
  string line;
  ifstream in( file.asString().c_str() );
  if ( in.fail() ) {
    ERR << "Error reading " << file << endl;
    return PMError( YouError::E_read_youservers_failed, file.asString() );
  }
  while( getline( in, line ) ) {
    if ( !line.empty() && *line.begin() != '#' ) {
      PMYouServer server;
      if ( server.fromString( line ) ) {
	  server.setType( "default" );
	  addServer( server );
      }
    }
  }

  return PMError();
}

bool PMYouServers::addSLPServers( )
{
    D__ << "Add SLP servers" << endl;

    SLPError err; 
    SLPHandle hslp;
    
    err = SLPOpen( NULL, SLP_FALSE, &hslp); 
    if ( err != SLP_OK ) 
    { 
        y2error( "Error opening slp handle %i\n", err );
	return false;
    } 


    if( err != SLP_OK ) 
    { 
        y2error( "Error registering service with slp" ); 
    }
    else
    {

	err = SLPFindSrvs( hslp, 
			   "you.suse", 
			   0,                    /* use configured scopes */ 
			   0,                    /* no attr filter        */ 
			   PMSLPSrvURLCallback, 
			   this ); 

	if( err != SLP_OK ) 
	{ 
	    y2error( "Error registering service with slp" ); 
	} 
    }

    SLPClose(hslp); 

    if ( err == SLP_OK )
	return true;
    else
	return false;
    
    D__ << "Add SLP servers done" << endl;    
}

void PMYouServers::addServer( const PMYouServer &server )
{
  D__ << "Add server: " << server.name() << " (" << server.url();
  if ( !server.directory().empty() )
    D__ << ", directory: " << server.directory() << endl;
  D__ << ")" << endl;

  list<PMYouServer>::const_iterator it;
  for( it = _servers.begin(); it != _servers.end(); ++it ) {
    if ( *it == server ) break;
  }
  if ( it == _servers.end() ) _servers.push_back( server );
}

void PMYouServers::addPackageVersion( const string &pkgName, string &url )
{
  PMSelectablePtr selectable = Y2PM::packageManager().getItem( pkgName );
  if ( !selectable ) {
    WAR << pkgName << " is not installed." << endl;
  } else {
    PMPackagePtr pkg = selectable->installedObj();
    if ( !pkg ) {
      WAR << "No installed object for " << pkgName << endl;
    } else {
      url += "&" + pkgName + "=" + pkg->edition().asString();
    }
  }
}

list<PMYouServer> PMYouServers::servers()
{
  if ( _servers.size() == 0 ) {
    list<PMYouServer> servers;
    servers.push_back( defaultServer() );
    return servers;
  } else {
    return _servers;
  }
}

PMYouServer PMYouServers::defaultServer()
{
  if ( _servers.size() == 0 ) {
    PMYouServer server;
    if ( _patchPaths->primaryProduct()->businessProduct() ) {
      server.setUrl( "http://sdb.suse.de/download/" );
    } else {
      server.setUrl( "ftp://ftp.suse.com/pub/suse/" );
    }
    return server;
  } else {
    return *_servers.begin();
  }
}

PMYouServer PMYouServers::currentServer()
{
  return *(servers().begin());
}

string PMYouServers::encodeUrl( const string &url )
{
  D__ << url << endl;

  string result;
  
  string::const_iterator it;
  for( it = url.begin(); it != url.end(); ++it ) {
    switch ( *it ) {
      case ' ':
        result += "%20";
        break;
      default:
        result += *it;
    }
  }

  D__ << result << endl;

  return result;
}
