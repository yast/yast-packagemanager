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

#include "y2pm/InstSrcDataYUM.h"
#include "YUMImpl.h"

#include <y2pm/InstSrcDescr.h>
#include <y2pm/InstSrcError.h>
#include <y2pm/MediaAccess.h>
#include <y2pm/Timecount.h>

#include <Y2PM.h>

using namespace std;
using namespace YUM;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcDataYUMPtr
//	CLASS NAME : constInstSrcDataYUMPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(InstSrcDataYUM,InstSrcData);

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : InstSrcDataYUM::InstSrcDataYUM
//	METHOD TYPE : Constructor
//
InstSrcDataYUM::InstSrcDataYUM( const Pathname & repodataDir_r )
{
  _impl = makeVarPtr( new YUM::Impl( *this, repodataDir_r ) );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : InstSrcDataYUM::~InstSrcDataYUM
//	METHOD TYPE : Destructor
//
InstSrcDataYUM::~InstSrcDataYUM()
{
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : InstSrcDataYUM::dumpOn
//	METHOD TYPE : std::ostream &
//
std::ostream &
InstSrcDataYUM::dumpOn( std::ostream & str ) const
{
  return Rep::dumpOn( str );
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : InstSrcDataYUM::getPackages
//	METHOD TYPE : const std::list<PMPackagePtr> &
//
const std::list<PMPackagePtr> &
InstSrcDataYUM::getPackages() const
{
  return _impl->getPackages();
}

//////////////////////////////////////////////////////////////////
//
//	METHOD NAME : InstSrcDataYUM::tryGetDescr
//	METHOD TYPE : PMError
//
PMError
InstSrcDataYUM::tryGetDescr( InstSrcDescrPtr & ndescr_r,
                             const InstSrcPtr  source_r,
                             MediaAccessPtr    media_r,
                             const Pathname &  product_dir_r,
                             const Url &       mediaurl_r )
{
  ndescr_r = 0;
  PMError err;

  ///////////////////////////////////////////////////////////////////
  // Prepare Repodata either from cache or media.
  ///////////////////////////////////////////////////////////////////
  Pathname repodataDir( source_r->cache_data_dir() );

  if ( Y2PM::runningFromSystem() || Y2PM::cacheToRamdisk() )
    {
      PathInfo cpath( repodataDir );
      if ( !cpath.isDir() ) {
        WAR << "Cache disabled: cachedir does not exist: " << cpath << endl;
        return Error::E_src_cache_disabled;
      }
    }
  else
    {
      ERR << Error::E_src_cache_disabled << endl;
      return Error::E_src_cache_disabled;
    }

  ///////////////////////////////////////////////////////////////////
  // Fill cache
  ///////////////////////////////////////////////////////////////////
  Repodata repodata( repodataDir );

  err = repodata.update( media_r, product_dir_r + Repodata::defaultRepodataDir() );
  if ( err )
    {
      ERR << "Failed to create cache! " << err << endl;
      return err;
    }

  ///////////////////////////////////////////////////////////////////
  // looks good? So create descr.
  ///////////////////////////////////////////////////////////////////

  InstSrcDescrPtr ndescr( new InstSrcDescr );
  ndescr->set_media_id( repodata.getId() );
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
//	METHOD NAME : InstSrcDataYUM::tryGetData
//	METHOD TYPE : PMError
//
PMError
InstSrcDataYUM::tryGetData( InstSrcDataPtr & ndata_r, const InstSrcPtr source_r )
{
  ndata_r = 0;
  PMError err;

  ///////////////////////////////////////////////////////////////////
  // Check local cache
  ///////////////////////////////////////////////////////////////////
  Pathname repodataDir( source_r->cache_data_dir() );

  if ( Y2PM::runningFromSystem() || Y2PM::cacheToRamdisk() )
    {
      PathInfo cpath( repodataDir );
      if ( !cpath.isDir() ) {
        WAR << "Cache disabled: cachedir does not exist: " << cpath << endl;
        return Error::E_src_cache_disabled;
      }
    }
  else
    {
      ERR << Error::E_src_cache_disabled << endl;
      return Error::E_src_cache_disabled;
    }

  ///////////////////////////////////////////////////////////////////
  // parse InstSrcData from cache and fill ndata
  ///////////////////////////////////////////////////////////////////
  InstSrcDataYUMPtr ndata( new InstSrcDataYUM( repodataDir ) );

  if ( ! ndata->_impl->repodata().hasData() )
    {
      ERR << Error::E_isrc_cache_invalid << endl;
      return Error::E_isrc_cache_invalid;
    }

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  if ( ! err ) {
    ndata_r = ndata;
  }
  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataYUM::tryGetMediaId
//	METHOD TYPE : PMError
//
PMError InstSrcDataYUM::tryGetMediaId( const Url & url_r, const Pathname & product_dir_r,
                                       std::string & mediaId_r )
{
  // reset
  mediaId_r = string();

  // access media at url
  MediaAccessPtr media( new MediaAccess );
  PMError err;

  if ( (err = media->open( url_r )) )
    {
      ERR << "Failed to open " << url_r << " " << err << endl;
      return err;
    }

  if ( (err = media->attach()) )
    {
      ERR << "Failed to attach media: " << err << endl;
      return err;
    }

  // retrieve repomd.xml
  Pathname filename( product_dir_r
                     + Repodata::defaultRepodataDir() + Repodata::defaultRepomd_Xml() );
  MediaAccess::FileProvider mediafile( media, filename );
  if ( (err = mediafile.error()) )
    {
      ERR << "Media can't provide '" << filename << "' " << err << endl;
      return err;
    }

  // parse repomd.xml
  Repodata repodata( mediafile().dirname() );
  if ( ! repodata.hasData() )
    return InstSrcError::E_no_instsrc_on_media;

  // return ID
  mediaId_r = repodata.getId();
  return Error::E_ok;
}
