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
#include <cctype>

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>
#include <y2util/stringutil.h>

#include <Y2PM.h>
#include <y2pm/Timecount.h>

#include <y2pm/InstTarget.h>
#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcError.h>
#include <y2pm/InstSrcManagerCallbacks.h>

#include <y2pm/MediaAccess.h>
#include <y2pm/InstSrcDescr.h>

#include <y2pm/InstSrcData.h>
#include <y2pm/InstSrcDataUL.h>
#include <y2pm/InstSrcDataPLAIN.h>
#include <y2pm/F_Media.h>

#include <y2pm/RpmHeader.h>

using namespace std;
using namespace InstSrcManagerCallbacks;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrc
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(InstSrc);

///////////////////////////////////////////////////////////////////

const PM::NumericISrcID InstSrc::noID( PM::NumericISrcID(-1) );
PM::NumericISrcID       InstSrc::_SRCID( 0 );

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
    : _srcID( ++_SRCID )
    , _cache_deleteOnExit( false )
    , _may_use_cache( true )
    , _specialCache( -1 )
    , _medianr (0)
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
  MIL << "Delete InstSrc " << *this << "(" << (_cache_deleteOnExit ? "delete " : "keep " ) << _cache << ")" << endl;

  if ( _media )
  {
    _media->release();
    if ( _media->close() ) {
      if ( _cache_deleteOnExit ) {
	ERR << "Media close failed! Can't delete cache!" << endl;
      }
      _media = NULL;
      return;
    }
    _media = NULL;
  }

  if ( _cache_deleteOnExit ) {
    PathInfo::recursive_rmdir( _cache );
  } else {
    // cleanup below media_dir
    PathInfo mediadir( cache_media_dir(), PathInfo::LSTAT );
    if ( mediadir.isDir() ) {
      PathInfo::clean_dir( mediadir.path() );
    }
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::preferredLocaleChanged
//	METHOD TYPE : void
//
void InstSrc::preferredLocaleChanged() const
{
  if ( _data ) {
    _data->preferredLocaleChanged();
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::specialCache
//	METHOD TYPE : bool
//
//	DESCRIPTION : Hack for adrian to enforce package
//                    candidate selection from this source.
//
bool InstSrc::specialCache() const
{
  if ( _specialCache == -1 ) {
    _specialCache = ( _cache.basename() == "IS_CACHE_0x00000000" ? 1 : 0 );
  }
  return  _specialCache;
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
  writeDescrCache(); // data are written on enable
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
    DBG << "Is enabled." << endl;
    return Error::E_ok;
  }

  if ( !_descr ) {
    ERR << "Cannot enable without source description" << endl;
    return Error::E_src_no_description;
  }


  //-----------------------------------------------------------------
  // Determine search path for providePackage
#warning Thats actually nothing InstSrc has to deal with. It is InstSrcDataUL specific!!!

  //std::map<std::string,std::list<Pathname> >
  InstSrcDescr::ArchMap archmap = _descr->content_archmap();
  InstSrcDescr::ArchMap::const_iterator archIt = archmap.find ((const std::string &)(Y2PM::baseArch()));

  if ( _descr->type() == T_UnitedLinux ) {
    if (archIt == archmap.end())
    {
      WAR << "No 'ARCH." << Y2PM::baseArch() << "' line, using ARCH." << _descr->content_defaultbase() << endl;
      archIt = archmap.find (_descr->content_defaultbase());
    }
    if (archIt == archmap.end())
    {
      ERR << "Unable to determine ARCH. line" << endl;
      return Error::E_src_no_description;
    }
  }

  ///////////////////////////////////////////////////////////////////
  // create InstSrcData according to Type stored in InstSrcDescr
  // and let it load it's data.
  ///////////////////////////////////////////////////////////////////
  Timecount _t( "InstSrc::enableSource" );

  PMError err;
  InstSrcDataPtr ndata;

  switch ( _descr->type() ) {

  case T_UnitedLinux:
    err = InstSrcDataUL::tryGetData( this, ndata, _media, _descr->descrdir(), archIt->second, Y2PM::getPreferredLocale() );
    break;

  case T_PlainDir:
    err = InstSrcDataPLAIN::tryGetData( ndata, this );
    break;

    ///////////////////////////////////////////////////////////////////
    // no default: let compiler warn '... not handled in switch'
    ///////////////////////////////////////////////////////////////////
  case T_UNKNOWN:
  case T_AUTODETECT:
    break;
  }

  if ( !ndata ) {
    ERR << "No InstSrcData type " << _descr->type() << " found on media " << _descr->url() << endl;
    return Error::E_no_instsrc_on_media;
  }

  if ( err ) {
    ERR << "Error retrieving InstSrcData: " << err << endl;
    return err;
  }

  ///////////////////////////////////////////////////////////////////
  // done
  ///////////////////////////////////////////////////////////////////
  if ( !err ) {
#warning TBD which is the correct list of allowed architectures if multiple products are installed ?
    if ( _descr->type() == T_UnitedLinux ) {
      if (Y2PM::allowedArchs().empty())
	Y2PM::setAllowedArchs (archIt->second);
    }
    _data = ndata;
    _data->_instSrc_attach( this );
    _data->_instSrc_propagate();    // propagate Objects to Manager classes.
    writeDescrCache();
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
    DBG << "Is disabled." << endl;
    return Error::E_ok;
  }

  ///////////////////////////////////////////////////////////////////
  // Remove back references to InstSrc in _data. Otherwise we
  // wont get zero refcounts.
  ///////////////////////////////////////////////////////////////////
  PMError err;

  if (_media)
  {
    if (_media->isAttached())
      _media->release();
    _media->close();
  }

  _data->_instSrc_withdraw(); // withdraw Objects from Manager classes.
  _data->_instSrc_detach();   // clear backreferences to InstSrc.
  _data = 0;

  // cleanup below media_dir
  PathInfo mediadir( cache_media_dir(), PathInfo::LSTAT );
  if ( mediadir.isDir() ) {
    PathInfo::clean_dir( mediadir.path() );
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::writeDescrCache
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrc::writeDescrCache()
{
  if ( ! _may_use_cache ) {
    MIL << "WriteCache disabled" << endl;
    return Error::E_src_cache_disabled;
  }

  if ( _descr ) {
    return _descr->writeCache( cache_descr_dir() );
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
  ///////////////////////////////////////////////////////////////////

  // cache_descr_dir must exist.
  cpath( cache_descr_dir() );
  if ( ! cpath.isDir() ) {
    ERR << "No cache description " << cpath << endl;
    return Error::E_bad_cache_descr;
  }

  // cache_data_dir might exist. if, it must be a directory
  cpath( cache_data_dir() );
  if ( cpath.isExist() ) {
    if ( !cpath.isDir() ) {
      ERR << "data_dir is not a directory " << cpath << endl;
      return Error::E_cache_dir_create;
    }
  }

  // cache_media_dir is created if missing.
  cpath( cache_media_dir() );
  if ( cpath.isExist() ) {
    if ( !cpath.isDir() ) {
      ERR << "media_dir is not a directory " << cpath << endl;
      return Error::E_cache_dir_create;
    }
  } else {
    int res = PathInfo::assert_dir( cpath.path() );
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

  int res = PathInfo::mkdir( cpath.path() );
  if ( res ) {
    ERR << "Unable to create cache dir " << cpath << " (errno " << res << ")" << endl;
    return Error::E_cache_dir_create;
  }

  _cache = cachedir_r;
  _cache_deleteOnExit = true; // preliminarily; will be unset if InstSrcManager accepts this.

  ///////////////////////////////////////////////////////////////////
  // create media_dir. descr/data dir, if mayUseCache
  ///////////////////////////////////////////////////////////////////

  if ( Y2PM::runningFromSystem() || Y2PM::cacheToRamdisk() ) {
    res = PathInfo::assert_dir( cache_descr_dir() );
    if ( res ) {
      ERR << "Unable to create descr_dir " << cache_descr_dir() << " (errno " << res << ")" << endl;
      return Error::E_cache_dir_create;
    }

    res = PathInfo::assert_dir( cache_data_dir() );
    if ( res ) {
      ERR << "Unable to create data_dir " << cache_data_dir() << " (errno " << res << ")" << endl;
      return Error::E_cache_dir_create;
    }
  } else {
    _may_use_cache = false;
    MIL << "descr/data caches disabled" << endl;
  }

  res = PathInfo::assert_dir( cache_media_dir() );
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

    case T_PlainDir:
	err = InstSrcDataPLAIN::tryGetDescr( ndescr, this, _media, product_dir_r, mediaurl_r );
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

    _descr = ndescr;
    MIL << "Found InstSrc " << _descr << endl;

#warning Fix setup of source data
    if ( Y2PM::runningFromSystem() && ctype == T_UnitedLinux ) {
      PathInfo cpath( cache_data_dir() );
      MIL << "Try to load cache " << cpath << endl;
      if ( cpath.isDir() ) {
	InstSrcDataUL::initDataCache( cpath.path(), this );
      }
      _media->release();
    }
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
    ENUM_OUT( PlainDir );
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
** private
**
**	FUNCTION NAME : provideMedia
**	FUNCTION TYPE : PMError
**
**	DESCRIPTION : provide media by medianr
**
*/
#warning NEEDS REWRITE: provideMedia
PMError InstSrc::provideMedia ( int medianr ) const
{
  if ( _medianr == medianr ) {
    return InstSrcError::E_ok; // everything ok
  }

  PMError err;
  Url url = _descr->url();
  bool triedReOpen = false;

  MediaChangeReport::Send report( mediaChangeReport );

  while ( true ) {
    err = Error::E_ok;
    MIL << "Looking for media " << medianr
      << " ("  << _descr->media_vendor() << " | " << _descr->media_id() << ") " << endl;

    // make shure media is open with correct Url
    if ( ! _media->isOpen() ) {
      _medianr = 0;
      err = _media->open ( url, cache_media_dir() );
    }

    // make shure media is attached
    if ( ! err && !_media->isAttached() ) {
      _medianr = 0;
      err = _media->attach();
    }

    // confirm it's the correct media
    if ( ! err ) {
#warning Actually it's InstSrcData which has to somehow confirm it's the correct media.

      Pathname mediafile( stringutil::form( "/media.%d/media", medianr ) );
      err = _media->provideFile( mediafile );

      if ( ! err ) {
	// read mediafile
	F_Media mediaf;
	err = mediaf.quickread( _media->localPath( mediafile ) );

	if ( ! err ) {
	  // check vendor and id
	  if (    mediaf.vendor() == _descr->media_vendor()
	       && mediaf.ident()  == _descr->media_id() ) {

	    _medianr = medianr; // everything ok
	    return InstSrcError::E_ok; // -----------------------------> return ok
	  }

	  // NOTE: We've got the medianr, but with wrong mediaident. Former versions
	  // asked via callback and offered to ignore the wrong mediaident. This might
	  // be ok for a single source product, but not for multisource (e.g. SLES/CORE).
	  //
	  // At least the content file should be scanned, to check whether product id and
	  // version match, before offering to use the media as substitute.

	}
      }
    }

    ///////////////////////////////////////////////////////////////////
    // Here: We've got no or a wrong media
    ///////////////////////////////////////////////////////////////////
    DBG << "Attempt result: " << err << endl;

    // try next device if media supports it
    if ( _media->isOpen() ) {
      _media->release();
      err = _media->attach( true ); // try next device of media
      if ( ! err )
	continue;               // ------------------------------------> continue
    }

    // No more devices to try, so have a look at the pathname.
    // If it ends in digits, replace them with medianumber.
    if ( ! triedReOpen ) {
      triedReOpen = true; // don't come here again
      string path = url.path();
      string::size_type pos = path.find_last_not_of( '/' );
      if ( pos != string::npos && ++pos != path.size() ) {
	path.erase( pos );
      }
      pos = path.find_last_not_of( "1234567890" );
      if ( pos != string::npos && ++pos != path.size() ) {
	string mnum( stringutil::numstring( medianr ) );
	path.erase( pos );
	path += mnum;

	// give it a try
	_media->close();
	url.setPath( path );
	continue;             // ------------------------------------> continue
      }
    }

    ///////////////////////////////////////////////////////////////////
    // Here: No device and no alternate path succeeded :(
    ///////////////////////////////////////////////////////////////////

    // Give up if there is no callback set
    if ( ! report->isSet() ){
      _media->close();
      ERR << "Can't find medium, can't ask user" << endl;
      return InstSrcError::E_skip_media; // ---------------------------> return
    }

    // ask what to do...
    string changereply;
    _media->release();

    while ( true ) {
      changereply = report->changeMedia( descr(), url, medianr, err );
      if ( changereply == "E" ) {
	_media->release( true );  // eject
	continue;                 // re-prompt after eject
      }
      break; // leave
    }

    if ( changereply == "S" ) {		        // skip
      _media->close();
      MIL << "Skip media" << endl;
      return InstSrcError::E_skip_media;        // ---> return
    }
    else if ( changereply == "C" ) {		// cancel installation
      _media->close();
      MIL << "Cancel media" << endl;
      return InstSrcError::E_cancel_media;      // ---> return
    }
    else if ( ! changereply.empty() ) {
      _media->close();
      url = Url( changereply );
      MIL << "Retry url '" << url << "'" << endl;
    }
    // else retry

  } // while


  // actually this should not be reached:
  _media->close();
  return InstSrcError::E_skip_media;
}


/******************************************************************
**
**
**	FUNCTION NAME : providePackage
**	FUNCTION TYPE : PMError
**
**	DESCRIPTION : provide package file by medianr and directory
**		return local path in path_r, even on error
*/
PMError
InstSrc::providePackage (int medianr, const Pathname& name, const Pathname& dir, Pathname& path_r) const
{
    PMError err = provideMedia (medianr);
    if (err != PMError::E_ok)
	return err;

    Pathname filename = _descr->datadir() + dir + name;

    MediaAccessPtr theMedia( _media );

    if ( ! Y2PM::runningFromSystem() && isRemote() && Y2PM::instTarget().initialized() ) {
      // A little bit different handling for ftp/http
      // during installation/update. Try not to download
      // into the ramdisk, but use tmp dir on the harddisk.
      PathInfo tmpdir( Y2PM::instTarget().rootdir() + "/var/adm/YaST/InstSrcManager/tmp" );

      if ( ! tmpdir.isDir() ) {
	PathInfo::assert_dir( tmpdir.path() );
	tmpdir(); // (re)stat
      }

      if ( tmpdir.isDir() ) {
	theMedia = new MediaAccess;
	if ( theMedia->open( _media->url(), tmpdir.path() ) || theMedia->attach() ) {
	  // error, so we try it the common way
	  theMedia = _media;
	}
      }
    }

    err = theMedia->provideFile( filename );

    if (err != PMError::E_ok)
    {
 	ERR << "Media can't provide '" << dir+name << "': " << err << endl;
	path_r = filename;		// pass back complete filename of missing package
	return err;
    }

    path_r = theMedia->localPath( filename );

    if ( isRemote() && ! RpmHeader::readPackage( path_r, RpmHeader::NOSIGNATURE ) ) {
      err = Error::E_corrupted_file;
      err.setDetails( (dir+name).asString() );
      PathInfo::unlink( path_r );
      ERR << "Bad digest '" << path_r << "': " << err << endl;
      path_r = filename;		// pass back complete filename of missing package
      return err;
    }

    rememberPreviouslyDnlPackage( path_r ); // Hack not to keep more than one downloaded package

    return PMError::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::rememberPreviouslyDnlPackage
//	METHOD TYPE : void
//
void InstSrc::rememberPreviouslyDnlPackage( const Pathname & newpath_r ) const
{
#warning Hack not to keep more than one downloaded package
  if ( isRemote() && previouslyDnlPackage != newpath_r ) {
    if ( ! previouslyDnlPackage.empty() ) {
      PathInfo::unlink( previouslyDnlPackage );
    }
    previouslyDnlPackage = newpath_r;
  }
}

/******************************************************************
**
**
**	FUNCTION NAME : provideFile
**	FUNCTION TYPE : PMError
**
**	DESCRIPTION : provide file by medianr and relative path
**		return local path in file_r
*/
PMError
InstSrc::provideFile (int medianr, const Pathname& path, Pathname& file_r) const
{
    PMError err = provideMedia (medianr);
    if (err != InstSrcError::E_ok)
	return err;

    err = _media->provideFile (path);
    if (err != PMError::E_ok)
    {
	ERR << "Media can't provide '" << path << "': " << err << endl;
	return err;
    }
    file_r = _media->localPath (path);
    return PMError::E_ok;
}


/******************************************************************
**
**
**	FUNCTION NAME : provideDir
**	FUNCTION TYPE : PMError
**
**	DESCRIPTION : provide directory by medianr and relative path
**		return local path in dir_r
*/
PMError
InstSrc::provideDir (int medianr, const Pathname& path, Pathname& dir_r) const
{
    PMError err = provideMedia (medianr);
    if (err != InstSrcError::E_ok)
	return err;

    err = _media->provideDir (path);
    if (err != PMError::E_ok)
    {
	ERR << "Media can't provide '" << path << "': " << err << endl;
	return err;
    }
    dir_r = _media->localPath (path);
    return PMError::E_ok;
}


/******************************************************************
**
**
**	FUNCTION NAME : isRemote
**	FUNCTION TYPE : bool
**
**	DESCRIPTION : if the media needs some time to provide a package
*/
bool
InstSrc::isRemote () const
{
  Url::Protocol protocol = _media->protocol();

  if ( protocol == Url::unknown && _descr ) {
    // not yet attached. check descr url
    protocol = _descr->url().protocol();
  }

  switch ( protocol ) {
  case Url::ftp:
  case Url::http:
  case Url::https:
    return true;

  default:
    break;
  }

  return false;
}


/******************************************************************
**
**
**	FUNCTION NAME : changeUrl
**	FUNCTION TYPE : bool
**
**	DESCRIPTION : change Url of media
*/
PMError
InstSrc::changeUrl (const Url & newUrl_r)
{
    MIL << "changeUrl (" << newUrl_r << ")" << endl;
    if ( !newUrl_r.isValid() )
    {
	WAR << "Won't change to invalid url: " << newUrl_r << endl;
	return InstSrcError::E_bad_url;
    }

    _descr->set_url( newUrl_r );
    writeDescrCache();

    if (_media->isOpen())
    {
	_medianr = 0;
	_media->release();
	_media->close();
	return _media->open (newUrl_r, cache_media_dir());
    }
    return PMError::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrc::releaseMedia
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrc::releaseMedia( bool if_removable_r ) const
{
  _medianr = 0;

  if ( !_media->isAttached() )
    return Error::E_ok;

  if ( if_removable_r
       && _media->protocol() != Url::cd
       && _media->protocol() != Url::dvd )
    return Error::E_ok;

  return _media->release();
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
