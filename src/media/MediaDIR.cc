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

   File:	MediaDIR.cc

   Author:	Klaus Kaempf <kkaempf@suse.de>
   Maintainer:	Klaus Kaempf <kkaempf@suse.de>

   Purpose:	Implementation class for MediaHandler
		This class handles access to a directory
/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2pm/MediaDIR.h>

#include <sys/mount.h>
#include <errno.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : MediaDIR
//
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::MediaDIR
//	METHOD TYPE : Constructor
//
//	DESCRIPTION : Attach point is always '/', as files are not copied.
//                    Thus attach_point_hint_r is ignored.
//
MediaDIR::MediaDIR( const Url &      url_r,
		    const Pathname & /*attach_point_hint_r*/ )
    : MediaHandler( url_r, url_r.path(),
		    false, // attachPoint_is_mediaroot
		    false ) // does_download
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::attachTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that not already attached, and attachPoint is a directory.
//
PMError MediaDIR::attachTo(bool next)
{
    if(next)
	return Error::E_not_supported_by_media;
  return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::releaseFrom
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaDIR::releaseFrom( bool eject )
{
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::getFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaDIR::getFile( const Pathname & filename ) const
{
  return MediaHandler::getFile( filename );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : MediaDIR::getDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached.
//
PMError MediaDIR::getDir( const Pathname & dirname, bool recurse_r ) const
{
  return MediaHandler::getDir( dirname, recurse_r );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaDIR::getDirInfo( std::list<std::string> & retlist,
			      const Pathname & dirname, bool dots ) const
{
  return MediaHandler::getDirInfo( retlist, dirname, dots );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : MediaDIR::getDirInfo
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Asserted that media is attached and retlist is empty.
//
PMError MediaDIR::getDirInfo( PathInfo::dircontent & retlist,
			   const Pathname & dirname, bool dots ) const
{
  return MediaHandler::getDirInfo( retlist, dirname, dots );
}
