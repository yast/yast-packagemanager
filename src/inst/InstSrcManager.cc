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

   File:       InstSrcManager.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>

#include <y2pm/InstSrcManager.h>
#include <y2pm/InstSrc.h>

#include <y2pm/MediaAccess.h>
#include <y2pm/PMPackageManager.h>

using namespace std;

///////////////////////////////////////////////////////////////////

#warning FIX _CACHE_ROOT_DIR ( "/tmp/test_test" )
Pathname InstSrcManager::_cache_root_dir( "/tmp/test_test" );

const Pathname InstSrcManager::_cache_tmp_dir( "tmp" );

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::InstSrcManager
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstSrcManager::InstSrcManager()
{
  int res = PathInfo::assert_dir( cache_tmp_dir(), 0700 );
  if ( res ) {
    ERR << "Unable to create cache " << cache_tmp_dir() << " (errno " << res << ")" << endl;
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::~InstSrcManager
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcManager::~InstSrcManager()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::genSrcCacheName
//	METHOD TYPE : Pathname
//
//	DESCRIPTION :
//
Pathname InstSrcManager::genSrcCacheName() const
{
  Pathname base( cache_root_dir() + "IS_CACHE_" );
  Pathname ret;
  for ( unsigned i = 1; i < 1000; ++i ) {
    ret = Pathname::extend( base, stringutil::hexstring( i ) );
    PathInfo np( ret );
    if ( !np.isExist() )
      return ret;
  }
  INT << "Unable to getSrcCacheName in " << cache_root_dir() << endl;
  return Pathname();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::scanMedia
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::scanMedia( ISrcIdList & idlist_r, const Url & mediaurl_r )
{
  idlist_r.clear();
  PMError err;

  MIL << "scanMedia " << mediaurl_r << endl;

  ///////////////////////////////////////////////////////////////////
  // prepare media
  ///////////////////////////////////////////////////////////////////
  MediaAccessPtr  media = new MediaAccess;
  if ( (err = media->open( mediaurl_r, cache_tmp_dir() )) ) {
    ERR << "Failed to open " << mediaurl_r << " " << err << endl;
    return err;
  }

  if ( (err = media->attach()) ) {
    ERR << "Failed to attach media: " << err << endl;
    return err;
  }

  ///////////////////////////////////////////////////////////////////
  // look if there's a /media.1/products file
  ///////////////////////////////////////////////////////////////////
  list<Pathname> plist;
  PMError scan_err;

  if ( (scan_err = media->provideFile( "/media.1/products" )) ) {
    // no products file
    plist.push_back( "" );
  } else {
    // scan products file
    MIL << "Found '/media.1/products'." << endl;
#warning TBD scan /media.1/products
    plist.push_back( "" );
  }

  scan_err = Error::E_ok;
  for ( list<Pathname>::const_iterator iter = plist.begin(); iter != plist.end(); ++iter ) {
    InstSrcPtr nsrc;
    Pathname   srccache( genSrcCacheName() );

    if ( (scan_err = InstSrc::vconstruct( nsrc, srccache, mediaurl_r, *iter, InstSrc::T_TEST_DIST )) ) {
      // no InstSrc found
    } else {
#warning TBD prevent duplicate registration of sources
      // add new source to known_Sources and idlist_r
      _knownSources.insert( nsrc );
      idlist_r.push_back( nsrc );
    }
  }

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  DBG << "scanMedia " << mediaurl_r << " found " << idlist_r.size() << " InstSrc(es)" << endl;
  return err;
}

#if 0
PMError InstSrcManager::enableSource( InstSrcPtr & isrc_r )
{
  D__ << endl;
  if(isrc_r->Activate())
  {
    PMPackageManager::PM().addPackages( isrc_r->getPackages() );
    return E_ok;
  }
  return E_error;
}
#endif

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::enableSource
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::enableSource( const ISrcId & isrc_r )
{
  PMError err;

  // close you're eyes
  if ( isrc_r )
    return const_cast<InstSrc*>(isrc_r.operator->())->enableSource();

  err = Error::E_TBD;

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::disableSource
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::disableSource( const ISrcId & isrc_r )
{
  PMError err;

  // close you're eyes
  if ( isrc_r )
    return const_cast<InstSrc*>(isrc_r.operator->())->disableSource();

  err = Error::E_TBD;

  return err;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const InstSrcManager & obj )
{
  return str;
}


