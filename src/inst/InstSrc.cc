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
#include <ctype.h>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>

#include <Y2PM.h>

#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcError.h>

#include <y2pm/MediaAccess.h>
#include <y2pm/InstSrcDescr.h>

#include <y2pm/InstSrcData.h>
#include <y2pm/InstSrcDataUL.h>
#include <y2pm/InstSrcDataPLAIN.h>

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
    , _may_use_cache( true )
    , _specialCache( -1 )
    , _mediachangefunc (0)
    , _mediachangedata (0)
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
    _media->close();
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
    ERR << "Already enabled." << endl;
    return Error::E_src_already_enabled;
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
    ERR << "Not enabled." << endl;
    return Error::E_src_not_enabled;
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
    _descr->writeCache( cache_descr_dir() );
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


static
std::string number2string (int nr)
{
    static int digits[] = { 1000, 100, 10, 1 };
    static char num[5];
    char *nptr = num;
    if (nr > 1000)
	return string();
    for (int pos = 0; pos < 4; ++pos)
    {
	int digit = nr / digits[pos];
	if (digit > 0)
	{
	    *nptr++ = '0' + digit;
	    nr -= digit * digits[pos];
	}
    }
    *nptr = 0;

    return string (num);
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
PMError
InstSrc::provideMedia (int medianr) const
{
    PMError err;
    string reply;

    // if the url ends with digits, try re-opening with
    // digits replaced by medianr
    bool triedReOpen = false;

    Url url = _descr->url();
    string product = _descr->label();
    while (medianr != _medianr)
    {
	if (!_media->isAttached())
	{
	    InstSrcPtr ptr = InstSrcPtr::cast_away_const (this);
	    ptr->_medianr = 0;

	    err = _media->attach();

	    if (err == MediaError::E_not_open)
	    {
		MIL << "Not open, doing initial open '" << url << "'" << endl;
		err = _media->open (url, cache_media_dir());
		if (err != MediaError::E_ok)
		{
		    ERR << "open (" << url << ") failed: " << err << endl;
		}
		else
		{
		    err = _media->attach();
		}
	    }

	}

	Pathname mediafile ("/media."+number2string(medianr)+"/media");

	err = _media->provideFile (mediafile);
	if (err == MediaError::E_ok)
	{
	    // open media file
	    std::ifstream mediaf (_media->localPath (mediafile).asString().c_str());
	    if (mediaf)
	    {
		char vendor[200];
		char id[200];
		vendor[0] = 0;
		id[0] = 0;
		if ((mediaf.getline (vendor, 200, '\n'))
		    && (string (vendor) == _descr->media_vendor()))	// check vendor
		{
		    if ((mediaf.getline (id, 200, '\n'))
			&& (string (id) == _descr->media_id()))		// check id
		    {
			InstSrcPtr ptr = InstSrcPtr::cast_away_const (this);
			ptr->_medianr = medianr;			// everything ok
			mediaf.close();
			break;
		    }
		}
		mediaf.close();
		MIL << "vendor '" << vendor << "' id '" << id << "'" << endl;
		if (_mediachangefunc != 0)
		{
		    string error = string(vendor) + " != " + (const std::string &)(_descr->media_vendor());
		    if (id[0] != 0)
		    {
			error = error + "<br>\n" + id + " != " + _descr->media_id();
		    }

		    //---------------------------------------------------------
		    // wrong media ID number callback

		    _media->release();

		    string changereply;
		    for (;;)
		    {
			changereply = (*_mediachangefunc) (error, url.asString(), product, -1, medianr, _mediachangedata);

			if (changereply != "E")		// eject
			    break;
			_media->attach();
			_media->release(true);
		    }					// re-prompt after eject

		    if (changereply == "S")			// skip
		    {
			return InstSrcError::E_skip_media;
		    }
		    else if (changereply == "I")		// ignore
		    {
			_media->attach();
			MIL << "ignore bad media id" << endl;
			reply = "";
			break;
		    }
		    else if (changereply == "C")		// cancel installation
		    {
			return InstSrcError::E_cancel_media;
		    }
		    else if (changereply.empty())
		    {
			continue;
		    }
		    else					// new url
		    {
			url = Url(changereply);
			if (url.isValid())
			{
			    _media->close();				// close medium
			    MIL << "Retry url '" << url << "'" << endl;
			    continue;
			}
		    }
		}

	    } // media file ok
	}
	else
	{
	    MIL << "provideFile failed: " << err.errstr() << endl;
	}

	_media->release();

	if (_media->attach (true) == PMError::E_ok)	// retry next device of media
	    continue;

	std::string path = url.getPath();

	if (!triedReOpen)
	{
	    while (path[path.size()-1] == '/')		// erase trailing '/'
		path.erase (path.size()-1);
	}

	if (!triedReOpen
	    && isdigit(path[path.size()-1]))
	{
	    triedReOpen = true;				// don't come here again
	    MIL << "Closing path '" << path << "'" << endl;
	    _media->close();				// close medium
	    while (isdigit(path[path.size()-1])
		   && !path.empty())
	    {
		path.erase (path.size()-1);		// remove trailing digits
	    }
	    path += number2string (medianr);		// attach medianr
	    url.setPath (path);
	    MIL << "Re-open url '" << url << "'" << endl;
	    continue;					// try re-open
	}

	if (_mediachangefunc == 0)
	{
	    ERR << "Can't find medium, can't ask user" << endl;
	    medianr = 0;
	    break;
	}

	// wrong media number callback
	//---------------------------------------------------------

	string changereply;
	for (;;)
	{
	    changereply = (*_mediachangefunc) (err.errstr(), url.asString(), product, _medianr, medianr, _mediachangedata);

	    if (changereply != "E")		// eject
		break;
	    _media->attach();
	    _media->release(true);
	}					// re-prompt after eject

	if (changereply == "S")			// skip
	{
	    MIL << "skip media" << endl;
	    return InstSrcError::E_skip_media;
	}
	else if (changereply == "C")		// cancel installation
	{
	    MIL << "cancel media" << endl;
	    return InstSrcError::E_cancel_media;
	}
	else if (changereply.empty())
	{
	    _media->attach();
	}
	else					// new url
	{
	    _media->close();			// close medium
	    url = Url(changereply);
	}
    }

    if (reply != "")
    {
	_media->release();
	return InstSrcError::E_no_media;
    }

    InstSrcPtr isptr = InstSrcPtr::cast_away_const (this);
    isptr->_medianr = medianr;			// everything ok

    return InstSrcError::E_ok;
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
    err = _media->provideFile (filename);

    if (err != PMError::E_ok)
    {
	ERR << "Media can't provide '" << dir+name << "': " << err.errstr() << endl;
	path_r = filename;		// pass back complete filename of missing package
	return err;
    }

    path_r = _media->localPath (filename);
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
	ERR << "Media can't provide '" << path << "': " << err.errstr() << endl;
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
	ERR << "Media can't provide '" << path << "': " << err.errstr() << endl;
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
  MediaAccess::MediaType mtype = _media->type();

  if ( mtype == MediaAccess::NONE && _descr ) {
    // not yet attached. check descr url
    mtype = MediaAccess::typeOf( _descr->url() );
  }

  switch ( mtype ) {
  case MediaAccess::FTP:
  case MediaAccess::HTTP:
  case MediaAccess::HTTPS:
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
PMError
InstSrc::releaseMedia( bool if_removable_r )
{
  _medianr = 0;

  if ( !_media->isAttached() )
    return Error::E_ok;

  if ( if_removable_r
       && _media->type() != MediaAccess::CD
       && _media->type() != MediaAccess::DVD )
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
