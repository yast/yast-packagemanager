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

#include <Y2PM.h>

#include <y2pm/InstSrcManager.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/InstSrcData.h>

#include <y2pm/MediaAccess.h>
#include <y2pm/PMPackageManager.h>

using namespace std;

///////////////////////////////////////////////////////////////////

Pathname InstSrcManager::_cache_root_dir( "/var/adm/YaST/InstSrcManager" );

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
InstSrcManager::InstSrcManager( const bool autoEnable_r )
{
  int res = PathInfo::assert_dir( cache_tmp_dir(), 0700 );
  if ( res ) {
    ERR << "Unable to create cache " << cache_tmp_dir() << " (errno " << res << ")" << endl;
  }
  if ( Y2PM::runningFromSystem() ) {
    initSrcPool( autoEnable_r );
  } else {
    MIL << "Not running from system: no init from SrcPool" << endl;
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
//	METHOD NAME : InstSrcManager::initSrcPool
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::initSrcPool( const bool autoEnable_r )
{
  MIL << "Read " << (autoEnable_r?"and auto enable ":"") << "SrcPool from " << cache_root_dir() << endl;

  PMError err;
  unsigned count = 0;

  ///////////////////////////////////////////////////////////////////
  // scan cachedir and add sources
  ///////////////////////////////////////////////////////////////////
  list<string> retlist;
  int res = PathInfo::readdir( retlist, cache_root_dir(), false );
  if ( res ) {
    ERR << "Error reading InstSrc cache at " << cache_root_dir() << " (errno " << res << ")" << endl;
  } else {
    for ( list<string>::iterator it = retlist.begin(); it != retlist.end(); ++it ) {
      DBG << "Check '" << *it << "'" << endl;
      if ( it->find( "IS_CACHE_" ) == 0 ) {
	if ( ! scanSrcCache( cache_root_dir() + *it ) ) {
	  // no error
	  ++count;
	}
      }
    }
  }

  // check rank values
  err = writeNewRanks();

  // autoenable, if desired
  if ( autoEnable_r ) {
    for ( ISrcPool::const_iterator it = _knownSources.begin(); it != _knownSources.end(); ++it ) {
      if ( (*it)->descr()->default_activate() ) {
	err = enableSource( *it );
	if ( !err ) {
	  MIL << "Auto enabled: " << *it << endl;
	} else {
	  ERR << "Failed auto enable: " << *it << endl;
	}
      } else {
	MIL << "Stays disabled: " << *it << endl;
      }
    }
  }

  if ( !count ) {
    WAR << "Got no InstSrc'es from cahce!" << endl;
  } else {
    MIL << "Read " << count << " InstSrc'es from cache." << endl;
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::scanSrcCache
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::scanSrcCache( const Pathname & srccache_r )
{
  MIL << "Read InstSrc from cache " << srccache_r << endl;

  InstSrcPtr nsrc;
  PMError err = InstSrc::vconstruct( nsrc, srccache_r );
  if ( err ) {
    WAR << "Invalid InstSrc cache at " << srccache_r << " (" << err << ")" << endl;
    return Error::E_isrc_cache_invalid;
  }

  ISrcId nid = poolAdd( nsrc, /*rankcheck*/false );
  if ( ! nid ) {
    WAR << "Duplicate InstSrc cache at " << srccache_r << endl;
    return Error::E_isrc_cache_duplicate;
  }
  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::writeNewRanks
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::writeNewRanks()
{
  unsigned rank = 0;
  for ( ISrcPool::const_iterator it = _knownSources.begin(); it != _knownSources.end(); ++it, ++rank ) {
    (*it)->descr()->set_default_rank( rank );
    (*it)->_mgr_attach();
  }
  DBG << *this;
  return Error::E_ok;
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
//	METHOD NAME : InstSrcManager::poolHandle
//	METHOD TYPE : InstSrcManager::ISrcPool::iterator
//
//	DESCRIPTION :
//
InstSrcManager::ISrcPool::iterator InstSrcManager::poolHandle( const ISrcId & isrc_r )
{
  if ( isrc_r ) {
    InstSrcPtr item = InstSrcPtr::cast_away_const( isrc_r );
    for ( ISrcPool::iterator it = _knownSources.begin(); it != _knownSources.end(); ++it ) {
      if ( *it == item )
	return it;
    }
  }
  return _knownSources.end();
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
  if ( isrc_r ) {
    InstSrcPtr item = InstSrcPtr::cast_away_const( isrc_r );
    for ( ISrcPool::const_iterator it = _knownSources.begin(); it != _knownSources.end(); ++it ) {
      if ( *it == item )
	return item;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::lookupInstSrc
//	METHOD TYPE : InstSrcPtr
//
//	DESCRIPTION :
//
InstSrcPtr InstSrcManager::lookupInstSrc( const InstSrcPtr & isrc_r ) const
{
  if ( ! ( isrc_r && isrc_r->descr() ) )
    return 0;

  for ( ISrcPool::const_iterator it = _knownSources.begin(); it != _knownSources.end(); ++it ) {
    if ( InstSrcDescr::sameContentProduct( (*it)->descr(), isrc_r->descr() ) ) {
      return *it;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::poolAdd
//	METHOD TYPE : ISrcId
//
//	DESCRIPTION :
//
InstSrcManager::ISrcId InstSrcManager::poolAdd( InstSrcPtr nsrc_r, bool rankcheck_r )
{
  if ( ! nsrc_r ) {
    INT << "Try to add NULL InstSrc" << endl;
    return 0;
  }

  if ( ! nsrc_r->descr() ) {
    INT << "Try to add InstSrc without InstSrcDescr" << endl;
    return 0;
  }

  InstSrcPtr gotsrc = lookupInstSrc( nsrc_r );
  if ( gotsrc ) {
    WAR << "Try to add duplicate InstSrc: " << nsrc_r << " (have " << gotsrc << ")" << endl;
    return 0;
  }

  ///////////////////////////////////////////////////////////////////
  // Insert sorted by default_rank
  //
  // On startup only rankchecks are disabled. Assignment of new ranks
  // and _mgr_attach() are handled afterwars.
  ///////////////////////////////////////////////////////////////////

  unsigned rank = nsrc_r->descr()->default_rank();

  if ( rank == NO_RANK ) {

    if ( rankcheck_r ) {
      // assign new rank (highest rank -> least priority)
      unsigned nrank = ( _knownSources.size() ? (*_knownSources.rbegin())->descr()->default_rank() + 1
					      : 0 );
      nsrc_r->descr()->set_default_rank( nrank );
      DBG << "Assigned rank " << nrank << " to " << nsrc_r << endl;
    }
    // insert at end
    _knownSources.push_back( nsrc_r );

  } else {

    ISrcPool::iterator it = _knownSources.begin();
    for ( ; it != _knownSources.end(); ++it ) {
      unsigned itrank = (*it)->descr()->default_rank();
      if ( rank == itrank && rankcheck_r ) {
	INT << "Duplicate InstSrc rank " << itrank << endl;
	continue;
      }
      if ( rank < itrank ) {
	_knownSources.insert( it, nsrc_r );
	break;
      }
    }
    if ( it == _knownSources.end() ) {
      _knownSources.insert( it, nsrc_r );
    }

  }

  if ( rankcheck_r ) {
    nsrc_r->_mgr_attach();
    DBG << *this;
  }

  MIL << "Added InstSrc " << nsrc_r << endl;
  return nsrc_r;
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

  MediaAccess::FileProvider pfile( media, "/media.1/products" );
  if ( pfile.error() ) {
    // no products file: default ProductEntry is /
    products.insert( ProductEntry() );
  } else {
    // scan products file
    MIL << "Found '/media.1/products'." << endl;
    scanProductsFile( pfile(), products );
  }

  media = 0; // release media

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
      ISrcId nid = poolAdd( nsrc );
      if ( nid ) {
	idlist_r.push_back( nid );
      } else {
        scan_err = Error::E_isrc_cache_duplicate;
      }
    }
  }

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  DBG << "scanMedia " << mediaurl_r << " found " << idlist_r.size() << " InstSrc(es)" << endl;
  return scan_err;
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

  WAR << "bad ISrcId " << isrc_r << endl;
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

  WAR << "bad ISrcId " << isrc_r << endl;
  return Error::E_bad_id;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::setAutoenable
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::setAutoenable( const ISrcId isrc_r, const bool yesno )
{
  InstSrcPtr it( lookupId( isrc_r ) );
  if ( it ) {
    if ( it->descr()->default_activate() != yesno ) {
      it->descr()->set_default_activate( yesno );
      return it->writeDescrCache();
    }
    return Error::E_ok;
  }

  WAR << "bad ISrcId " << isrc_r << endl;
  return Error::E_bad_id;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::rankUp
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Highest priority is beginning of list!
//
PMError InstSrcManager::rankUp( const ISrcId isrc_r )
{
  ISrcPool::iterator it( poolHandle( isrc_r ) );
  if ( it == _knownSources.end() ) {
    WAR << "bad ISrcId " << isrc_r << endl;
    return Error::E_bad_id;
  }

  ISrcPool::iterator prev( it );
  if ( --prev != _knownSources.end() ) {
    _knownSources.splice( prev, _knownSources, it );
  }

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::rankDown
//	METHOD TYPE : PMError
//
//	DESCRIPTION : Highest priority is beginning of list!
//
PMError InstSrcManager::rankDown( const ISrcId isrc_r )
{
  ISrcPool::iterator it( poolHandle( isrc_r ) );
  if ( it == _knownSources.end() ) {
    WAR << "bad ISrcId " << isrc_r << endl;
    return Error::E_bad_id;
  }

  ISrcPool::iterator next( it );
  if ( ++next != _knownSources.end() ) {
    _knownSources.splice( ++next, _knownSources, it );
  }
  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::rankBehind
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::rankBehind( const ISrcId isrc_r, const ISrcId point_r )
{
  ISrcPool::iterator it( poolHandle( isrc_r ) );
  if ( it == _knownSources.end() ) {
    WAR << "bad ISrcId " << isrc_r << endl;
    return Error::E_bad_id;
  }

  if ( !point_r ) {
    // to end of list
    _knownSources.splice( _knownSources.end(), _knownSources, it );
    return Error::E_ok;
  }

  ISrcPool::iterator next( poolHandle( point_r ) );
  if ( next == _knownSources.end() ) {
    WAR << "bad ISrcId " << point_r << endl;
    return Error::E_bad_id;
  }

  _knownSources.splice( ++next, _knownSources, it );

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::rankBefore
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::rankBefore( const ISrcId isrc_r, const ISrcId point_r )
{
  ISrcPool::iterator it( poolHandle( isrc_r ) );
  if ( it == _knownSources.end() ) {
    WAR << "bad ISrcId " << isrc_r << endl;
    return Error::E_bad_id;
  }

  if ( !point_r ) {
    // to beginning of list
    _knownSources.splice( _knownSources.begin(), _knownSources, it );
    return Error::E_ok;
  }

  ISrcPool::iterator prev( poolHandle( point_r ) );
  if ( prev == _knownSources.end() ) {
    WAR << "bad ISrcId " << point_r << endl;
    return Error::E_bad_id;
  }

  _knownSources.splice( prev, _knownSources, it );

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::setNewRanks
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::setNewRanks()
{
  return writeNewRanks();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::deleteSource
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::deleteSource( const ISrcId & isrc_r )
{
  return Error::E_TBD;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::rewriteUrl
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::rewriteUrl( const ISrcId isrc_r, const Url & newUrl_r )
{
  if ( !newUrl_r.isValid() ) {
    WAR << "Won't write invalid url: " << newUrl_r << endl;
    return Error::E_bad_url;
  }

  InstSrcPtr it( lookupId( isrc_r ) );
  if ( it ) {
    it->descr()->set_url( newUrl_r );
    return it->writeDescrCache();
  }

  WAR << "bad ISrcId " << isrc_r << endl;
  return Error::E_bad_id;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::getSources
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void InstSrcManager::getSources( ISrcIdList & idlist_r, const bool enabled_only ) const
{
  idlist_r.clear();
  for ( ISrcPool::const_iterator it = _knownSources.begin(); it != _knownSources.end(); ++it ) {
    if ( !enabled_only || (*it)->enabled() ) {
      idlist_r.push_back( *it );
    }
  }
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
  str << "===[known sources]===================" << endl;
  for ( InstSrcManager::ISrcPool::const_iterator it = obj._knownSources.begin();
	it != obj._knownSources.end(); ++it ) {
    str << (*it)->descr()->default_activate() << " [" << (*it)->descr()->default_rank() << "] "
	<< (*it) << endl;
  }
  str << "=====================================" << endl;
  return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::cacheCopyTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::cacheCopyTo( const Pathname & newRoot_r )
{
  if ( newRoot_r.empty() || newRoot_r == "/" ) {
    ERR << "Invalid newRoot '" << newRoot_r << "'" << endl;
    return Error::E_error;
  }

  PathInfo npath( newRoot_r );
  if ( !npath.isDir() ) {
    ERR << "newRoot is not directory: " << npath << "'" << endl;
    return Error::E_error;
  }

  Pathname old_cache_root_dir = _cache_root_dir;
  _cache_root_dir = newRoot_r + _cache_root_dir;

  PMError err = intern_cacheCopyTo( newRoot_r );

  _cache_root_dir = old_cache_root_dir;

  if ( err ) {
    ERR << "cacheCopyTo '" << newRoot_r << "' failed " << err << endl;
  } else {
    MIL << "cacheCopyTo '" << newRoot_r << "' done" << endl;
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcManager::intern_cacheCopyTo
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcManager::intern_cacheCopyTo( const Pathname & newRoot_r )
{
  int res = PathInfo::assert_dir( cache_tmp_dir(), 0700 );
  if ( res ) {
    ERR << "Unable to create cache " << cache_tmp_dir() << " (errno " << res << ")" << endl;
    return Error::E_error;
  }

  // iterate all enabled sources and copy caches

  for ( ISrcPool::const_iterator it = _knownSources.begin(); it != _knownSources.end(); ++it ) {
    if ( ! (*it)->enabled() )
      continue;

    Pathname  srccache( genSrcCacheName() );
    DBG << "Use " << srccache << " for " << (*it) << endl;

    ///////////////////////////////////////////////////////////////////
    // first prepare cache
    // cachedir must not exist, but parent dir must.
    ///////////////////////////////////////////////////////////////////
    PathInfo cpath( srccache );

    if ( cpath.isExist() ) {
      ERR << "Cache dir already exists " << cpath << endl;
      return Error::E_cache_dir_exists;
    }

    int res = PathInfo::mkdir( cpath.path(), 0700 );
    if ( res ) {
      ERR << "Unable to create cache dir " << cpath << " (errno " << res << ")" << endl;
      return Error::E_cache_dir_create;
    }

    ///////////////////////////////////////////////////////////////////
    // create media_dir. descr/data dir
    ///////////////////////////////////////////////////////////////////
    Pathname ndesc( srccache + InstSrc::_c_descr_dir );
    Pathname ndata( srccache + InstSrc::_c_data_dir );
    Pathname nmedi( srccache + InstSrc::_c_media_dir );

    res = PathInfo::assert_dir( ndesc, 0700 );
    if ( res ) {
      ERR << "Unable to create descr_dir " << ndesc << " (errno " << res << ")" << endl;
      return Error::E_cache_dir_create;
    }

    res = PathInfo::assert_dir( ndata, 0700 );
    if ( res ) {
      ERR << "Unable to create data_dir " << ndata << " (errno " << res << ")" << endl;
      return Error::E_cache_dir_create;
    }

    res = PathInfo::assert_dir( nmedi, 0700 );
    if ( res ) {
      ERR << "Unable to create media_dir " << nmedi << " (errno " << res << ")" << endl;
      return Error::E_cache_dir_create;
    }

    ///////////////////////////////////////////////////////////////////
    // write cache to ...
    ///////////////////////////////////////////////////////////////////

    if ( (*it)->descr() ) {
      (*it)->descr()->writeCache( ndesc );
    }
    if ( (*it)->data() ) {
      (*it)->data()->writeCache( ndata );
    }
  }

  return Error::E_ok;
}


