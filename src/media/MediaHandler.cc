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

   File:	MediaHandler.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Abstract base class for 'physical' media access

/-*/

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>
#include <y2pm/MediaHandler.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaHandler
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::MediaHandler
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
MediaHandler::MediaHandler ( const Url &      url_r,
			     const Pathname & attach_point_r,
			     const bool       attachPoint_is_mediaroot_r,
			     const bool       does_download_r )
    : _attachPoint( attach_point_r )
    , _tmp_attachPoint( false )
    , _attachPoint_is_mediaroot( attachPoint_is_mediaroot_r )
    , _does_download( does_download_r )
    , _isAttached( false )
    , _url( url_r )
{
  if ( _attachPoint.empty() ) {
    ///////////////////////////////////////////////////////////////////
    // provide a default attachpoint
    ///////////////////////////////////////////////////////////////////

    Pathname aroot;
    PathInfo adir;
    const char * defmounts[] = { "/var/adm/mount", "/var/tmp", /**/NULL/**/ };
    for ( const char ** def = defmounts; *def; ++def ) {
      adir( *def );
      if ( adir.isDir() && adir.userMayRWX() ) {
	aroot = adir.path();
	break;
      }
    }
    if ( aroot.empty() ) {
      ERR << "Create attach point: Can't find a writable directory to create an attach point" << endl;
      return;
    }

    Pathname abase( aroot + "AP_" );
    Pathname apoint;

    for ( unsigned i = 1; i < 1000; ++i ) {
      adir( Pathname::extend( abase, stringutil::hexstring( i ) ) );
      if ( ! adir.isExist() && PathInfo::mkdir( adir.path() ) == 0 ) {
	apoint = adir.path();
	break;
      }
    }

    if ( apoint.empty() ) {
      ERR << "Unable to create an attach point below " << aroot << endl;
      return;
    }

    // success
    _attachPoint = apoint;
    _tmp_attachPoint = true;
    MIL << "Created default attach point " << _attachPoint << endl;

  } else {
    ///////////////////////////////////////////////////////////////////
    // check if provided attachpoint is usable.
    ///////////////////////////////////////////////////////////////////
    PathInfo adir( _attachPoint );
    if ( !adir.isDir() ) {
      ERR << "Provided attach point is not a directory: " << adir << endl;
      _attachPoint = Pathname();
    }

  }

  ///////////////////////////////////////////////////////////////////
  // must init _localRoot after _attachPoint is determined.
  ///////////////////////////////////////////////////////////////////

  if ( !_attachPoint.empty() ) {
    _localRoot = _attachPoint;
    if ( _attachPoint_is_mediaroot )
      _localRoot += _url.path();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::~MediaHandler
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
MediaHandler::~MediaHandler()
{
  if ( _isAttached ) {
    INT << "MediaHandler deleted with media attached." << endl;
    return; // no cleanup if media still mounted!
  }

  if ( _tmp_attachPoint ) {
    int res = PathInfo::recursive_rmdir( _attachPoint );
    if ( res == 0 ) {
      MIL << "Deleted default attach point " << _attachPoint << endl;
    } else {
      ERR << "Failed to Delete default attach point " << _attachPoint
	<< " errno(" << res << ")" << endl;
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::attach
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError MediaHandler::attach( bool next )
{
  if ( _isAttached )
    return Error::E_ok;

  if ( _attachPoint.empty() ) {
    ERR << Error::E_bad_attachpoint << endl;
    return Error::E_bad_attachpoint;
  }

  PMError err = attachTo( next ); // pass to concrete handler
  if ( err ) {
    WAR << "Attach failed: " << err << " " << *this << endl;
  } else {
    _isAttached = true;
    MIL << "Attached: " << *this << endl;
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::disconnect
//	METHOD TYPE : PMError
//
PMError MediaHandler::disconnect()
{
  if ( !_isAttached )
    return  Error::E_ok;

  PMError err = disconnectFrom(); // pass to concrete handler
  if ( err ) {
    WAR << "Disconnect failed: " << err << " " << *this << endl;
  } else {
    MIL << "Disconnected: " << *this << endl;
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::release
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError MediaHandler::release( bool eject )
{
  if ( !_isAttached ) {
    if ( eject )
      forceEject();
    return Error::E_ok;
  }

  PMError err = releaseFrom( eject ); // pass to concrete handler
  if ( err ) {
    WAR << "Release failed: " << err << " " << *this << endl;
  } else {
    _isAttached = false;
    MIL << "Released: " << *this << endl;
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::provideFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError MediaHandler::provideFile( Pathname filename ) const
{
  if ( !_isAttached ) {
    INT << Error::E_not_attached << " on provideFile(" << filename << ")" << endl;
    return Error::E_not_attached;
  }

  filename = filename.absolutename();

  PMError err = getFile( filename ); // pass to concrete handler
  if ( err ) {
    WAR << "provideFile(" << filename << "): " << err << endl;
  } else {
    MIL << "provideFile(" << filename << ")" << endl;
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::provideDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError MediaHandler::provideDir( Pathname dirname ) const
{
  if ( !_isAttached ) {
    INT << Error::E_not_attached << " on provideDir(" << dirname << ")" << endl;
    return Error::E_not_attached;
  }

  dirname = dirname.absolutename();

  list<string> filelist;
  PMError err = getDirInfo( filelist, dirname, false );
  if ( err ) {
    WAR << "provideDir(" << dirname << "): " << err << endl;
  } else {
    MIL << "provideDir(" << dirname << ")" << endl;

    for ( list<string>::iterator it = filelist.begin(); it != filelist.end(); ++it ) {

      Pathname filename = dirname + *it;
      PMError res = getFile( filename ); // pass to concrete handler
      switch ( res ) {
      case Error::E_ok:
	DBG << "provideDir: file(" << filename << ")" << endl;
	break;
      case Error::E_not_a_file:
	DBG << "provideDir: file(" << filename << "): " << res << " SKIPPED" << endl;
	break;
      case Error::E_file_not_found:
	// might be incorrect directory.yast
	WAR << "provideDir: file(" << filename << "): " << res << endl;
	break;
      default:
	ERR << "provideDir: file(" << filename << "): " << res << " ABORTING " << endl;
	err = res;
	break;
      }
      if ( err )
	break; // abort
    }
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::releasePath
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError MediaHandler::releasePath( Pathname pathname ) const
{
  if ( ! _does_download || _attachPoint.empty() )
    return Error::E_ok;

  pathname = pathname.absolutename();
  PathInfo info( localPath( pathname ) );

  if ( info.isFile() ) {
    PathInfo::unlink( info.path() );
  } else if ( info.isDir() ) {
    if ( info.path() != _localRoot ) {
      PathInfo::recursive_rmdir( info.path() );
    } else {
      PathInfo::clean_dir( info.path() );
    }
  }

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::dirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError MediaHandler::dirInfo( list<string> & retlist, Pathname dirname, bool dots ) const
{
  retlist.clear();
  if ( !_isAttached ) {
    INT << Error::E_not_attached << " on dirInfo(" << dirname << ")" << endl;
    return Error::E_not_attached;
  }

  dirname = dirname.absolutename();

  // look for directory.yast
  Pathname dirFile = dirname + "directory.yast";
  PMError err = getFile( dirFile );
  DBG << "provideFile(" << dirFile << "): " << err << endl;

  if ( err ) {
    err = getDirInfo( retlist, dirname, dots ); // pass to concrete handler
    if ( err ) {
      WAR << "dirInfo(" << dirname << "): " << err << endl;
    } else {
      MIL << "dirInfo(" << dirname << ")" << endl;
    }

    return err;
  }

  // using directory.yast
  ifstream dir( localPath( dirFile ).asString().c_str() );
  if ( dir.fail() ) {
    ERR << "Unable to load '" << localPath( dirFile ) << "'" << endl;
    return Error::E_system;
  }

  string line;
  while( getline( dir, line ) ) {
    if ( line == "directory.yast" ) continue;
    if ( dots ) {
      if ( line == "." || line == ".." ) continue;
    } else {
      if ( *line.begin() == '.' ) continue;
    }
    retlist.push_back( line );
  }

  return Error::E_ok;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const MediaHandler & obj )
{
  str << obj.url() << ( obj.isAttached() ? "" : " not" )
    << " attached; localRoot \"" << obj.localRoot() << "\"";
  return str;

}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::getFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//                    Default implementation of pure virtual.
//
PMError MediaHandler::getFile( const Pathname & filename ) const
{
  PathInfo info( localPath( filename ) );
  if( info.isFile() ) {
    return Error::E_ok;
  }

  return( info.isExist() ? Error::E_not_a_file : Error::E_file_not_found );
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//                    Default implementation of pure virtual.
//
PMError MediaHandler::getDirInfo( std::list<std::string> & retlist,
				  const Pathname & dirname, bool dots ) const
{
  PathInfo info( localPath( dirname ) );
  if( ! info.isDir() ) {
    return Error::E_not_a_directory;
  }

  int res = PathInfo::readdir( retlist, info.path(), dots );
  if ( res )
    return Error::E_system;

  return Error::E_ok;
}
