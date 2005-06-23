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

  File:       InstSrcDataYUM.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: YUM installation source

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>
#include <y2util/stringutil.h>

#include <y2pm/InstSrcDataYUM.h>
#include <y2pm/PMYUMPackageDataProvider.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/InstSrcError.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/Timecount.h>

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcDataYUMPtr
//	CLASS NAME : constInstSrcDataYUMPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(InstSrcDataYUM,InstSrcData);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataYUM::InstSrcDataYUM
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstSrcDataYUM::InstSrcDataYUM( const Pathname & repodataDir_r )
{
  DBG << "Found " << _packages.size() << " packages" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataYUM::~InstSrcDataYUM
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcDataYUM::~InstSrcDataYUM()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataYUM::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrcDataYUM::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataYUM::providePkgToInstall
//	METHOD TYPE : PMError
//
PMError InstSrcDataYUM::providePkgToInstall( const Pathname & pkgfile_r, Pathname & path_r ) const
{
#if 0
  path_r = Pathname();

  if ( pkgfile_r.empty() ) {
    ERR << "Empty path to provide!" << endl;
    return Error::E_no_source;
  }

  if ( !attached() ) {
    ERR << "Not attached to an instSrc!" << endl;
    return Error::E_src_not_enabled;
  }

  MediaAccessPtr media( _instSrc->media() );

  if ( ! media ) {
    ERR << "No instSrc media" << endl;
    return Error::E_no_media;
  }

  if ( ! media->isOpen() ) {
    PMError ret = media->open( _instSrc->descr()->url(),
			       _instSrc->cache_media_dir() );
    if ( ret ) {
      ERR << "Failed to open media " << _instSrc->descr()->url() << ": " << ret << endl;
      return Error::E_no_media;
    }
  }

  if ( ! media->isAttached() ) {
    PMError ret = media->attach();
    if ( ret ) {
      ERR << "Failed to attach media: " << ret << endl;
      return Error::E_no_media;
    }
  }

  Pathname pkgfile( _instSrc->descr()->datadir() + pkgfile_r );
  PMError err = media->provideFile( pkgfile );
  if ( err ) {
    ERR << "Media can't provide '" << pkgfile << "' (" << err << ")" << endl;
    return err;
  }

  path_r = media->localPath( pkgfile );

  if ( _instSrc->isRemote() && ! RpmHeader::readPackage( path_r, RpmHeader::NOSIGNATURE ) ) {
    err = Error::E_corrupted_file;
    err.setDetails( pkgfile.asString() );
    PathInfo::unlink( path_r );
    ERR << "Bad digest '" << path_r << "': " << err << endl;
    path_r = Pathname();
    return err;
  }

  _instSrc->rememberPreviouslyDnlPackage( path_r );

  return PMError::E_ok;
#endif
  ERR << "NOP" << endl;
  return PMError::E_error;
}

//////////////////////////////////////////////////////////////////
//
//	METHOD NAME : InstSrcDataYUM::tryGetDescr
//	METHOD TYPE : PMError
//
//	Try to read content data (describing the product)
//	and fill InstSrcDescrPtr class.
//
PMError InstSrcDataYUM::tryGetDescr( InstSrcDescrPtr & ndescr_r,
				       const InstSrcPtr source_r,
				       MediaAccessPtr media_r,
				       const Pathname & product_dir_r,
				       const Url & mediaurl_r )
{
#if 0
  ndescr_r = 0;
  PMError err;

  PathInfo cpath;

  // FIXME: indentation
  if ( Y2PM::runningFromSystem() || Y2PM::cacheToRamdisk() ) {
  ///////////////////////////////////////////////////////////////////
  // Check local cache
  ///////////////////////////////////////////////////////////////////

  Pathname cdir( source_r->cache_data_dir() );

  cpath( cdir );
  if ( !cpath.isDir() ) {
    WAR << "Cache disabled: cachedir does not exist: " << cpath << endl;
    return Error::E_src_cache_disabled;
  }

  cpath( cdir + "IS_YUMcache.gz" ); // cachefile in local cache
  if ( !cpath.isFile() ) {
    cpath( cdir + "IS_YUMcache" ); // cachefile in local cache
  }

  ///////////////////////////////////////////////////////////////////
  // If no local cache, get IS_YUMcache from media_r.
  // NOTE: descrdir and datadir equal product_dir_r
  ///////////////////////////////////////////////////////////////////

  MediaAccessPtr media = source_r->media();

  if ( !cpath.isFile() ) {
    Pathname m_file( product_dir_r + "IS_YUMcache.gz" );
    MediaAccess::FileProvider cachefile( media, m_file );
    if ( cachefile.error() ) {
      WAR << "Media can't provide '" << m_file << "' " << cachefile.error() << endl;
    } else {
      MIL << "Found cache '" << m_file << "'" << endl;
      PathInfo::copy_file2dir( cachefile(), cdir );

      cpath( cdir + "IS_YUMcache.gz" ); // restat
    }
  }

  if ( !cpath.isFile() ) {
    Pathname m_file( product_dir_r + "IS_YUMcache" );
    MediaAccess::FileProvider cachefile( media, m_file );
    if ( cachefile.error() ) {
      WAR << "Media can't provide '" << m_file << "' " << cachefile.error() << endl;
    } else {
      MIL << "Found cache '" << m_file << "'" << endl;
      PathInfo::copy_file2dir( cachefile(), cdir );

      cpath( cdir + "IS_YUMcache" ); // restat
    }
  }

  if ( !cpath.isFile() ) {

    switch ( media->protocol() ) {
    case Url::ftp:
    case Url::http:
    case Url::https:
      ERR << "FTP/HTTP package scan supported. Create IS_YUMcache!" << endl;
      return MediaError::E_not_supported_by_media;
    default:
      break;
    }

    Pathname pkgroot( media->localPath( product_dir_r ) );
    MIL << "Start package scan in " << pkgroot << endl;

    Pathname c_file( cdir + "IS_YUMcache" );

    int res = RpmHeaderCache::buildHeaderCache( c_file, pkgroot );
    if ( res < 0 ) {
      ERR << "Failed to create cache " << c_file << " (" << res << ")" << endl;
      PathInfo::unlink( c_file );
      return Error::E_isrc_cache_invalid;
    }

#warning GZIP cache per default?
    cpath( c_file ); // restat
    MIL << "Created cache for " << res << " packages found." << endl;
  }

  }
  else
  {
    PMError err = media_r->provideFile( "IS_YUMcache" );
    if ( err ) {
	ERR << "Media can't provide 'IS_YUMcache' " << err << endl;
	return err;
    }
    cpath = media_r->localPath("IS_YUMcache");
  }

  ///////////////////////////////////////////////////////////////////
  // looks good? So create descr.
  ///////////////////////////////////////////////////////////////////

  RpmHeaderCache cache( cpath.path() );

  if ( ! cache.open() ) {
    ERR << "Invalid cache " << cpath << endl;
    PathInfo::unlink( cpath.path() );
    return Error::E_isrc_cache_invalid;
  }

  InstSrcDescrPtr ndescr( new InstSrcDescr );
#warning which descr data to provide for InstSrcDataYUM?

  ndescr->set_media_id( stringutil::numstring( cache.cdate() ) );
  ndescr->set_media_count( 1 );
  ndescr->set_content_label( mediaurl_r.asString( /*path*/true, /*options*/false ) + product_dir_r.asString() );
  ndescr->set_content_descrdir( "/" );
  ndescr->set_content_datadir( "/" );

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  if ( ! err ) {
    ndescr_r = ndescr;
  }

  return err;
#endif
  ERR << "NOP" << endl;
  return PMError::E_error;
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : InstSrcDataYUM::tryGetData
//	METHOD TYPE : PMError
//
PMError InstSrcDataYUM::tryGetData( InstSrcDataPtr & ndata_r, const InstSrcPtr source_r )
{
#if 0
  ndata_r = 0;
  PMError err;
  PathInfo cpath;

  ///////////////////////////////////////////////////////////////////
  // Check local cache
  ///////////////////////////////////////////////////////////////////

  Pathname cdir( source_r->cache_data_dir() );

  if ( Y2PM::runningFromSystem() || Y2PM::cacheToRamdisk() ) {

    cpath( cdir );
    if ( !cpath.isDir() ) {
      WAR << "Cache disabled: cachedir does not exist: " << cpath << endl;
      return Error::E_src_cache_disabled;
    }

    cpath( cdir + "IS_YUMcache.gz" ); // cachefile in local cache
    if ( !cpath.isFile() ) {
      WAR << "No cachefile " << cpath << endl;

      cpath( cdir + "IS_YUMcache" ); // cachefile in local cache
      if ( !cpath.isFile() ) {
	WAR << "No cachefile " << cpath << endl;
      }
    }

  } else {
    cpath = source_r->media()->localPath("IS_YUMcache");
  }

  if ( !cpath.isFile() ) {
    ERR << "No cachefile found in " << cdir << endl;
    return Error::E_isrc_cache_invalid;
  } else {
    MIL << "Using cachefile " << cpath << endl;
  }

  ///////////////////////////////////////////////////////////////////
  // parse InstSrcData from cache and fill ndata
  ///////////////////////////////////////////////////////////////////

  InstSrcDataYUMPtr ndata( new InstSrcDataYUM( cpath.path() ) );

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  if ( ! err ) {
    ndata_r = ndata;
  }

  return err;
#endif
  ERR << "NOP" << endl;
  return PMError::E_error;
}
