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

#include <y2pm/InstSrc.h>

#include <y2pm/MediaAccess.h>
#include <y2pm/InstSrcDescr.h>

#include <y2pm/InstSrcData.h>
#include <y2pm/InstSrcData_UL.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrc
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(InstSrc);

///////////////////////////////////////////////////////////////////

const Pathname InstSrc::_c_descr_dir( "DESCRIPTION" );
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
  MIL << "New InstSrc" << endl;
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
  MIL << "Delete InstSrc (" << (_cache_deleteOnExit ? "delete " : "keep " ) << _cache << ")" << endl;

  if ( _media )
    _media->close();

  if ( _cache_deleteOnExit ) {
    PathInfo::recursive_rmdir( _cache );
  }
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
      err = InstSrcData_UL::tryGetData( ndata, _media, _descr->descrdir() );

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
  PMError err;

#warning TBD save way to detach the InstSrcData
  // back references in _data to this and _media must be cleared, otherwise we
  // wont get zero refcounts.

  _data = 0;

  return err;
}

#if 0
//-----------------------------
// general functions

/**
 * clean up, e.g. remove all caches
 */
bool
InstSrc::Erase()
{
    D__ << __FUNCTION__ << std::endl;
    return false;
}

/**
 * @return description of Installation source
 * This is needed by the InstSrcMgr
 */
const InstSrcDescr *
InstSrc::getDescription() const
{
    D__ << __FUNCTION__ << std::endl;
    return _descr;
}

/**
 * register this source (store cache files etc)
 * return pathname of saved content file
 */
const Pathname
InstSrc::registerSource (void) const
{
    D__ << __FUNCTION__ << std::endl;
    return _descr->writeCache ();
}

//-----------------------------
// activation status

/**
 * return activation status
 */
bool
InstSrc::getActivation() const
{
    D__ << __FUNCTION__ << std::endl;
    return _descr->getActivation();
}


/**
 * temporary (de)activate source
 */
void
InstSrc::setActivation (bool yesno)
{
    D__ << __FUNCTION__ << std::endl;
    return _descr->setActivation (yesno);
}

//-----------------------------
// source content access

/**
 * return the number of selections on this source
 */
int
InstSrc::numSelections() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->numSelections();
}


/**
 * return the number of packages on this source
 */
int
InstSrc::numPackages() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->numPackages();
}


/**
 * return the number of patches on this source
 */
int
InstSrc::numPatches() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->numPatches();
}


/**
 * generate PMSolvable objects for each patch on the source
 * @return list of PMSolvablePtr on this source
 */
const std::list<PMSolvablePtr> *
InstSrc::getPatches() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->getPatches();
}
#endif

/**
 * generate PMSelection objects for each selection on the source
 * @return list of PMSelectionPtr on this source
 */
const std::list<PMSelectionPtr> *
InstSrc::getSelections() const
{
    D__ << __FUNCTION__ << std::endl;
    return _data->getSelections();
}

/**
 * generate PMPackage objects for each Item on the source
 * @return list of PMPackagePtr on this source
 * */
const std::list<PMPackagePtr> *
InstSrc::getPackages() const
{
    D__ << __FUNCTION__ << std::endl;
    if (!_data)
    {
	cerr << "InstSrc::getPackages() no _data" << endl;
	return 0;
    }
    return _data->getPackages();
}

/**
 * find list of packages
 * @return list of PMPackagePtr matching name ,[version] ,[release] ,[architecture]
 */
const std::list<PMPackagePtr>
InstSrc::findPackages (const std::list<PMPackagePtr> *packages, const string& name, const string& version, const string& release, const string& arch) const
{
    D__ << __FUNCTION__ << std::endl;
    if (!_data)
    {
	cerr << "InstSrc::findPackages() no _data" << endl;
	return std::list<PMPackagePtr>();
    }
cerr << _data << "->findPackages()" << endl;
    return _data->findPackages (packages, name, version, release, arch);
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
  // cache_descr_dir must exist, media_dir is created if missing.
  ///////////////////////////////////////////////////////////////////
  cpath( cache_descr_dir() );

  if ( ! cpath.isDir() ) {
    ERR << "No cache description " << cpath << endl;
    return Error::E_bad_cache_descr;
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
  _cache_deleteOnExit = true; // preliminarily

  ///////////////////////////////////////////////////////////////////
  // create media_dir
  ///////////////////////////////////////////////////////////////////

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
  PMError err;
  MIL << "Look for InstSrc type " << type_r << " on media " << mediaurl_r << endl;

  ///////////////////////////////////////////////////////////////////
  // prepare media
  ///////////////////////////////////////////////////////////////////
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
      err = InstSrcData_UL::tryGetDescr( ndescr, _media, product_dir_r );
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

    if ( autodetect ) {
      if ( !ndescr ) {
	WAR << "No InstSrc type " << ctype << " found" << endl;
      }
    } else {
      break; // no autodetect
    }

  } // for

  if ( !ndescr ) {
    ERR << "No InstSrc type " << type_r << " found on media " << mediaurl_r << endl;
    return Error::E_no_instsrc_on_media;
  }

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////

  ndescr->set_type( ctype );

  MIL << "Found InstSrc " << ndescr << endl;

#warning TBD finalize InstSrcDescr from media

  _descr = ndescr;

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
    Rep::dumpOn( str );
    return str;
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
  PMError err;
  MIL << "Create InstSrc from cache " << cachedir_r << endl;

  ///////////////////////////////////////////////////////////////////
  // read cache
  ///////////////////////////////////////////////////////////////////
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
  PMError err;
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
    nsrc->_cache_deleteOnExit = false;
    nsrc_r = nsrc;
  }
  return err;
}
