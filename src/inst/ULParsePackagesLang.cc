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

  File:       ULParsePackagesLang.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>
#include <y2util/stringutil.h>
#include <y2util/TagCacheRetrieval.h>
#include <y2util/PathInfo.h>

#include <y2pm/ULParsePackagesLang.h>

using namespace std;

///////////////////////////////////////////////////////////////////
#undef Y2LOG
#define Y2LOG "ULParse"
///////////////////////////////////////////////////////////////////

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : static ostream &
*/
static ostream & operator<<( ostream & str, const TagRetrievalPos & obj )
{
  str << '[' << obj.begin() << ',' << obj.end() << ']';
  if ( obj.empty() ) {
    str << "(empty)";
  } else {
    str << '(' << (obj.end()-obj.begin()) << ')';
  }
  return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : ULParsePackagesLang::ULParsePackagesLang
//	METHOD TYPE : Constructor
//
ULParsePackagesLang::ULParsePackagesLang( const Pathname & file_r )
    : _file( file_r )
{
  // Check an open file
  PathInfo file( _file );
  if ( ! file.isFile() ) {
    WAR << "No File: " << file << endl;
    return;
  }

  _stream.open( file.path().asString().c_str() );
  if ( ! _stream.is_open() ) {
    WAR << "Can't open" << file << endl;
    return;
  }

  // Find initial version tag
  TaggedParser::TagType type = _parser.lookupTag( _stream );
  if ( type != TaggedParser::SINGLE
       || _parser.currentTag() != "Ver"
       || ! _parser.currentLocale().empty() ) {
    WAR << "Initial '=Ver:' tag missing in " << file << endl;
    return;
  }

  _version = _parser.data();
  if ( _version != "2.0" ) {
    WAR << "Version '" << _version << "' != 2.0 in " << file << endl;
    return;
  }

  // initialize tagset
  _tagset.setAllowMultipleSets( true );	// multiple tagsets per file
  _tagset.setAllowUnknownTags( true );	// skip unknown tags

  // Using loop and switch to get a compiler warning, if tags are
  // defined but uninitialized, or vice versa.
  for ( Tags tag = Tags(0); tag < NUM_TAGS; tag = Tags(tag+1) ) {
    switch ( tag ) {
#define DEFTAG(T,ARGS) case T: _tagset.addTag ARGS; break

      DEFTAG( PACKAGE,		( "Pkg", tag,	TaggedFile::SINGLE, TaggedFile::START ) );

      DEFTAG( SUMMARY,		( "Sum", tag,	TaggedFile::SINGLE ) );
      DEFTAG( DESCRIPTION,	( "Des", tag,	TaggedFile::MULTI ) );
      DEFTAG( INSNOTIFY,	( "Ins", tag,   TaggedFile::MULTI ) );
      DEFTAG( DELNOTIFY,	( "Del", tag,   TaggedFile::MULTI ) );
      DEFTAG( LICENSETOCONFIRM,	( "Eul", tag,   TaggedFile::MULTI ) );

#undef DEFTAG
      // No default: let compiler warn missing enumeration values
    case NUM_TAGS: break;
    }
  }

  // Everything ok so far
  _retrieval = new TagCacheRetrieval( file.path() );
  MIL << _file << " ready to parse." << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : ULParsePackagesLang::~ULParsePackagesLang
//	METHOD TYPE : Destructor
//
ULParsePackagesLang::~ULParsePackagesLang()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : ULParsePackagesLang::getEntry
//	METHOD TYPE : TaggedFile::assignstatus
//
TaggedFile::assignstatus ULParsePackagesLang::getEntry( Entry & entry_r ) {

  entry_r = Entry();
  TaggedFile::assignstatus ret = _tagset.assignSet( _parser, _stream );

  if ( ret == TaggedFile::ACCEPTED_FULL ) {

    getData( entry_r );

  } else if ( ret == TaggedFile::REJECTED_EOF ) {

    MIL << _file << " at EOF" << endl;

  } else {

    ERR << _file << "(" << _parser.lineNumber() << "): error "
      << ret << ", last tag read: " << _parser.currentTag();
    if ( ! _parser.currentLocale().empty() )
      ERR << "." << _parser.currentLocale();
    ERR << endl;

  }

  return ret;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : ULParsePackagesLang::getData
//	METHOD TYPE : void
//
void ULParsePackagesLang::getData( Entry & entry_r )
{
  string ident( _tagset.getTagByIndex(PACKAGE)->Data() );
  if ( ident.empty() ) {
    ERR << _file << "(" << _parser.lineNumber() << ") No '=Pkg' value found" << endl;
    return;
  }

  vector<string> splitted;
  stringutil::split( ident, splitted, " ", false );
  if ( splitted.size() != 4 ) {
    ERR << _file << "(" << _parser.lineNumber() << ") Illegal '=Pkg' value '" << ident << "'" << endl;
    return;
  }

  entry_r = Entry( PkgIdent( PkgName( splitted[0] ),
			     PkgEdition( splitted[1], splitted[2] ),
			     PkgArch( splitted[3] ) ),
		   _retrieval );

  entry_r.posSUMMARY          = _tagset.getTagByIndex( SUMMARY )->Pos();
  entry_r.posDESCRIPTION      = _tagset.getTagByIndex( DESCRIPTION )->Pos();
  entry_r.posINSNOTIFY        = _tagset.getTagByIndex( INSNOTIFY )->Pos();
  entry_r.posDELNOTIFY        = _tagset.getTagByIndex( DELNOTIFY )->Pos();
  entry_r.posLICENSETOCONFIRM = _tagset.getTagByIndex( LICENSETOCONFIRM )->Pos();
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : std::ostream &
*/
ostream & operator<<( ostream & str, const ULParsePackagesLang::Entry & obj )
{
  str << obj.nameEdArch() << endl;
  str << "    SUM " << obj.posSUMMARY << endl;
  str << "    DES " << obj.posDESCRIPTION << endl;
  str << "    INY " << obj.posINSNOTIFY << endl;
  str << "    DNY " << obj.posDELNOTIFY << endl;
  str << "    EUL " << obj.posLICENSETOCONFIRM;

  return str;
}

/******************************************************************
**
**
**	FUNCTION NAME : operator<<
**	FUNCTION TYPE : ostream &
*/
ostream & operator<<( ostream & str, const ULParsePackagesLang & obj )
{
  str << obj._file << " (" << obj._retrieval << ")";
  return str;
}

