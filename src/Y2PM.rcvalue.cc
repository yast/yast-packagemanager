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

  File:       Y2PM.rcvalue.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>
#include <y2util/Date.h>
#include <y2util/Pathname.h>
#include <y2util/PathInfo.h>

#include <y2util/TaggedFile.h>
#include <y2util/TagCacheRetrieval.h>

#include <Y2PM.h>
#include <y2pm/InstTarget.h>
#include <y2pm/InstTargetError.h>
#include "PMRcValues.h"
#include "PMTextParse.h"

#undef Y2LOG
#define Y2LOG "Y2PMrc"

using namespace std;

///////////////////////////////////////////////////////////////////
namespace PM {
;//////////////////////////////////////////////////////////////////

// helper functions
struct TextParser {
  // read
  PMError retrieveData( TagCacheRetrieval & cache_r, const TaggedFile::Tag & tag_r, TextParse::SingleLine::text_type & data_r ) {
    data_r = tag_r.Data();
    return PMError::E_ok;
  }
  PMError retrieveData( TagCacheRetrieval & cache_r, const TaggedFile::Tag & tag_r, TextParse::MultiLine::text_type & data_r ) {
    cache_r.retrieveData( tag_r.Pos(), data_r );
    return PMError::E_ok;
  }
  template<typename _Tp>
  PMError assignValue( TagCacheRetrieval & cache_r, const TaggedFile::Tag & tag_r, _Tp & value_r ) {
    TextParse::TextConvert<_Tp> tconv;
    typename TextParse::TextConvert<_Tp>::text_type text;

    retrieveData( cache_r, tag_r, text );
    tconv.fromText( value_r, text );

    return PMError::E_ok;
  }
  // write
  PMError writeData( ostream & str, const TaggedFile::Tag & tag_r, const TextParse::SingleLine::text_type & data_r ) {
    str << '=' << tag_r.Name() << ": " << data_r << endl;
    return PMError::E_ok;
  }
  PMError writeData( ostream & str, const TaggedFile::Tag & tag_r, const TextParse::MultiLine::text_type & data_r ) {
    str << '+' << tag_r.Name() << ':' << endl;
    for ( TextParse::MultiLine::text_type::const_iterator it = data_r.begin(); it != data_r.end(); ++it ) {
      str << *it << endl;
    }
    str << '-' << tag_r.Name() << ':' << endl;
    return PMError::E_ok;
  }
  template<typename _Tp>
  PMError writeValue( ostream & str, const TaggedFile::Tag & tag_r, const _Tp & value_r ) {
    TextParse::TextConvert<_Tp> tconv;
    typename TextParse::TextConvert<_Tp>::text_type text;

    tconv.toText( value_r, text );
    writeData( str, tag_r, text );

    return PMError::E_ok;
  }
};

// Adding new rcvalues:
//
// - add new value to struct RcValues (PMRcValues.h)
// - add a tag for this value in enum Tags
// - define tag and add it to tagset in Y2pmRc Constructor
// - READ: in assign() extend switch and convert parsed value (from tagset).
//   Assign it to the new value in RcValues (passed as argument).
// - WRITE: in write() convert the rcvalue (passed as argument) and write
//   it to the file.
//
// Conversion of datatypes to/fromText: you may use/extend the default conversions
// provided in TextParse.h (common) and PMTextParse.h (y2pm specific types)
//
struct Y2pmRc : public RcValues, public TextParser {
  static const Pathname _defaultDir;
  static const string   _defaultFilename;
  static Pathname defaultRcPath() {
    return _defaultDir + _defaultFilename;
  }
  // name and version of tagset
  const string _rcname;
  const string _rcversion;
  //
  bool               _initialized;
  Pathname           _rcfile;
  TaggedFile::TagSet _tagset;

  ///////////////////////////////////////////////////////////////////
  //
  enum Tags {
    // magic
    RC_NAME,
    RC_VERSION,
    // data
    REQUESTEDLOCALES,
    // last entry:
    NUM_TAGS
  };

  ///////////////////////////////////////////////////////////////////
  //
  Y2pmRc()
    : _rcname( "y2pmrc" )
    , _rcversion( "1.0-0" )
    , _initialized( false )
  {
    _tagset.setAllowUnknownTags( true );
    // magic
    _tagset.addTag( "RCName",		RC_NAME,	TaggedFile::SINGLE );
    _tagset.addTag( "RCVersion",	RC_VERSION,	TaggedFile::SINGLE );

    // data
    _tagset.addTag( "requestedLocales",	REQUESTEDLOCALES,	TaggedFile::MULTI );
  }

  ///////////////////////////////////////////////////////////////////
  // assign tagset to some RcValues
  PMError assign( RcValues & rcValues_r ) {
    // As 'TagCacheRetrievalPtr cache' isn't srored or propagated
    // there's no need for a 'TagCacheRetrievalPtr' and we may omitt
    // an explicit call to 'cache.stopRetrieval();'
    TagCacheRetrieval cache( _rcfile );
    cache.startRetrieval();

    for ( Tags t = (Tags)0; t != NUM_TAGS; t = (Tags)(t+1) ) {
      TaggedFile::Tag * tg = _tagset.getTagByIndex( t );
      if ( !tg ) {
	INT << "No tag " << t << " in tagset!" << endl;
	continue;
      }
      switch ( t ) {

      // magic
      case RC_NAME:
	if ( tg->Pos().empty() || tg->Data() != _rcname ) {
	  ERR << "Bad Magic '" << tg->Data() << "', expected '" << _rcname << "'" << endl;
	  return PMError::E_error; //Error::E_parse_bad_magic;
	}
	break;
      case RC_VERSION:
	if ( tg->Pos().empty() || tg->Data() != _rcversion ) {
	  ERR << "Unsopported " << _rcname << " version '" << tg->Data()
	    << "', expected '" << _rcversion << "'" << endl;
	  return PMError::E_error; //Error::E_parse_unsupported_version;
	}
	break;

      // data
      case REQUESTEDLOCALES:
	assignValue( cache, *tg, rcValues_r.requestedLocales );
	break;

      // last entry:
      case NUM_TAGS:
	break;
      }
    }

    return PMError::E_ok;
  }

  ///////////////////////////////////////////////////////////////////
  // write values to stream
  PMError write( ostream &  rcstream_r, const RcValues & rcValues_r ) {
    if ( ! rcstream_r ) {
      ERR << "bad stream to write" << endl;
      return PMError::E_error;
    }
    PMError err;

    rcstream_r << "## ------------------------------------------------------" << endl;
    rcstream_r << "## Generated " << Date(Date::now()) << endl;
    rcstream_r << "## Do not modify!" << endl;
    rcstream_r << "## ------------------------------------------------------" << endl;

    for ( Tags t = (Tags)0; t != NUM_TAGS; t = (Tags)(t+1) ) {
      TaggedFile::Tag * tg = _tagset.getTagByIndex( t );
      if ( !tg ) {
	INT << "No tag " << t << " in tagset!" << endl;
	continue;
      }
      switch ( t ) {

      // magic
      case RC_NAME:
	writeValue( rcstream_r, *tg, _rcname );
	break;
      case RC_VERSION:
	writeValue( rcstream_r, *tg, _rcversion );
	rcstream_r << "## ------------------------------------------------------" << endl;
	break;

      // data
      case REQUESTEDLOCALES:
	writeValue( rcstream_r, *tg, rcValues_r.requestedLocales );
	break;

      // last entry:
      case NUM_TAGS:
	break;
      }
    }

    if ( ! rcstream_r ) {
      ERR << "error writing to stream" << endl;
      err = PMError::E_error;
    }
    return err;
  }

  ///////////////////////////////////////////////////////////////////
  // parse from stream into tagset
  PMError parse( istream & rcstream_r ) {
    PMError err;
    if ( rcstream_r ) {
      TaggedParser parser;
      TaggedFile::assignstatus status = _tagset.assignSet( parser, rcstream_r );
      DBG << "assignstatus == " << status << endl;
    } else {
      ERR << "bad stream to parse" << endl;
      err = PMError::E_error;
    }
    return err;
  }
  ///////////////////////////////////////////////////////////////////
  // parse some file into tagset
  PMError parse( const Pathname & rcfile_r ) {
    ifstream rcstream( rcfile_r.asString().c_str() );
    PMError err = parse( rcstream );
    if ( err ) {
      ERR << "Parse '" << rcfile_r << "' failed: " << err << endl;
    }
    return err;
  }

  ///////////////////////////////////////////////////////////////////
  // write values to file
  PMError write( const Pathname & rcfile_r, const RcValues & rcValues_r ) {
    ofstream rcstream( rcfile_r.asString().c_str() );
    PMError err = write( rcstream, rcValues_r );
    if ( err ) {
      ERR << "Write '" << rcfile_r << "' failed: " << err << endl;
    }
    return err;
  }

  ///////////////////////////////////////////////////////////////////
  // parse rcfile and assign it to this RcValues
  PMError initFrom( const Pathname & rcfile_r ) {
    PMError err;
    _initialized = true;
    // init either from file or defaults
    if ( ! rcfile_r.empty() ) {
      err = parse( rcfile_r );
      if ( ! err ) {
	_rcfile = rcfile_r;
	err = assign( *this );
      }
    }
    return err;
  }

  ///////////////////////////////////////////////////////////////////
  // Write this RcValues to rcfile_r (using a tmpfile if rcfile_r
  // already exists).
  PMError saveTo( const Pathname & rcfile_r ) {

    PathInfo p( rcfile_r.dirname() );
    if ( ! p.isDir() ) {
      ERR << "Parent directory does not exist: " << p << endl;
      return PMError::E_error;
    }

    p( rcfile_r );
    if ( p.isExist() && ! p.isFile() ) {
      ERR << "Is not a file: " << p << endl;
      return PMError::E_error;
    }

    // use a tmpfile if rcfile_r exists
    unsigned tmpext = 0;
    while( p.isExist() ) {
      ++tmpext;
      p( rcfile_r.extend( stringutil::numstring( tmpext ) ) );
    }

    PMError err = write( p.path(), *this );
    if ( ! err && tmpext ) {
      int res = PathInfo::rename( p.path(), rcfile_r );
      if ( res ) {
	ERR << "Error(" << res << ") renaming " << p << " to " << rcfile_r.basename() << endl;
	err = PMError::E_error;
      }
    }
    if ( err ) {
      // either on write or remaning the tmpfile
      PathInfo::unlink( p.path() );
    }

    return err;
  }
};

const Pathname Y2pmRc::_defaultDir( "/var/adm/YaST/y2pm" );
const string Y2pmRc::_defaultFilename( "y2pmrc" );

static Y2pmRc _yp2pmrc;

///////////////////////////////////////////////////////////////////

/******************************************************************
**
**
**	FUNCTION NAME : rcValues
**	FUNCTION TYPE : RcValues &
*/
RcValues & rcValues()
{
  if ( ! _yp2pmrc._initialized ) {
    Y2PM::rcInit();
  }
  return _yp2pmrc;
}

///////////////////////////////////////////////////////////////////
} // namespace PM
///////////////////////////////////////////////////////////////////

using PM::_yp2pmrc;

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::rcInit
//	METHOD TYPE : PMError
//
PMError Y2PM::rcInit()
{
  if ( _yp2pmrc._initialized ) {
      MIL << "Already initialized: ";
    if ( _yp2pmrc._rcfile.empty() ) {
      MIL << "defaults" << endl;
    } else {
      MIL << "file " << _yp2pmrc._rcfile << endl;
    }
    return PMError::E_ok;
  }

  PMError err;
  Pathname rcfile( _yp2pmrc.defaultRcPath() );

  // get path to use in case we're not running from system
  if ( ! runningFromSystem() ) {
    if ( _instTarget && instTarget().initialized() ) {
      rcfile = instTarget().rootdir() + rcfile;
    } else {
      rcfile = Pathname();
      err = InstTargetError::E_not_initialized;
      ERR << "Unable to access rcfile: " << err << endl;
    }
  }

  // check if path to use exists
  if ( ! ( rcfile.empty() || PathInfo( rcfile ).isFile() ) ) {
    WAR << "rcfile does not exist: " << rcfile << endl;
    rcfile = Pathname();
  }

  // init either from file or defaults
  MIL << "Initialize using: ";
  if ( rcfile.empty() ) {
    MIL << "defaults" << endl;
  } else {
    MIL << "file " << rcfile << endl;
  }

  err = _yp2pmrc.initFrom( rcfile );
  if ( err ) {
    ERR << "Initialize returned " << err << endl;
  }

  // postprocess
  if ( runningFromSystem() ) {
    if ( rcfile.empty() ) {
      // Writing an initial rcfile:

      // Use PreferredLocale for locale dependent packages in selections.
      // This is how old versions did it.
      _yp2pmrc.requestedLocales.insert( getPreferredLocale() );

      // Make shure new rcfioe gets written.
      _yp2pmrc._rcfile = _yp2pmrc.defaultRcPath();

      // Writing an initial rcfile
      MIL << "Writing initial rcfile..." << endl;
      rcSave();
    }
  } else {
    // Disable automated writing to system. If new settings are to be copied
    // to the system, it's done at the end when updating the InstSrces.
    _yp2pmrc._rcfile = Pathname();
  }

  return err;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : Y2PM::rcSave
//	METHOD TYPE : PMError
//
PMError Y2PM::rcSave()
{
  if ( ! _yp2pmrc._initialized ) {
    WAR << "rcvalues not yet initialized." << endl;
    return PMError::E_ok;
  }
  if ( _yp2pmrc._rcfile.empty() ) {
    WAR << "Writing rcfile disabled." << endl;
    return PMError::E_ok;
  }

  Pathname rcfile( _yp2pmrc._rcfile );

  int res = PathInfo::assert_dir( rcfile.dirname() );
  if ( res ) {
    ERR << "Error(" << res << ") creating directory '" << rcfile.dirname() << "'" << endl;
    return PMError::E_error;
  }

  PMError err = _yp2pmrc.saveTo( rcfile );
  if ( err ) {
    ERR << "Save returned " << err << endl;
  }

  return err;
}

