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
#include <y2pm/MediaWget.h>

using namespace std;

IMPL_BASE_POINTER(MediaAccess);

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaAccess
//
///////////////////////////////////////////////////////////////////

inline MediaAccess::ProtocolTypes MediaAccess::_init_protocolTypes()
{
  ProtocolTypes ret;

  ret.insert( ProtocolTypes::value_type( "cd",    CD ) );
  ret.insert( ProtocolTypes::value_type( "dvd",   DVD ) );
  ret.insert( ProtocolTypes::value_type( "nfs",   NFS ) );
  ret.insert( ProtocolTypes::value_type( "dir",   DIR ) );
  ret.insert( ProtocolTypes::value_type( "hd",    DISK ) );
  ret.insert( ProtocolTypes::value_type( "ftp",   FTP ) );
  ret.insert( ProtocolTypes::value_type( "smb",   SMB ) );
  ret.insert( ProtocolTypes::value_type( "http",  HTTP ) );
  ret.insert( ProtocolTypes::value_type( "https", HTTPS ) );

  return ret;
}

const MediaAccess::ProtocolTypes MediaAccess::protocolTypes( _init_protocolTypes() );

const Pathname MediaAccess::_noPath; // empty path

inline MediaAccess::MediaType MediaAccess::typeOf( const Url & url_r )
{
  ProtocolTypes::const_iterator t = protocolTypes.find( stringutil::toLower( url_r.getProtocol() ) );
  if ( t == protocolTypes.end() )
    return NONE;
  return t->second;
}

///////////////////////////////////////////////////////////////////
// constructor
MediaAccess::MediaAccess ()
    : _handler (0)
{
}

// destructor
MediaAccess::~MediaAccess()
{
  close(); // !!! make shure handler gets properly deleted.
}

// open URL
PMError
MediaAccess::open (const Url& url, const Pathname & preferred_attach_point)
{
    if(!url.isValid())
	return Error::E_bad_url;

    close();

    MediaType utype = typeOf( url );

    switch ( utype ) {
    case CD:
      _handler = new MediaCD (url,preferred_attach_point,utype);
      break;
    case DVD:
      _handler = new MediaCD (url,preferred_attach_point,utype);
      break;
    case NFS:
      _handler = new MediaNFS (url,preferred_attach_point,utype);
      break;
    case DIR:
      _handler = new MediaDIR (url,preferred_attach_point,utype);
      break;
    case DISK:
       _handler = new MediaDISK (url,preferred_attach_point,utype);
     break;
    case FTP:
      _handler = new MediaWget (url,preferred_attach_point,utype);
      break;
    case SMB:
      _handler = new MediaSMB (url,preferred_attach_point,utype);
      break;
    case HTTP:
      _handler = new MediaWget (url,preferred_attach_point,utype);
      break;
    case HTTPS:
      _handler = new MediaWget (url,preferred_attach_point,utype);
      break;

    case NONE:
	return Error::E_bad_media_type;
	break;
    }

    // check created handler
    if ( !_handler ){
      return Error::E_system;
    }

    return Error::E_ok;
}

// Type of media if open, otherwise NONE.
MediaAccess::MediaType
MediaAccess::type() const
{
  if ( !_handler )
    return NONE;

  return _handler->type();
}

// close handler
void
MediaAccess::close (void)
{
  ///////////////////////////////////////////////////////////////////
  // !!! make shure handler gets properly deleted.
  // I.e. release attached media befire deleting the handler.
  ///////////////////////////////////////////////////////////////////
  if ( _handler ) {
    if ( _handler->isAttached() )
      _handler->release();
    delete _handler;
    _handler = 0;
  }
}


// attach media
PMError
MediaAccess::attach (bool next)
{
  if ( !_handler )
    return Error::E_not_open;

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

// release attached media
PMError
MediaAccess::release( bool eject )
{
  if ( !_handler )
    return Error::E_not_open;

  return _handler->release( eject );
}


// provide file denoted by path to attach dir
//
// filename is interpreted relative to the attached url
// and a path prefix is preserved to destination
PMError
MediaAccess::provideFile (const Pathname & filename) const
{
  if ( !_handler )
    return Error::E_not_open;

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
MediaAccess::provideDir (const Pathname & dirname) const
{
  if ( !_handler )
    return Error::E_not_open;

  return _handler->provideDir( dirname );
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
MediaAccess::dirInfo( std::list<std::string> & retlist,
		      const Pathname & dirname, bool dots ) const
{
  retlist.clear();

  if ( !_handler )
    return Error::E_not_open;

  return _handler->dirInfo( retlist, dirname, dots );
}

#if 0

// find file denoted by pattern
//
// filename is interpreted relative to the attached url
// pattern might have a single trailing '*'
//
const Pathname *
MediaAccess::findFile (const Pathname & dirname, const string & pattern) const
{
    if (_handler == 0)
    {
	return 0;  //Error::E_not_open;
    }
    if (pattern.empty())
    {
	return 0;
    }
    if (_handler->getAttachPoint().empty())
    {
	return 0; //Error::E_not_attached;
    }
    return _handler->findFile (dirname, pattern);
}


// get file information
const PathInfo *
MediaAccess::fileInfo (const Pathname & filename) const
{
    if (_handler == 0)
    {
	return 0;
    }
    if (_handler->getAttachPoint().empty())
    {
	return 0; //Error::E_not_attached;
    }
    return _handler->fileInfo (filename);
}


// clean up a file from destination
// if filename == "", the whole destination is cleared
PMError
MediaAccess::cleanUp (const Pathname & filename) const
{
    Pathname fullname = _handler->getAttachPoint() + filename;
    PathInfo info (fullname);

    if (info.isDir())
    {
	// rm -rf
    }
    else
    {
	unlink (fullname.asString().c_str());
    }
    return Error::E_ok;
}
#endif


std::ostream &
MediaAccess::dumpOn( std::ostream & str ) const
{
  if ( ! isOpen() )
    return str << "MediaAccess( closed )";

  const char * tstr = "???"; // default for unknown types
  switch ( _handler->type() ) {
  case CD:    tstr = "CD";    break;
  case DVD:   tstr = "DVD";   break;
  case NFS:   tstr = "NFS";   break;
  case DIR:   tstr = "DIR";   break;
  case DISK:  tstr = "DISK";  break;
  case FTP:   tstr = "FTP";   break;
  case SMB:   tstr = "SMB";   break;
  case HTTP:  tstr = "HTTP";  break;
  case HTTPS: tstr = "HTTPS"; break;
  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  case NONE:
    break;
  }
  str << tstr << "(";
  _handler->dumpOn( str );
  return str << ")";
}

///////////////////////////////////////////////////////////////////

