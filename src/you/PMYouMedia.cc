/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SUSE LINUX AG |
\----------------------------------------------------------------------/

  File:       PMYouMedia.cc

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Provide YOU media information.

/-*/

#include <y2pm/PMYouMedia.h>

#include <y2util/Y2SLog.h>

#include <fstream>

#include <stdlib.h>

using namespace std;

IMPL_BASE_POINTER(PMYouMedia);

PMYouMedia::PMYouMedia( MediaAccess &media )
  : _media( media ), _number( -1 ), _count( 0 )
{
}

PMYouMedia::~PMYouMedia()
{
}

PMError PMYouMedia::readInfo( int number )
{
  PMError error;

  _number = -1;
  _vendor = "";
  _id = "";
  _count = 0;
  _flags = "";

  Pathname mediafile( stringutil::form( "/media.%d/media", number ) );

  error = _media.provideFile( mediafile );
  if ( error ) {
    WAR << "Can't provide file '" << mediafile << endl;
    return error;
  } else {
    // open media file
    std::ifstream mediaf( _media.localPath( mediafile ).asString().c_str() );
    if ( !mediaf ) {
      ERR << "Can't open file '" << _media.localPath( mediafile ) << "'"
          << endl;
      return PMError( MediaError::E_system, "Unable to open file." );
    } else {
      if ( !getline( mediaf, _vendor ) ) {
        ERR << "Can't get vendor" << endl;
        return PMError( MediaError::E_system, "Unable to read vendor." );
      }
      if ( !getline( mediaf, _id ) ) {
        ERR << "Can't get id" << endl;
        return PMError( MediaError::E_system, "Unable to read id." );
      }
      string countStr;
      if ( getline( mediaf, countStr ) ) {
        _count = atoi( countStr.c_str() );

        getline( mediaf, _flags );
      }
      mediaf.close();
    }
    MIL << "vendor: '" << _vendor.c_str() << "' id: '" << _id.c_str() << "' "
        << "count: " << _count << " flags: '" << _flags.c_str() << "'" << endl;
  }

  _number = number;

  return PMError::E_ok;
}

bool PMYouMedia::isPartOf( const PMYouMediaPtr &mediaInstance )
{
  if ( mediaInstance->vendor() != _vendor || mediaInstance->id() != _id ) {
    return false;
  }
  
  return true;
}

bool PMYouMedia::doubleSided() const
{
  return _flags == "doublesided";
}
