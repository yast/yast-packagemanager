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
#include <sstream>

#include <y2util/Y2SLog.h>
#include <y2pm/MediaHandler.h>

using namespace std;

// use directory.yast on every media (not just via ftp/http)
#define NONREMOTE_DIRECTORY_YAST 1

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
//	INTERNAL FUNCTION
//	NAME: limitFileNamesInPath
//      DESCR: Looks into pathname and looks if any path component
//             is longer than maxLen. If it is, this component
//             is replaced by a shortened one. The shortened 
//             Pathname is returned.

static Pathname limitFileNamesInPath(const Pathname &pathname, int maxLen)
{
    using namespace std;
    if (maxLen > -1 && pathname.asString().length() > maxLen) {
        string base = pathname.basename();
        if (base.length() > maxLen) {
            // too long. This will probably not happen very often,
            // so this is not time critical.
            //
            // we will build the result as follows:
            // first n chars + '~xxxx~' + last m chars
            // such that total length will be below maxLen
            // xxxx is a cheap hash of the chars cut away to
            // guarantee (more or less ;-) uniqueness
            // use 8 chars less to accomodate for adding the hash.
            int cutAwayCount = base.length() - maxLen + 8;
            int splitPos1 = maxLen / 2 - 4;
            while ( (base[splitPos1] & 0x80) && splitPos1 > 0) {
                // don't split within UTF8 encoding
                --splitPos1;
            }
            int splitPos2 = splitPos1 + cutAwayCount;
            while ( (base[splitPos2] & 0x80) && splitPos2 < maxLen-1) {
                // don't split within UTF8 encoding
                ++splitPos2;
            }
            int sum = 0;
            for (int i=splitPos1; i<splitPos2; ++i) {
                sum += base[i];
            }
            ostringstream os;
            os << base.substr(0,splitPos1) 
               << "~" << hex << (sum & (unsigned) 0xffff) << "~"
               << base.substr(splitPos2);
            string base2 = os.str();
            WAR << "limitFileNamesInPath: " 
                << "'" << base << "' -> '" << base2 << "'" << endl;
            base = base2;
        }
        return limitFileNamesInPath(pathname.dirname(), maxLen) + base;
    }
    else
        return pathname;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::localPath
//	METHOD TYPE : Pathname
//
Pathname MediaHandler::localPath( const Pathname & pathname ) const {
    if ( _localRoot.empty() )
        return _localRoot;

    // we must check maximum file name length
    // this is important for fetching the suseservers, the
    // url with all parameters can get too long (bug #42021)

    int maxLen = pathconf(_localRoot.asString().c_str(),_PC_NAME_MAX);
    WAR << "maxLen: " << maxLen << endl;
    return _localRoot + limitFileNamesInPath(pathname.absolutename(),maxLen);
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

  PMError err = getDir( dirname, /*recursive*/false ); // pass to concrete handler
  if ( err ) {
    WAR << "provideDir(" << dirname << "): " << err << endl;
  } else {
    MIL << "provideDir(" << dirname << ")" << endl;
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::provideDirTree
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError MediaHandler::provideDirTree( Pathname dirname ) const
{
  if ( !_isAttached ) {
    INT << Error::E_not_attached << " on provideDirTree(" << dirname << ")" << endl;
    return Error::E_not_attached;
  }

  PMError err = getDir( dirname, /*recursive*/true ); // pass to concrete handler
  if ( err ) {
    WAR << "provideDirTree(" << dirname << "): " << err << endl;
  } else {
    MIL << "provideDirTree(" << dirname << ")" << endl;
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
PMError MediaHandler::dirInfo( list<string> & retlist,
			       const Pathname & dirname, bool dots ) const
{
  retlist.clear();

  if ( !_isAttached ) {
    INT << Error::E_not_attached << " on dirInfo(" << dirname << ")" << endl;
    return Error::E_not_attached;
  }

  PMError err = getDirInfo( retlist, dirname, dots ); // pass to concrete handler
  if ( err ) {
    WAR << "dirInfo(" << dirname << "): " << err << endl;
  } else {
    MIL << "dirInfo(" << dirname << ")" << endl;
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::dirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError MediaHandler::dirInfo( PathInfo::dircontent & retlist,
			       const Pathname & dirname, bool dots ) const
{
  retlist.clear();

  if ( !_isAttached ) {
    INT << Error::E_not_attached << " on dirInfo(" << dirname << ")" << endl;
    return Error::E_not_attached;
  }

  PMError err = getDirInfo( retlist, dirname, dots ); // pass to concrete handler
  if ( err ) {
    WAR << "dirInfo(" << dirname << "): " << err << endl;
  } else {
    MIL << "dirInfo(" << dirname << ")" << endl;
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::getDirectoryYast
//	METHOD TYPE : PMError
//
PMError MediaHandler::getDirectoryYast( std::list<std::string> & retlist,
					const Pathname & dirname, bool dots ) const
{
  retlist.clear();

  PathInfo::dircontent content;
  PMError err = getDirectoryYast( content, dirname, dots );

  if ( ! err ) {
    // convert to std::list<std::string>
    for ( PathInfo::dircontent::const_iterator it = content.begin(); it != content.end(); ++it ) {
      retlist.push_back( it->name );
    }
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::getDirectoryYast
//	METHOD TYPE : PMError
//
PMError MediaHandler::getDirectoryYast( PathInfo::dircontent & retlist,
					const Pathname & dirname, bool dots ) const
{
  retlist.clear();

  // look for directory.yast
  Pathname dirFile = dirname + "directory.yast";
  PMError err = getFile( dirFile );
  DBG << "provideFile(" << dirFile << "): " << err << endl;

  if ( err ) {
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
    if ( line.empty() ) continue;
    if ( line == "directory.yast" ) continue;

    // Newer directory.yast append '/' to directory names
    // Remaining entries are unspecified, although most probabely files.
    PathInfo::file_type type = PathInfo::NOT_AVAIL;
    if ( *line.rbegin() == '/' ) {
      line.erase( line.end()-1 );
      type = PathInfo::T_DIR;
    }

    if ( dots ) {
      if ( line == "." || line == ".." ) continue;
    } else {
      if ( *line.begin() == '.' ) continue;
    }

    retlist.push_back( PathInfo::direntry( line, type ) );
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
//	METHOD NAME : MediaHandler::getDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//                    Default implementation of pure virtual.
//
PMError MediaHandler::getDir( const Pathname & dirname, bool recurse_r ) const
{
  PathInfo info( localPath( dirname ) );
  if( info.isDir() ) {
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

#if NONREMOTE_DIRECTORY_YAST
  // use directory.yast if available
  PMError err = getDirectoryYast( retlist, dirname, dots );
  if ( ! err ) {
    return Error::E_ok;
  }
#endif

  // readdir
  int res = PathInfo::readdir( retlist, info.path(), dots );
  if ( res )
    return Error::E_system;

  return Error::E_ok;
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
PMError MediaHandler::getDirInfo( PathInfo::dircontent & retlist,
				  const Pathname & dirname, bool dots ) const
{
  PathInfo info( localPath( dirname ) );
  if( ! info.isDir() ) {
    return Error::E_not_a_directory;
  }

#if NONREMOTE_DIRECTORY_YAST
  // use directory.yast if available
  PMError err = getDirectoryYast( retlist, dirname, dots );
  if ( ! err ) {
    return Error::E_ok;
  }
#endif

  // readdir
  int res = PathInfo::readdir( retlist, info.path(), dots );
  if ( res )
    return Error::E_system;

  return Error::E_ok;
}

