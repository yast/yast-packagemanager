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
			     const bool       does_download_r,
			     MediaAccess::MediaType type_r )
    : _attachPoint( attach_point_r )
    , _tmp_attachPoint( false )
    , _attachPoint_is_mediaroot( attachPoint_is_mediaroot_r )
    , _does_download( does_download_r )
    , _type( type_r )
    , _isAttached( false )
    , _url( url_r )
{
  if ( _attachPoint.empty() ) {
    ///////////////////////////////////////////////////////////////////
    // provide a default attachpoint
    ///////////////////////////////////////////////////////////////////
#warning CHECK if fixed "/var/adm/mount" is appropriate

    Pathname aroot( "/var/adm/mount" );
    PathInfo adir( aroot );
    if ( !adir.isDir() ) {
      ERR << "Create attach point: directory does not exist: " << adir << endl;
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
      _localRoot += _url.getPath();
  }

  dumpOn( MIL ) << endl;;
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
  }

  if ( _tmp_attachPoint ) {
    int res = PathInfo::recursive_rmdir( _attachPoint );
    if ( res ) {
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
PMError MediaHandler::attach(bool next)
{
  if ( _isAttached )
    return Error::E_already_attached;

  if ( _attachPoint.empty() )
    return Error::E_bad_attachpoint;

  PMError err = attachTo(next); // pass to concrete handler
  if ( !err ) {
    _isAttached = true;
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
  if ( !_isAttached )
    return Error::E_not_attached;

  PMError err = releaseFrom( eject ); // pass to concrete handler
  if ( !err ) {
    _isAttached = false;
    DBG << "ok" << endl;
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
PMError MediaHandler::provideFile( const Pathname & filename ) const
{
  D__ << filename << endl;
  if ( !_isAttached )
    return Error::E_not_attached;

  D__ << filename.absolutename() << endl;
  return getFile( filename.absolutename() ); // pass to concrete handler
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::releaseFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError MediaHandler::releaseFile( const Pathname & filename ) const
{
  if ( ! _does_download || _attachPoint.empty() )
    return Error::E_ok;

  PathInfo info( localPath( filename.absolutename() ) );
  if ( info.isFile() ) {
    PathInfo::unlink( info.path() );
  }

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::provideDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError MediaHandler::provideDir( const Pathname & dirname ) const
{
    D__ << dirname << endl;
    if ( !_isAttached )
	return Error::E_not_attached;

    D__ << dirname.absolutename() << endl;
    std::list<std::string> filelist;
    PMError err = getDirInfo( filelist, dirname.absolutename(), false );
    if (err == PMError::E_ok)
    {
	for (std::list<std::string>::iterator it = filelist.begin();
	     it != filelist.end(); ++it)
	{
	    Pathname filename = dirname + *it;
	    err = getFile ( filename.absolutename() ); // pass to concrete handler
	    if (err != PMError::E_ok)
		break;
	}
    }
    return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::releaseDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError MediaHandler::releaseDir( const Pathname & dirname ) const
{
  return releasePath (dirname);
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::releasePath
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError MediaHandler::releasePath( const Pathname & pathname ) const
{
  if ( ! _does_download || _attachPoint.empty() )
    return Error::E_ok;

  PathInfo info( localPath( pathname.absolutename() ) );
  if ( info.isDir() ) {
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
PMError MediaHandler::dirInfo( std::list<std::string> & retlist,
			       const Pathname & dirname, bool dots ) const
{
  retlist.clear();

  if ( !_isAttached )
    return Error::E_not_attached;

  return getDirInfo( retlist, dirname.absolutename(), dots );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaHandler::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream &
MediaHandler::dumpOn( ostream & str ) const
{
  str << _url << ( _isAttached ? "" : " not" ) << " attached; localRoot \""
    << _localRoot << "\"";
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
  D__ << filename << endl;
  PathInfo info( localPath( filename ) );
  D__ << info << endl;
  if( ! info.isFile() ) {
    return Error::E_file_not_found;
  }

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

#if 0
///////////////////////////////////////////////////////////////////
// PROTECTED
//
//
//	METHOD NAME : MediaHandler::scanDirectory
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//	  scan directory for files matching pattern
//	  pattern might have a single trailing '*'
//	  FIXME: use proper regexp handling
//
const Pathname *
MediaHandler::scanDirectory (const Pathname & dirname, const string & pattern) const
{
    // prepend mountpoint to dirname
    Pathname *fullpath = new Pathname (_attachPoint + dirname);

    // open mounted directory
    DIR *dir = opendir (fullpath->asString().c_str());
    struct dirent *entry;
    if (dir == 0)
    {
	return 0;
    }

    // scan directory

    while ((entry = readdir (dir)) != 0)
    {
	char *dptr = entry->d_name;		// directory name pointer
	const char *pptr = pattern.c_str();		// pattern pointer

	// match pattern

	while ((*dptr != 0)
		&& (*pptr != 0))
	{
	    if (*dptr == *pptr)		// pattern matches
	    {
		dptr++;
		pptr++;
	    }
	    else if (*pptr == '*')	// wildcard matches
	    {
		pptr++;			// assume '*' at end of pattern
		break;
	    }
	    else
		break;			// no match
	}

	if (*pptr == 0)			// match !
	{
	    *fullpath += entry->d_name;
	    closedir (dir);
	    return fullpath;
	}
    }
    closedir (dir);
    return 0;		// no match
}

///////////////////////////////////////////////////////////////////
// PROTECTED
//
//
//	METHOD NAME : MediaHandler::readDirectory
//	METHOD TYPE : std::list <std::string> *
//
//	DESCRIPTION :
//	  read directory to list of strings
//	  return NULL on error
//
const list<string> *
MediaHandler::readDirectory (const Pathname & dirname) const
{
    list<string> *dirlist = new list<string>;

    // prepend mountpoint to dirname
    Pathname fullpath = _attachPoint + dirname;

    // open mounted directory
    DIR * dir = opendir (fullpath.asString().c_str());
    if ( !dir ) {
	return errno;
    }

    struct dirent * entry = 0;
    while ( (entry = readdir( dir )) != 0 )
    {
	dirlist->push_back (entry->d_name);
    }
    closedir (dir);

    return dirlist;
}
#endif
