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

  File:       InstSrcDataPLAIN.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>
#include <y2util/stringutil.h>

#include <y2pm/InstSrcDataPLAIN.h>
#include <y2pm/PMPLAINPackageDataProvider.h>
#include <y2pm/InstSrcDescr.h>
#include <y2pm/InstSrcError.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/RpmHeaderCache.h>
#include <y2pm/RpmHeader.h>
#include <y2pm/Timecount.h>

#include <Y2PM.h>

using namespace std;

/******************************************************************
**
**
**	FUNCTION NAME : archCompat
**	FUNCTION TYPE : set<PkgArch>
*/
set<PkgArch> archCompat( const PkgArch & arch )
{
  static map<PkgArch,string> compatmap;

  if ( compatmap.empty() ) {
#define OUTMAP(a,c) compatmap[PkgArch(a)] = c
    OUTMAP( "i386",	"noarch" );
    OUTMAP( "i486",	"i386 noarch" );
    OUTMAP( "i586",	"i486 i386 noarch" );
    OUTMAP( "i686",	"i586 i486 i386 noarch" );
    OUTMAP( "athlon",	"i686 i586 i486 i386 noarch" );
    OUTMAP( "x86_64",	"athlon i686 i586 i486 i386 noarch" );
#undef OUTMAP
  }

  set<PkgArch> ret;
  ret.insert( arch );
  ret.insert( PkgArch("noarch") );

  map<PkgArch,string>::const_iterator it = compatmap.find( arch );
  if ( it != compatmap.end() ) {
    MIL << "archCompat: " << arch << ": ";
    vector<string> archvec;
    stringutil::split( it->second, archvec );
    for ( unsigned i = 0; i < archvec.size(); ++i ) {
      ret.insert( PkgArch(archvec[i]) );
      MIL << archvec[i] << " ";
    }
    MIL << endl;
  }

  return ret;
}

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcDataPLAINPtr
//	CLASS NAME : constInstSrcDataPLAINPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(InstSrcDataPLAIN,InstSrcData,InstSrcData);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataPLAIN::InstSrcDataPLAIN
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstSrcDataPLAIN::InstSrcDataPLAIN( const Pathname & cachefile_r )
    : _cache( * new RpmHeaderCache( cachefile_r ) )
{
  if ( !_cache.open() ) {
    ERR << "Failed to open cache " << cachefile_r << endl;
    return;
  }
  MIL << "Scan cachefile " << cachefile_r << endl;

  set<PkgArch> compatArch( archCompat( Y2PM::baseArch() ) );

  unsigned hpos;
  Pathname pkgfile;
  int      isSource;
  for ( constRpmHeaderPtr iter = _cache.getFirst( pkgfile, isSource, hpos );
	iter; iter = _cache.getNext( pkgfile, isSource, hpos ) ) {
    D__ << "At " << hpos << (isSource?" src ":" bin ") << iter << " for " << pkgfile << endl;

    if ( isSource ) {
      INT << "No yet able to handle .src.rpm." << endl;
      continue;
    }

    PkgArch arch( iter->tag_arch() );

    if ( compatArch.find( arch ) == compatArch.end() ) {
      WAR << "Drop incompatible " << arch << " (" << Y2PM::baseArch() << ")" << endl;
      continue;
    }

    // create dataprovider and package
    PMPLAINPackageDataProviderPtr ndp = new PMPLAINPackageDataProvider( this, hpos, pkgfile );
    PMPackagePtr nptr = new PMPackage( iter->tag_name(), iter->tag_edition(), arch, ndp );

    // add PMSolvable data to package

    //nptr->setProvides ( iter->tag_provides () );
#warning Filtering splitprovides is not job of InstSrc! Solvable itself should handle this.
    PMSolvable::PkgRelList_type oprovides( iter->tag_provides() );
    for ( PMSolvable::PkgRelList_type::iterator it = oprovides.begin(); it != oprovides.end(); /*advance inside*/ ) {
      if ( it->op() == NONE ) {
	PkgSplit testsplit( it->name(), /*quiet*/true );
	if ( testsplit.valid() ) {
	  ndp->_attr_SPLITPROVIDES.insert( testsplit );
	  it = oprovides.erase( it );
	  continue;
	}
      }
      ++it;
    }
    nptr->setProvides ( oprovides );

    nptr->setRequires ( iter->tag_requires() );
    nptr->setConflicts( iter->tag_conflicts() );
    nptr->setObsoletes( iter->tag_obsoletes() );

    // let dataprovider collect static data
    ndp->loadStaticData( iter );

    _packages.push_back( nptr );
  }

  DBG << "Found " << _packages.size() << " packages" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataPLAIN::~InstSrcDataPLAIN
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcDataPLAIN::~InstSrcDataPLAIN()
{
  delete &_cache;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataPLAIN::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrcDataPLAIN::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataPLAIN::getHeaderAt
//	METHOD TYPE : constRpmHeaderPtr
//
constRpmHeaderPtr InstSrcDataPLAIN::getHeaderAt( unsigned _cachepos ) const
{
  return _cache.getAt( _cachepos );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataPLAIN::providePkgToInstall
//	METHOD TYPE : PMError
//
PMError InstSrcDataPLAIN::providePkgToInstall( const Pathname & pkgfile_r, Pathname & path_r ) const
{
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
  _instSrc->rememberPreviouslyDnlPackage( path_r );

  return PMError::E_ok;
}

//////////////////////////////////////////////////////////////////
//
//	METHOD NAME : InstSrcDataPLAIN::tryGetDescr
//	METHOD TYPE : PMError
//
//	Try to read content data (describing the product)
//	and fill InstSrcDescrPtr class.
//
PMError InstSrcDataPLAIN::tryGetDescr( InstSrcDescrPtr & ndescr_r,
				       const InstSrcPtr source_r,
				       MediaAccessPtr media_r,
				       const Pathname & product_dir_r,
				       const Url & mediaurl_r )
{
  ndescr_r = 0;
  PMError err;

  ///////////////////////////////////////////////////////////////////
  // Check local cache
  ///////////////////////////////////////////////////////////////////

  Pathname cdir( source_r->cache_data_dir() );

  PathInfo cpath( cdir );
  if ( !cpath.isDir() ) {
    WAR << "Cache disabled: cachedir does not exist: " << cpath << endl;
    return Error::E_src_cache_disabled;
  }

  cpath( cdir + "IS_PLAINcache.gz" ); // cachefile in local cache
  if ( !cpath.isFile() ) {
    cpath( cdir + "IS_PLAINcache" ); // cachefile in local cache
  }

  ///////////////////////////////////////////////////////////////////
  // If no local cache, get IS_PLAINcache from media_r.
  // NOTE: descrdir and datadir equal product_dir_r
  ///////////////////////////////////////////////////////////////////

  MediaAccessPtr media = source_r->media();

  if ( !cpath.isFile() ) {
    Pathname m_file( product_dir_r + "IS_PLAINcache.gz" );
    MediaAccess::FileProvider cachefile( media, m_file );
    if ( cachefile.error() ) {
      WAR << "Media can't provide '" << m_file << "' " << cachefile.error() << endl;
    } else {
      MIL << "Found cache '" << m_file << "'" << endl;
      PathInfo::copy_file2dir( cachefile(), cdir );

      cpath( cdir + "IS_PLAINcache.gz" ); // restat
    }
  }

  if ( !cpath.isFile() ) {
    Pathname m_file( product_dir_r + "IS_PLAINcache" );
    MediaAccess::FileProvider cachefile( media, m_file );
    if ( cachefile.error() ) {
      WAR << "Media can't provide '" << m_file << "' " << cachefile.error() << endl;
    } else {
      MIL << "Found cache '" << m_file << "'" << endl;
      PathInfo::copy_file2dir( cachefile(), cdir );

      cpath( cdir + "IS_PLAINcache" ); // restat
    }
  }

  if ( !cpath.isFile() ) {

    switch ( media->protocol() ) {
    case Url::ftp:
    case Url::http:
    case Url::https:
      ERR << "FTP/HTTP package scan supported. Create IS_PLAINcache!" << endl;
      return MediaError::E_not_supported_by_media;
    default:
      break;
    }

    Pathname pkgroot( media->localPath( product_dir_r ) );
    MIL << "Start package scan in " << pkgroot << endl;

    Pathname c_file( cdir + "IS_PLAINcache" );

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
#warning which descr data to provide for InstSrcDataPLAIN?

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
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : InstSrcDataPLAIN::tryGetData
//	METHOD TYPE : PMError
//
PMError InstSrcDataPLAIN::tryGetData( InstSrcDataPtr & ndata_r, const InstSrcPtr source_r )
{
  ndata_r = 0;
  PMError err;

  ///////////////////////////////////////////////////////////////////
  // Check local cache
  ///////////////////////////////////////////////////////////////////

  Pathname cdir( source_r->cache_data_dir() );

  PathInfo cpath( cdir );
  if ( !cpath.isDir() ) {
    WAR << "Cache disabled: cachedir does not exist: " << cpath << endl;
    return Error::E_src_cache_disabled;
  }

  cpath( cdir + "IS_PLAINcache.gz" ); // cachefile in local cache
  if ( !cpath.isFile() ) {
    WAR << "No cachefile " << cpath << endl;

    cpath( cdir + "IS_PLAINcache" ); // cachefile in local cache
    if ( !cpath.isFile() ) {
      WAR << "No cachefile " << cpath << endl;
    }
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

  InstSrcDataPLAINPtr ndata( new InstSrcDataPLAIN( cpath.path() ) );

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  if ( ! err ) {
    ndata_r = ndata;
  }

  return err;
}
