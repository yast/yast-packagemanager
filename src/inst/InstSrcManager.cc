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
//	METHOD NAME : InstSrcManager::scanProductsFile
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::scanProductsFile( const Pathname & file_r, ProductSet & pset_r ) const
{
#warning TBD Actually scan products file.
  pset_r.insert( ProductEntry() );
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::lookupId
//	METHOD TYPE : InstSrcPtr
//
//	DESCRIPTION :
//
InstSrcPtr InstSrcManager::lookupId( const ISrcId & isrc_r ) const
{
  InstSrcPtr it = InstSrcPtr::cast_away_const( isrc_r );
  if ( _knownSources.find( it ) == _knownSources.end() )
    return 0;
  return it;
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
  ProductSet products;
#if 0
  MediaAccess::ProvideFile pfile( media, "/media.1/products" );
  if ( pfile.error() ) {
    // no products file: default ProductEntry is /
    products.insert( ProductEntry() );
  } else {
    // scan products file
    MIL << "Found '/media.1/products'." << endl;
    scanProductsFile( pfile(), products );
  }
#else
  products.insert( ProductEntry() );
#endif
  ///////////////////////////////////////////////////////////////////
  // scan products found
  ///////////////////////////////////////////////////////////////////
  PMError scan_err;

  for ( ProductSet::const_iterator iter = products.begin(); iter != products.end(); ++iter ) {
    InstSrcPtr nsrc;
    Pathname   srccache( genSrcCacheName() );

    if ( (scan_err = InstSrc::vconstruct( nsrc, srccache, mediaurl_r, iter->_dir)) ) {
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
  InstSrcPtr it( lookupId( isrc_r ) );
  if ( it ) {
    return it->enableSource();
  }

  E__ << "bad ISrcId " << isrc_r << endl;
  return Error::E_bad_id;
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
  InstSrcPtr it( lookupId( isrc_r ) );
  if ( it ) {
    return it->disableSource();
  }

  E__ << "bad ISrcId " << isrc_r << endl;
  return Error::E_bad_id;
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


