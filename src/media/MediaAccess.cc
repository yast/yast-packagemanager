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

   File:	MediaAccess.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Handle access to a medium

/-*/

#include <ctype.h>

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>

#include <y2pm/MediaAccess.h>
#include <y2pm/MediaHandler.h>

#include <y2pm/MediaCD.h>
#include <y2pm/MediaDIR.h>
#include <y2pm/MediaDISK.h>
#include <y2pm/MediaNFS.h>
#include <y2pm/MediaSMB.h>
#include <y2pm/MediaCIFS.h>
#include <y2pm/MediaCurl.h>

using namespace std;

IMPL_BASE_POINTER(MediaAccess);

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaAccess
//
///////////////////////////////////////////////////////////////////

const Pathname MediaAccess::_noPath; // empty path

///////////////////////////////////////////////////////////////////
// constructor
MediaAccess::MediaAccess ()
    : _handler (0)
{
}

// destructor
MediaAccess::~MediaAccess()
{
  DBG << *this << endl;
  close(); // !!! make sure handler gets properly deleted.
}

// open URL
PMError
MediaAccess::open (const Url& url, const Pathname & preferred_attach_point)
{
    if(!url.isValid()) {
        ERR << Error::E_bad_url << " opening " << url << endl;
	return Error::E_bad_url;
    }

    close();

    switch ( url.protocol() ) {
      case Url::cd:
      case Url::dvd:
        _handler = new MediaCD (url,preferred_attach_point);
        break;
      case Url::nfs:
        _handler = new MediaNFS (url,preferred_attach_point);
        break;
      case Url::file:
      case Url::dir:
        _handler = new MediaDIR (url,preferred_attach_point);
        break;
      case Url::hd:
        _handler = new MediaDISK (url,preferred_attach_point);
        break;
      case Url::smb:
        _handler = new MediaSMB (url,preferred_attach_point);
        break;
      case Url::cifs:
        _handler = new MediaCIFS (url,preferred_attach_point);
        break;
      case Url::ftp:
      case Url::http:
      case Url::https:
        _handler = new MediaCurl (url,preferred_attach_point);
        break;
      case Url::unknown:
	ERR << Error::E_bad_media_type << " opening " << url << endl;
	return Error::E_bad_media_type;
	break;
    }

    // check created handler
    if ( !_handler ){
      ERR << "Failed to create media handler" << endl;
      return Error::E_system;
    }

    MIL << "Opened: " << *this << endl;
    return Error::E_ok;
}

// Type of media if open, otherwise NONE.
Url::Protocol
MediaAccess::protocol() const
{
  if ( !_handler )
    return Url::unknown;

  return _handler->protocol();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaAccess::url
//	METHOD TYPE : Url
//
Url MediaAccess::url() const
{
  if ( !_handler )
    return Url();

  return _handler->url();
}

// close handler
PMError
MediaAccess::close ()
{
  DBG << "handler " << _handler << endl;
  ///////////////////////////////////////////////////////////////////
  // !!! make shure handler gets properly deleted.
  // I.e. release attached media before deleting the handler.
  ///////////////////////////////////////////////////////////////////
  PMError err;
  if ( _handler ) {
    err = _handler->release();
    (err?WAR:MIL) << "Close: " << *this << " (" << err << ")" << endl;
    delete _handler;
    _handler = 0;
  }
  return err;
}


// attach media
PMError
MediaAccess::attach (bool next)
{
  if ( !_handler ) {
    INT << Error::E_not_open << endl;
    return Error::E_not_open;
  }
  return _handler->attach(next);
}

// True if media is open and attached.
bool
MediaAccess::isAttached() const
{
  return( _handler && _handler->isAttached() );
}

// local directory that corresponds to medias url
// If media is not open an empty pathname.
const Pathname &
MediaAccess::localRoot() const
{
  if ( !_handler )
    return _noPath;

  return _handler->localRoot();
}

// Short for 'localRoot() + pathname', but returns an empty
// * pathname if media is not open.
Pathname
MediaAccess::localPath( const Pathname & pathname ) const
{
  if ( !_handler )
    return _noPath;

  return _handler->localPath( pathname );
}

PMError
MediaAccess::disconnect()
{
  if ( !_handler )
    return Error::E_not_open;

  return _handler->disconnect();
}

// release attached media
PMError
MediaAccess::release( bool eject )
{
  if ( !_handler )
    return Error::E_ok;

  return _handler->release( eject );
}


// provide file denoted by path to attach dir
//
// filename is interpreted relative to the attached url
// and a path prefix is preserved to destination
PMError
MediaAccess::provideFile( const Pathname & filename, bool cached ) const
{
  if ( cached ) {
    PathInfo pi( localPath( filename ) );
    if ( pi.isExist() )
      return Error::E_ok;
  }

  if ( !_handler ) {
    INT << Error::E_not_open << " on provideFile(" << filename << ")" << endl;
    return Error::E_not_open;
  }

  return _handler->provideFile( filename );
}

PMError
MediaAccess::releaseFile( const Pathname & filename ) const
{
  if ( !_handler )
    return Error::E_ok;

  return _handler->releaseFile( filename );
}

// provide directory tree denoted by path to attach dir
//
// dirname is interpreted relative to the attached url
// and a path prefix is preserved to destination
PMError
MediaAccess::provideDir( const Pathname & dirname ) const
{
  if ( !_handler ) {
    INT << Error::E_not_open << " on provideDir(" << dirname << ")" << endl;
    return Error::E_not_open;
  }

  return _handler->provideDir( dirname );
}

PMError
MediaAccess::provideDirTree( const Pathname & dirname ) const
{
  if ( !_handler ) {
    INT << Error::E_not_open << " on provideDirTree(" << dirname << ")" << endl;
    return Error::E_not_open;
  }

  return _handler->provideDirTree( dirname );
}

PMError
MediaAccess::releaseDir( const Pathname & dirname ) const
{
  if ( !_handler )
    return Error::E_ok;

  return _handler->releaseDir( dirname );
}

PMError
MediaAccess::releasePath( const Pathname & pathname ) const
{
  if ( !_handler )
    return Error::E_ok;

  return _handler->releasePath( pathname );
}

// Return content of directory on media
PMError
MediaAccess::dirInfo( list<string> & retlist, const Pathname & dirname, bool dots ) const
{
  retlist.clear();

  if ( !_handler ) {
    INT << Error::E_not_open << " on dirInfo(" << dirname << ")" << endl;
    return Error::E_not_open;
  }

  return _handler->dirInfo( retlist, dirname, dots );
}

// Return content of directory on media
PMError
MediaAccess::dirInfo( PathInfo::dircontent & retlist, const Pathname & dirname, bool dots ) const
{
  retlist.clear();

  if ( !_handler ) {
    INT << Error::E_not_open << " on dirInfo(" << dirname << ")" << endl;
    return Error::E_not_open;
  }

  return _handler->dirInfo( retlist, dirname, dots );
}

std::ostream &
MediaAccess::dumpOn( std::ostream & str ) const
{
  if ( !_handler )
    return str << "MediaAccess( closed )";

  string tstr = Url::protocolToString( _handler->protocol() );
  str << tstr << "(" << *_handler << ")";
  return str;
}

PMError MediaAccess::getFile( const Url &from, const Pathname &to )
{
  D__ << "From: " << from << endl << "To: " << to << endl;

  Pathname path = from.path();
  Pathname dir = path.dirname();
  string base = path.basename();

  Url u = from;
  u.setPath( dir.asString() );

  MediaAccess media;

  PMError error = media.open( u );
  if ( error ) return error;

  error = media.attach();
  if ( error ) return error;

  error = media.provideFile( base );
  if ( error ) return error;

  if ( PathInfo::copy( media.localPath( base ), to ) != 0 ) {
    return MediaError::E_write_error;
  }

  error = media.release();

  return error;
}

///////////////////////////////////////////////////////////////////

