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

  File:       InstSrc.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:	Class for installation sources
		Defines (provides access to)
		- media access (a pointer to MediaAccess)
		- source description (product, version, vendor, ...)
		- contents (list of package, list of selections, ...)

/-*/

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>

#include <Y2PM.h>

#include <y2pm/InstSrc.h>

#include <y2pm/MediaAccess.h>
#include <y2pm/InstSrcDescr.h>

#include <y2pm/InstSrcData.h>
#include <y2pm/InstSrcDataUL.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrc
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(InstSrc);

///////////////////////////////////////////////////////////////////

const Pathname InstSrc::_c_descr_dir( "DESCRIPTION" );
const Pathname InstSrc::_c_data_dir ( "DATA" );
const Pathname InstSrc::_c_media_dir( "MEDIA" );

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::InstSrc
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
InstSrc::InstSrc()
    : _cache_deleteOnExit( false )
{
  MIL << "New InstSrc " << *this << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::~InstSrc
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrc::~InstSrc()
{
#warning ****************************************
#warning ** FORCE _cache_deleteOnExit until InstSrcMgr ready
  //INT << "FORCE _cache_deleteOnExit until InstSrcMgr ready" << endl;
  //_cache_deleteOnExit = true;
#warning ****************************************

  MIL << "Delete InstSrc " << *this << "(" << (_cache_deleteOnExit ? "delete " : "keep " ) << _cache << ")" << endl;

  if ( _media )
    _media->close();

  if ( _cache_deleteOnExit ) {
    PathInfo::recursive_rmdir( _cache );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::_mgr_attach
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void InstSrc::_mgr_attach()
{
  _cache_deleteOnExit = false;
  writeCache();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::enableSource
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrc::enableSource()
{
  MIL << "Enable InstSrc " << *this << endl;

  ///////////////////////////////////////////////////////////////////
  // pre checks
  ///////////////////////////////////////////////////////////////////

  if ( _data ) {
    ERR << "Already enabled." << endl;
    return Error::E_src_already_enabled;
  }

  if ( !_descr ) {
    ERR << "Cannot enable without source description" << endl;
    return Error::E_src_no_description;
  }

  ///////////////////////////////////////////////////////////////////
  // create InstSrcData according to Type stored in InstSrcDescr
  ///////////////////////////////////////////////////////////////////
  PMError err;
  InstSrcDataPtr ndata;

  switch ( _descr->type() ) {

    case T_UnitedLinux:
	err = InstSrcDataUL::tryGetData( ndata, _media, _descr->descrdir() );
	break;

    case T_TEST_DIST:
	err = InstSrcData::tryGetData( ndata, _media, _descr->descrdir() );
	break;

    ///////////////////////////////////////////////////////////////////
    // no default: let compiler warn '... not handled in switch'
    ///////////////////////////////////////////////////////////////////
    case T_UNKNOWN:
    case T_AUTODETECT:
      break;
  }

  if ( err && ndata ) {
    INT << "Note: reading InstSrcData returned data and error" << endl;
    ndata = 0;
  }

  if ( !ndata ) {
    ERR << "No InstSrcData type " << _descr->type() << " found on media " << _descr->url() << endl;
    return Error::E_no_instsrcdata_on_media;
  }

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  if ( !err ) {
    _data = ndata;
    _data->_instSrc_attach( this ); // adjust backreferences to InstSrc.
    _data->_instSrc_propagate();    // propagate Objects to Manager classes.

    writeCache();
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::disableSource
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrc::disableSource()
{
  MIL << "Disable InstSrc " << *this << endl;

  ///////////////////////////////////////////////////////////////////
  // pre checks
  ///////////////////////////////////////////////////////////////////

  if ( !_data ) {
    ERR << "Not enabled." << endl;
    return Error::E_src_not_enabled;
  }

  ///////////////////////////////////////////////////////////////////
  // Remove back references to InstSrc in _data. Otherwise we
  // wont get zero refcounts.
  ///////////////////////////////////////////////////////////////////
  PMError err;

  writeCache();

  _data->_instSrc_withdraw(); // withdraw Objects from Manager classes.
  _data->_instSrc_detach();   // clear backreferences to InstSrc.
  _data = 0;

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::writeCache
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrc::writeCache()
{
#warning errorchecks
  if ( ! Y2PM::runningFromSystem() ) {
    MIL << "Not running from system: writeCache disabled" << endl;
    return Error::E_src_cache_disabled;
  }

  if ( _descr ) {
    _descr->writeCache( cache_descr_dir() );
  }

  if ( _data ) {
    _data->writeCache( cache_data_dir() );
  }

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::_init_openCache
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrc::_init_openCache( const Pathname & cachedir_r )
{
  MIL << "InstSrc open cache " << cachedir_r << endl;

  ///////////////////////////////////////////////////////////////////
  // check cachedir_r
  ///////////////////////////////////////////////////////////////////
  PathInfo cpath( cachedir_r );

  if ( ! cpath.isDir() ) {
    ERR << "Bad cache dir " << cpath << endl;
    return Error::E_bad_cache_dir;
  }

  _cache = cachedir_r;

  ///////////////////////////////////////////////////////////////////
  // check cache
  // cache_descr_dir must exist, cache_data_dir/cache_media_dir
  // are created if missing.
  ///////////////////////////////////////////////////////////////////
  cpath( cache_descr_dir() );

  if ( ! cpath.isDir() ) {
    ERR << "No cache description " << cpath << endl;
    return Error::E_bad_cache_descr;
  }

  cpath( cache_data_dir() );
  if ( cpath.isExist() ) {
    if ( !cpath.isDir() ) {
      ERR << "data_dir is not a directory " << cpath << endl;
      return Error::E_cache_dir_create;
    }
  } else {
    int res = PathInfo::assert_dir( cpath.path(), 0700 );
    if ( res ) {
      ERR << "Unable to create data_dir " << cpath << " (errno " << res << ")" << endl;
      return Error::E_cache_dir_create;
    }
  }

  cpath( cache_media_dir() );
  if ( cpath.isExist() ) {
    if ( !cpath.isDir() ) {
      ERR << "media_dir is not a directory " << cpath << endl;
      return Error::E_cache_dir_create;
    }
  } else {
    int res = PathInfo::assert_dir( cpath.path(), 0700 );
    if ( res ) {
      ERR << "Unable to create media_dir " << cpath << " (errno " << res << ")" << endl;
      return Error::E_cache_dir_create;
    }
  }

  ///////////////////////////////////////////////////////////////////
  // parse cache_descr_file
  ///////////////////////////////////////////////////////////////////
  PMError err;
  InstSrcDescrPtr ndescr;

  if ( (err = InstSrcDescr::readCache( ndescr, cache_descr_dir())) ) {
    ERR << "Error reading cache description " << cpath << " " << err << endl;
    return err;
  }

  if ( !ndescr ) {
    INT << "Reading cache description returned NULL" << endl;
    return Error::E_bad_cache_descr;
  }

  MIL << "Found InstSrc " << ndescr << endl;

  ///////////////////////////////////////////////////////////////////
  // setup MediaAccess and InstSrcDescr
  ///////////////////////////////////////////////////////////////////

  _descr = ndescr;
  _media = new MediaAccess;

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::_init_newCache
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrc::_init_newCache( const Pathname & cachedir_r )
{
  MIL << "InstSrc create new cache " << cachedir_r << endl;

  ///////////////////////////////////////////////////////////////////
  // first prepare cache
  // cachedir must not exist, but parent dir must.
  ///////////////////////////////////////////////////////////////////
  PathInfo cpath( cachedir_r );

  if ( cpath.isExist() ) {
    ERR << "Cache dir already exists " << cpath << endl;
    return Error::E_cache_dir_exists;
  }

  int res = PathInfo::mkdir( cpath.path(), 0700 );
  if ( res ) {
    ERR << "Unable to create cache dir " << cpath << " (errno " << res << ")" << endl;
    return Error::E_cache_dir_create;
  }

  _cache = cachedir_r;
  _cache_deleteOnExit = true; // preliminarily; will be unset if InstSrcManager accepts this.

  ///////////////////////////////////////////////////////////////////
  // create descr/data/media_dir
  ///////////////////////////////////////////////////////////////////

  res = PathInfo::assert_dir( cache_descr_dir(), 0700 );
  if ( res ) {
    ERR << "Unable to create descr_dir " << cache_descr_dir() << " (errno " << res << ")" << endl;
    return Error::E_cache_dir_create;
  }

  res = PathInfo::assert_dir( cache_data_dir(), 0700 );
  if ( res ) {
    ERR << "Unable to create data_dir " << cache_data_dir() << " (errno " << res << ")" << endl;
    return Error::E_cache_dir_create;
  }

  res = PathInfo::assert_dir( cache_media_dir(), 0700 );
  if ( res ) {
    ERR << "Unable to create media_dir " << cache_media_dir() << " (errno " << res << ")" << endl;
    return Error::E_cache_dir_create;
  }

  return Error::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::_init_newMedia
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrc::_init_newMedia( const Url & mediaurl_r, const Pathname & product_dir_r,
				 Type type_r )
{
  MIL << "Look for InstSrc type " << type_r << " on media " << mediaurl_r << endl;

  ///////////////////////////////////////////////////////////////////
  // prepare media
  ///////////////////////////////////////////////////////////////////
  PMError err;
  _media = new MediaAccess;

  if ( (err = _media->open( mediaurl_r, cache_media_dir() )) ) {
    ERR << "Failed to open " << mediaurl_r << " " << err << endl;
    return err;
  }

  if ( (err = _media->attach()) ) {
    ERR << "Failed to attach media: " << err << endl;
    return err;
  }

  ///////////////////////////////////////////////////////////////////
  // detect InstSrcDesc
  ///////////////////////////////////////////////////////////////////
  InstSrcDescrPtr ndescr;

  bool autodetect = ( type_r == T_AUTODETECT );

  Type ctype;
  for ( ctype = ( autodetect ? Type(1) : type_r );
	ctype < T_AUTODETECT && !ndescr; ctype = Type(ctype+1) ) {

    if ( autodetect ) {
      MIL << "Try InstSrc type " << ctype << endl;
    }

    switch ( ctype ) {

    case T_UnitedLinux:
	err = InstSrcDataUL::tryGetDescr( ndescr, _media, product_dir_r );
	break;

    case T_TEST_DIST:
	err = InstSrcData::tryGetDescr( ndescr, _media, product_dir_r );
	break;

    ///////////////////////////////////////////////////////////////////
    // no default: let compiler warn '... not handled in switch'
    ///////////////////////////////////////////////////////////////////
    case T_UNKNOWN:
    case T_AUTODETECT:
      break;
    }

    if ( err && ndescr ) {
      INT << "Note: reading InstSrcDescr returned data and error" << endl;
      ndescr = 0;
    }

    if ( !autodetect || ndescr )
      break; // no autodetect or found descr: break to preserve ctype
    else
      WAR << "No InstSrc type " << ctype << " found" << endl;

  } // for

  if ( !ndescr ) {
    ERR << "No InstSrc type " << type_r << " found on media " << mediaurl_r << endl;
    return Error::E_no_instsrc_on_media;
  }

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  if ( !err ) {
    // finalize data not located on media:
    ndescr->set_type( ctype );
    ndescr->set_url( mediaurl_r );
    ndescr->set_product_dir( product_dir_r );
    ndescr->set_default_activate( false );

    _descr = ndescr;
    MIL << "Found InstSrc " << _descr << endl;
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrc::dumpOn( ostream & str ) const
{
    Rep::dumpOn( str ) << "(";
    str << _descr << "|" << _data ;
    return str << ")";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::toString
//	METHOD TYPE : string
//
//	DESCRIPTION :
//
string InstSrc::toString( const Type t )
{
  switch ( t ) {

#define ENUM_OUT(V) case T_##V: return #V; break
    ENUM_OUT( UnitedLinux );
    ENUM_OUT( TEST_DIST );
    ENUM_OUT( AUTODETECT );
#undef ENUM_OUT

  ///////////////////////////////////////////////////////////////////
  // no default: let compiler warn '... not handled in switch'
  ///////////////////////////////////////////////////////////////////
  case T_UNKNOWN:
    break;
  }

  return "UNKNOWN";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::fromString
//	METHOD TYPE : InstSrc::Type
//
//	DESCRIPTION :
//
InstSrc::Type InstSrc::fromString( std::string s )
{
  for ( Type ctype = Type(1); ctype < T_AUTODETECT; ctype = Type(ctype+1) ) {
    if ( s == toString( ctype ) )
      return ctype;
  }
  return T_UNKNOWN;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : std::ostream &
**
**	DESCRIPTION :
*/
ostream & operator<<( ostream & str, const InstSrc::Type obj )
{
  return str << InstSrc::toString( obj );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::vconstruct
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrc::vconstruct( InstSrcPtr & nsrc_r, const Pathname & cachedir_r )
{
  nsrc_r = 0;
  MIL << "Create InstSrc from cache " << cachedir_r << endl;

  ///////////////////////////////////////////////////////////////////
  // read cache
  ///////////////////////////////////////////////////////////////////
  PMError err;
  InstSrcPtr nsrc( new InstSrc );

  if ( (err = nsrc->_init_openCache( cachedir_r )) ) {
    nsrc = 0;
    ERR << "Failed to create InstSrc from cache " << cachedir_r << " " << err << endl;
    return err;
  }

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  if ( !err ) {
    nsrc_r = nsrc;
  }
  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::vconstruct
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrc::vconstruct( InstSrcPtr & nsrc_r, const Pathname & cachedir_r,
			     const Url & mediaurl_r, const Pathname & product_dir_r,
			     Type type_r )
{
  nsrc_r = 0;
  MIL << "Create InstSrc type " << type_r
    << " from media " << mediaurl_r << " (" << product_dir_r << ")"
    << " using cache " << cachedir_r << endl;

  ///////////////////////////////////////////////////////////////////
  // quickchecks
  ///////////////////////////////////////////////////////////////////

  if ( ! mediaurl_r.isValid() ) {
    ERR << "Invalid url " << mediaurl_r << endl;
    return Error::E_bad_url;
  }

  ///////////////////////////////////////////////////////////////////
  // init cache
  ///////////////////////////////////////////////////////////////////
  PMError err;
  InstSrcPtr nsrc( new InstSrc );

  if ( (err = nsrc->_init_newCache( cachedir_r )) ) {
    nsrc = 0; // removes cache in destructor
    ERR << "Failed to create InstSrc cache " << cachedir_r << " " << err << endl;
    return err;
  }

  ///////////////////////////////////////////////////////////////////
  // access media and try to get an InstSrcDescr
  ///////////////////////////////////////////////////////////////////

  if ( (err = nsrc->_init_newMedia( mediaurl_r, product_dir_r, type_r )) ) {
    nsrc = 0; // removes cache in destructor
    ERR << "Unable to detect an InstSrc on " << mediaurl_r << " " << err << endl;
    return err;
  }

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  if ( !err ) {
    nsrc_r = nsrc;
  }
  return err;
}
