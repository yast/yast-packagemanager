/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

   File:	MediaSMB.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to SMB media
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/Mount.h>
#include <y2pm/MediaSMB.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaSMB
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::MediaSMB
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaSMB::MediaSMB( const Url &      url_r,
		    const Pathname & attach_point_hint_r )
    : MediaHandler( url_r, attach_point_hint_r,
		    false, // attachPoint_is_mediaroot
		    false ) // does_download
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::attachTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that not already attached, and attachPoint is a directory.
//
PMError MediaSMB::attachTo(bool next)
{
    if(_url.host().empty())
	    return Error::E_no_host_specified;
    if(next)
	return Error::E_not_supported_by_media;

    const char* const filesystem = "smbfs";
    const char *mountpoint = attachPoint().asString().c_str();
    Mount mount;
    PMError ret;

    string path = "//";
    path += _url.host();
    path += _url.path();

    Mount::Options options( _url.option("mountoptions") );
    string username = _url.username();
    string password = _url.password();

    options["guest"]; // prevent smbmount from asking for password

    if ( ! options.has( "rw" ) ) {
      options["ro"];
    }

    Mount::Options::iterator toEnv;

    // extract 'username', do not overwrite any _url.username
    toEnv = options.find("username");
    if ( toEnv != options.end() ) {
      if ( username.empty() )
	username = toEnv->second;
      options.erase( toEnv );
    }
    toEnv = options.find("user"); // actually cifs specific
    if ( toEnv != options.end() ) {
      if ( username.empty() )
	username = toEnv->second;
      options.erase( toEnv );
    }

    // extract 'password', do not overwrite any _url.password
    toEnv = options.find("password");
    if ( toEnv != options.end() ) {
      if ( password.empty() )
	password = toEnv->second;
      options.erase( toEnv );
    }
    toEnv = options.find("pass"); // actually cifs specific
    if ( toEnv != options.end() ) {
      if ( password.empty() )
	password = toEnv->second;
      options.erase( toEnv );
    }

    // pass 'username' and 'password' via environment
    Mount::Environment environment;
    environment["USER"] = username;
    environment["PASSWD"] = password;

    //////////////////////////////////////////////////////
#warning waiting for mount to pass environment to smbmount
    // unless plain mount supports this we add username
    // and password again.
    options["username"] = username;
    options["password"] = password;
    //
    //////////////////////////////////////////////////////

    ret = mount.mount( path, mountpoint, filesystem,
		       options.asString(), environment );
    if(ret != Error::E_ok)
    {
	return ret;
    }

    return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::releaseFrom
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaSMB::releaseFrom( bool eject )
{
    Mount mount;
    PMError ret;

    if ((ret = mount.umount(attachPoint().asString())) != Error::E_ok)
    {
	return ret;
    }

    return ret;
}


///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaSMB::getFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaSMB::getFile (const Pathname & filename) const
{
  return MediaHandler::getFile( filename );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaSMB::getDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaSMB::getDir( const Pathname & dirname, bool recurse_r ) const
{
  return MediaHandler::getDir( dirname, recurse_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaSMB::getDirInfo( std::list<std::string> & retlist,
			      const Pathname & dirname, bool dots ) const
{
  return MediaHandler::getDirInfo( retlist, dirname, dots );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaSMB::getDirInfo( PathInfo::dircontent & retlist,
			   const Pathname & dirname, bool dots ) const
{
  return MediaHandler::getDirInfo( retlist, dirname, dots );
}
