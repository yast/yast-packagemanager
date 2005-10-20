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
#include <fstream>

#include <y2util/Y2SLog.h>
#include <y2util/TmpPath.h>
#include <y2pm/Mount.h>
#include <y2pm/MediaSMB.h>

#include <sys/types.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>

using namespace std;

/******************************************************************
**
**
**	FUNCTION NAME : getShare
**	FUNCTION TYPE : inline Pathname
**
** Get the 1st path component (CIFS share name).
*/
inline string getShare( Pathname spath_r )
{
  if ( spath_r.empty() )
    return string();

  string share( spath_r.absolutename().asString() );
  string::size_type sep = share.find( "/", 1 );
  if ( sep == string::npos )
    share = share.erase( 0, 1 ); // nothing but the share name in spath_r
  else
    share = share.substr( 1, sep-1 );

  // deescape %2f in sharename
  while ( (sep = share.find( "%2f" )) != string::npos ) {
    share.replace( sep, 3, "/" );
  }

  return share;
}

/******************************************************************
**
**
**	FUNCTION NAME : stripShare
**	FUNCTION TYPE : inline Pathname
**
** Strip off the 1st path component (CIFS share name).
*/
inline Pathname stripShare( Pathname spath_r )
{
  if ( spath_r.empty() )
    return Pathname();

  string striped( spath_r.absolutename().asString() );
  string::size_type sep = striped.find( "/", 1 );
  if ( sep == string::npos )
    return "/"; // nothing but the share name in spath_r

  return striped.substr( sep );
}

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
		    stripShare( url_r.path() ), // urlpath WITHOUT share name at attachpoint
		    false )       // does_download
    , _vfstype( "smbfs" )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaSMB::attachTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that not already attached, and attachPoint
//      is a directory.
//
//      NOTE: The implementation currently serves both, "smbfs"
//      and "cifs". The only difference is the vfstype passed to
//      the mount command.
//
PMError MediaSMB::attachTo(bool next)
{
    if(_url.host().empty())
	    return Error::E_no_host_specified;
    if(next)
	return Error::E_not_supported_by_media;

    const char *mountpoint = attachPoint().asString().c_str();
    Mount mount;
    PMError ret;

    string path = "//";
    path += _url.host() + "/" + getShare( _url.path() );

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

    // look for a workgroup
    string workgroup = _url.option("workgroup");
    if ( workgroup.size() ) {
      options["workgroup"] = workgroup;
    }

    // pass 'username' and 'password' via environment
    Mount::Environment environment;
    if ( username.size() )
      environment["USER"] = username;
    if ( password.size() )
      environment["PASSWD"] = password;

    //////////////////////////////////////////////////////
    // In case we need a tmpfile, credentials will remove
    // it in it's destructor after the mout call below.
    TmpPath credentials;
    if ( username.size() || password.size() )
      {
        TmpFile tmp;
        ofstream outs( tmp.path().asString().c_str() );
        outs << "username=" <<  username << endl;
        outs << "password=" <<  password << endl;
        outs.close();

        credentials = tmp;
        options["credentials"] = credentials.path().asString();
      }
    //
    //////////////////////////////////////////////////////

    ret = mount.mount( path, mountpoint, _vfstype,
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
