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

  File:       F_Media.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>

#include <y2pm/F_Media.h>
#include <y2pm/InstSrcError.h>

using namespace std;

///////////////////////////////////////////////////////////////////

const PM::LocaleString F_Media::_noLabel; // empty

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : F_Media::F_Media
//	METHOD TYPE : Constructor
//
F_Media::F_Media()
    : _count( 0 )
    , _flags( 0 )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : F_Media::~F_Media
//	METHOD TYPE : Destructor
//
F_Media::~F_Media()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : F_Media::read
//	METHOD TYPE : PMError
//
PMError F_Media::read( istream & stream_r, const bool quick_r )
{
  reset();

  if ( ! stream_r ) {
    ERR << "Bad stream to read." << endl;
    return InstSrcError::E_open_file;
  }

  // vendor
  string line = stringutil::getline( stream_r, stringutil::TRIM );
  if ( line.empty() ) {
    ERR << "Parse error: missing 'vendor'" << endl;
    return InstSrcError::E_no_instsrc_on_media;
  } else {
    _vendor = Vendor( line );
  }

  // ident
  line = stringutil::getline( stream_r, stringutil::TRIM );
  if ( line.empty() ) {
    ERR << "Parse error: missing 'ident'" << endl;
    return InstSrcError::E_no_instsrc_on_media;
  } else {
    _ident = line;
  }

  // thats all in quick mode!
  if ( ! quick_r ) {

    // count
    line = stringutil::getline( stream_r, stringutil::TRIM );
    if ( line.empty() ) {
      ERR << "Parse error: missing 'count'" << endl;
      return InstSrcError::E_no_instsrc_on_media;
    } else {
      _count = atoi( line.c_str() );
      if ( !_count ) {
	WAR << "Parse: media count must not be '0', using '1'" << endl;
	_count = 1;
      }
    }

    //
    // bad :(
    // optional flags / or 1st medianame
    // fortunately there's currently just one flag
    bool skipread = false; // in case we get the 1st medianame here
    line = stringutil::getline( stream_r, stringutil::TRIM );
    if ( line == "doublesided" ) {
      _flags |= DOUBLESIDED;
    } else if ( line.substr( 0, 5 ) == "MEDIA" ) {
      skipread = true;
    } else if ( line.size() ) {
      WAR << "Parse: ignore unknown flags '" << line << "'" << endl;
    }

    // medianames
    while ( stream_r || skipread ) {
      if ( skipread ) {
	skipread = false;
      } else {
	line = stringutil::getline( stream_r, stringutil::TRIM );
      }
      if ( !(stream_r.fail() || stream_r.bad()) ) {
	string tag = stringutil::stripFirstWord( line );
	if ( tag.substr( 0, 5 ) == "MEDIA" && tag.find_first_of( "123456789", 5 ) == 5 ) {
	  // MEDIA{N}[.LANG]
	  unsigned num = atoi( tag.c_str() + 5 );
	  LangCode lang;
	  string::size_type dot = tag.rfind( '.' );
	  if ( dot != string::npos ) {
	    lang = LangCode( tag.substr( dot+1 ) );
	  }
	  _labels[num]._datamap[lang] = line;
	}
      }
    }
  } // ! quick_r

  if ( stream_r.bad() ) {
    ERR << "Error parsing stream" << endl;
    return InstSrcError::E_no_instsrc_on_media;
  }

  DBG << *this;
  return InstSrcError::E_ok;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : F_Media::read
//	METHOD TYPE : PMError
//
PMError F_Media::read( const Pathname & file_r, const bool quick_r )
{
  ifstream stream( file_r.asString().c_str() );
  PMError err = read( stream, quick_r );
  if ( err ) {
    ERR << err << " parsing " << file_r << endl;
  }
  return err;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const F_Media & obj )
{
  str << "VENDOR: " << obj.vendor() << endl;
  str << "IDENT:  " << obj.ident() << endl;
  str << "COUNT:  " << obj.count() << endl;
  str << "FLAGS:  " << (obj.doublesided()?"doublesided":"") << endl;
  for ( F_Media::LabelMap::const_iterator it = obj._labels.begin(); it != obj._labels.end(); ++it ) {
    str << "MEDIA" << it->first << endl;
    const PM::LocaleString::datamap & data( it->second._datamap );
    for ( PM::LocaleString::datamap::const_iterator el = data.begin(); el != data.end(); ++el ) {
      str << el->first << "\t'" << el->second << "'" << endl;
    }
  }
  return str;
}
