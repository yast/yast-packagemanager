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

   File:       InstSrcDescr.cc

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

   Purpose:	Installation source description
		Keeps all information to identify and describe an installation
		source, but does not have knowledge about its contents (i.e the
		packages, selectins, etc ). See InstSrcData for the contents.

/-*/

#include <cstdlib> // for atoi

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>
#include <y2util/ExternalProgram.h>
#include <y2util/TaggedFile.h>

#include <y2pm/InstTarget.h>
#include <y2pm/InstSrcDescr.h>

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDescr
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(InstSrcDescr);

///////////////////////////////////////////////////////////////////

const Pathname InstSrcDescr::_cache_file( "description" );

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::InstSrcDescr
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
InstSrcDescr::InstSrcDescr()
    : _type               ( InstSrc::T_UNKNOWN )
    , _default_activate   ( true )
    , _default_refresh    ( true )
    , _default_rank       ( NO_RANK )
    , _usefordeltas	  ( true )
    , _media_count        ( 0 )
    , _media_doublesided  ( false )
    , _content_distproduct( PkgName(), PkgEdition() )
    , _content_requires   ( PkgName(), NONE, PkgEdition() )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::~InstSrcDescr
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcDescr::~InstSrcDescr()
{
}

///////////////////////////////////////////////////////////////////
//
//	METHOD NAME : InstSrcDescr::content_relnotesurl
//	METHOD TYPE : const std::string
//
const std::string InstSrcDescr::content_relnotesurl() const
{
  PkgArch tarch( InstTarget::baseArch() );
  string  relnotes( _content_relnotesurl );

  for ( string::size_type pos = relnotes.find( "%a" );
	pos != string::npos; pos = relnotes.find( "%a", pos ) ) {
    relnotes.replace( pos, 2, tarch.asString() );
  }
  return relnotes;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::label
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string InstSrcDescr::label() const
{
  PM::LocaleOrder langs( Y2PM::getLocaleFallback() );

  for ( PM::LocaleOrder::const_iterator lang = langs.begin(); lang != langs.end(); ++lang ) {
    LabelMap::const_iterator found = _content_labelmap.find( *lang );
    if ( found != _content_labelmap.end() ) {
      // gotcha
      return found->second;
    }
  }
  // nothing appropriate found: return default label
  return content_label();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::hasFlag
//	METHOD TYPE : bool
//
bool InstSrcDescr::hasFlag( const string & flag_r ) const
{
  vector<string> words;
  stringutil::split( content_flags(), words );
  return( find( words.begin(), words.end(), flag_r ) != words.end() );
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::sameContentProduct
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool InstSrcDescr::sameContentProduct( const constInstSrcDescrPtr & rhs,
				       const constInstSrcDescrPtr & lhs )
{
  if ( rhs == lhs )
    return true;
  if ( ! ( rhs && lhs ) )
    return false;
  return rhs->content_product().sameProduct( lhs->content_product() );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrcDescr::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str ) << "(";
  str << _type << ":" << _content_product;
  str << " from " << _url << "(" << _product_dir << ")";

  return str << ")";
}

///////////////////////////////////////////////////////////////////
//
// Cache writing / reading
//
///////////////////////////////////////////////////////////////////

static const std::string TypeTag	= "Type";
static const std::string UrlTag		= "URL";
static const std::string ProdDirTag	= "ProductDir";
static const std::string DefActTag	= "Default_activate";
static const std::string DefRefreshTag	= "Default_refresh";
static const std::string DefRankTag	= "Default_rank";
static const std::string UseForDeltasTag= "UseForDeltas";
static const std::string MediaTag	= "Media";
static const std::string ProductTag	= "Product";
static const std::string ArchTag	= "Arch";
static const std::string RequiresTag	= "Requires";
static const std::string DefBaseTag	= "DefaultBase";
static const std::string LabelMapTag	= "LabelMap";
static const std::string LinguasTag	= "Linguas";
static const std::string LabelTag	= "Label";
static const std::string LangTag	= "Language";
static const std::string TimeTag	= "Timezone";
static const std::string DescrDirTag	= "DescriptionDir";
static const std::string DataDirTag	= "DataDir";
static const std::string FlagsTag	= "Flags";
static const std::string RelnotesUrlTag	= "RelnotesUrl";
static const std::string YouUrlTag	= "YouUrl";
static const std::string YouTypeTag	= "YouType";
static const std::string YouPathTag	= "YouPath";

static const std::string FlagMediaDoublesided( "doublesided" );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::writeStream
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcDescr::writeStream( std::ostream & str ) const
{
  str << "=" << TypeTag << ": " << InstSrc::toString(_type) << endl;
  str << "=" << UrlTag << ": " << _url.asString (true, true, true) << endl;
  str << "=" << ProdDirTag << ": " << _product_dir << endl;
  str << "=" << DefActTag << ": " << (_default_activate?"1":"0") << endl;
  str << "=" << DefRefreshTag << ": " << (_default_refresh?"1":"0") << endl;
  str << "=" << DefRankTag << ": " << _default_rank << endl;
  str << "=" << UseForDeltasTag << ": " << (_usefordeltas?"1":"0") << endl;
  // data from media file
#warning let F_Media read/write data
  str << "+" << MediaTag << ":" << endl;
  str << _media_vendor << endl;
  str << _media_id << endl;
  str << _media_count << endl;
  if ( _media_doublesided ) {
    str << FlagMediaDoublesided << endl;
  }
  for ( F_Media::LabelMap::const_iterator it = _media_labels.begin(); it != _media_labels.end(); ++it ) {
    const PM::LocaleString::datamap & data( it->second._datamap );
    for ( PM::LocaleString::datamap::const_iterator el = data.begin(); el != data.end(); ++el ) {
      str << "MEDIA" << it->first;
      if ( ! el->first.isSet() ) {
	str << '.' << el->first;
      }
      str << ' ' << el->second << endl;
    }
  }
  str << "-" << MediaTag << ":" << endl;

  // product data from content file
  str << "+" << ProductTag << ":" << endl;
  str << ProductIdent::toString(_content_product) << endl;
  str << PkgNameEd::toString(_content_distproduct) << endl;
  str << ProductIdent::toString(_content_baseproduct) << endl;
  str << _content_vendor << endl;
  str << "-" << ProductTag << ":" << endl;

  str << "=" << DefBaseTag << ":" << _content_defaultbase << endl;

  // content file archmap
  str << "+" << ArchTag << ":" << endl;
  ArchMap::const_iterator arch_pos;
  std::list<PkgArch>::iterator pos;

  for ( arch_pos = _content_archmap.begin(); arch_pos != _content_archmap.end(); ++arch_pos )
  {
    str << (*arch_pos).first << " ";
    std::list<PkgArch> archs =  (*arch_pos).second;

    for ( pos = archs.begin(); pos != archs.end(); ++pos  )
    {
      str  << (*pos) << " ";
    }
    str << endl;
  }
  str << "-" << ArchTag << ":" << endl;

  // content requires
  str << "=" << RequiresTag << ":" << PkgRelation::toString(_content_requires) << endl;

  // content file language
  str << "=" << LangTag << ": " << _content_language << endl;

  // content file linguas list
  str << "+" << LinguasTag << ":" << endl;
  std::list<LangCode>::const_iterator ling_pos;

  for ( ling_pos = _content_linguas.begin(); ling_pos !=  _content_linguas.end(); ++ling_pos )
  {
    str << (*ling_pos) << endl;
  }
  str << "-" << LinguasTag << ":" << endl;

  // content label
  str << "=" << LabelTag << ":" << _content_label << endl;

  // content file labelmap
  str << "+" << LabelMapTag << ": " << endl;
  LabelMap::const_iterator label_pos;

  for ( label_pos = _content_labelmap.begin(); label_pos != _content_labelmap.end(); ++label_pos )
  {
    str << (*label_pos).first << " " << (*label_pos).second << endl;
  }
  str << "-" << LabelMapTag << ":" << endl;

  // timezone
  str << "=" << TimeTag << ": " << _content_timezone << endl;

  // descrdir
  str << "=" << DescrDirTag << ": " << _content_descrdir << endl;

  // datadir
  str << "=" << DataDirTag << ": " << _content_datadir << endl;

  // flags
  str << "=" << FlagsTag << ": " << _content_flags << endl;

  // relnotesurl
  str << "=" << RelnotesUrlTag << ": " << _content_relnotesurl << endl;

  // youurl
  str << "=" << YouUrlTag << ": " << _content_youurl << endl;

  // youtype
  str << "=" << YouTypeTag << ": " << _content_youtype << endl;

  // youpath
  str << "=" << YouPathTag << ": " << _content_youpath << endl;

  return ( str.good() ? Error::E_ok : Error::E_error );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::writeCache
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcDescr::writeCache( const Pathname & cache_dir_r ) const
{
    Pathname destFileName = cache_dir_r + _cache_file;
    Pathname fileName     = destFileName.extend( ".new" );

    ofstream file( fileName.asString().c_str() );
    if ( !file ) {
      ERR << *this << " writeCache " << fileName << " " << Error::E_create_file << endl;
      return Error::E_create_file;
    }

    writeStream( file );

    if ( !file ) {
      ERR << *this << " writeCache " << fileName << " " << Error::E_write_file << endl;
      PathInfo::unlink( fileName );
      return Error::E_write_file;
    }

    int res = PathInfo::rename( fileName, destFileName );
    if ( res ) {
      ERR << *this << " writeCache rename " << fileName << " failed (errno " << res << ")" << endl;
      PathInfo::unlink( fileName );
      return Error::E_write_file;
    }

    MIL << *this << " wrote cache " << cache_dir_r << endl;
    return Error::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::readStream
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcDescr::readStream( InstSrcDescrPtr & ndescr_r, std::istream & descrstream )
{
  ndescr_r    = 0;

  if ( !descrstream ) {
    return Error::E_open_file;
  }

  InstSrcDescrPtr ndescr( new InstSrcDescr );

  ///////////////////////////////////////////////////////////////////
  // parse stream and fill into ndescr
  ///////////////////////////////////////////////////////////////////

    TaggedFile::TagSet tagset;
    // initialize tagset

    tagset.setAllowMultipleSets (false);	// multiple tagsets per file
    tagset.setAllowUnknownTags (true);		// skip unknown tags

    enum Tags { TYPE, URL, PRODUCTDIR,
	        ACTIVATE, REFRESH, RANK,
		FORDELTAS,
	        MEDIA, PRODUCT, DEFBASE, ARCH,
		REQUIRES, LANGUAGE, LABEL, LABELMAP, LINGUAS, TIMEZONE, DESCRDIR, DATADIR,
		FLAGS, RELNOTESURL,
		YOUURL, YOUTYPE, YOUPATH };

    // for the 'description' file

    tagset.addTag (TypeTag,		TYPE,	    TaggedFile::SINGLE, TaggedFile::START);
    tagset.addTag (UrlTag,		URL,	    TaggedFile::SINGLE);
    tagset.addTag (ProdDirTag,  	PRODUCTDIR, TaggedFile::SINGLE);	// product dir below _url
    tagset.addTag (DefActTag,		ACTIVATE,   TaggedFile::SINGLE);	// 1 = true (default activated), 0 = false
    tagset.addTag (DefRefreshTag,	REFRESH,    TaggedFile::SINGLE);	// 1 = true (default refresh), 0 = false
    tagset.addTag (DefRankTag,		RANK,       TaggedFile::SINGLE);	// RankValue, if provided
    tagset.addTag (UseForDeltasTag,	FORDELTAS,  TaggedFile::SINGLE);	// 1 = true, 0 = false
    tagset.addTag (MediaTag,		MEDIA,	    TaggedFile::MULTI);		// _media_vendor, _media_id, _media_count
    tagset.addTag (ProductTag,		PRODUCT,    TaggedFile::MULTI);		// _content_product, _content_distproduct, _content_baseproduct, _content_vendor
    tagset.addTag (DefBaseTag,		DEFBASE,    TaggedFile::SINGLE);	// _content_defaultbase
    tagset.addTag (ArchTag,		ARCH,	    TaggedFile::MULTI);		// _content_archmap
    tagset.addTag (RequiresTag, 	REQUIRES,   TaggedFile::SINGLE);	// _content_requires
    tagset.addTag (LangTag,		LANGUAGE,   TaggedFile::SINGLE);	// _content_language
    tagset.addTag (LabelTag,		LABEL,	    TaggedFile::SINGLE);	// _content_label
    tagset.addTag (LabelMapTag,		LABELMAP,   TaggedFile::MULTI);	        // _content_labelmap
    tagset.addTag (LinguasTag,		LINGUAS,    TaggedFile::MULTI);		// _content_linguas
    tagset.addTag (TimeTag,		TIMEZONE,   TaggedFile::SINGLE);	// _content_timezone
    tagset.addTag (DescrDirTag, 	DESCRDIR,   TaggedFile::SINGLE);	// _content_descrdir
    tagset.addTag (DataDirTag,		DATADIR,    TaggedFile::SINGLE);	// _content_datadir
    tagset.addTag (FlagsTag,		FLAGS,      TaggedFile::SINGLE);	// _content_flags
    tagset.addTag (RelnotesUrlTag,	RELNOTESURL,TaggedFile::SINGLE);	// _content_relnotesurl
    tagset.addTag (YouUrlTag,		YOUURL,     TaggedFile::SINGLE);	// _content_youurl
    tagset.addTag (YouTypeTag,		YOUTYPE,    TaggedFile::SINGLE);	// _content_youtype
    tagset.addTag (YouPathTag,		YOUPATH,    TaggedFile::SINGLE);	// _content_youpath

    TaggedParser parser;

    TaggedFile::assignstatus status = tagset.assignSet (parser, descrstream);

    if (status != TaggedFile::ACCEPTED_FULL)
    {
	ERR <<"Line " << parser.lineNumber() << endl;
	ERR << "Status " << (int)status << ", Last tag read: " << parser.currentTag();
	if (!parser.currentLocale().empty()) ERR << "." << parser.currentLocale();
	ERR << endl;
        return Error::E_bad_cache_descr;
    }

#define GET_TAG(tagname) tagset.getTagByIndex (tagname)
#define GET_STRING(tagname) tagset.getTagByIndex (tagname)->Data()
#define GET_POS(tagname) tagset.getTagByIndex (tagname)->Pos()
    string tmpval;

    ndescr->set_default_activate (GET_STRING(ACTIVATE) == "1");
    if ( (tmpval = GET_STRING(REFRESH)).size() ) // else use constructor default
      ndescr->set_default_refresh (tmpval == "1");
    ndescr->set_type ( InstSrc::fromString (GET_STRING(TYPE)));
    ndescr->set_usefordeltas (GET_STRING(FORDELTAS) == "1");

    tmpval = GET_STRING(RANK);
    if ( tmpval.size() ) {
      ndescr->set_default_rank( atoi( tmpval.c_str() ) );
    }

    std::list<std::string> multi;
    GET_POS(MEDIA).retrieveData (descrstream, multi);
    if ( multi.size() >= 3 )
    {
#warning let F_Media read/write data
	std::list<std::string>::iterator multi_pos = multi.begin();
	// only check if ( !(*multi_pos).empty() ) if an empty string is an error
	ndescr->set_media_vendor( Vendor(*multi_pos) );
	++multi_pos;
	ndescr->set_media_id(*multi_pos);
	++multi_pos;
	ndescr->set_media_count( atoi(multi_pos->c_str()) );

	if ( multi.size() > 3 ) {
	  ++multi_pos;
	  ndescr->set_media_doublesided( *multi_pos == FlagMediaDoublesided );
	  bool skipread = ( (*multi_pos).substr( 0, 5 ) == "MEDIA" ); // in case we got the 1st medianame here
  F_Media::LabelMap media_labels;
  while ( true ) {
    if ( skipread ) {
      skipread = false;
    } else {
      ++multi_pos;
      if (  multi_pos == multi.end() )
	break;
    }
    string line( *multi_pos );
    string tag = stringutil::stripFirstWord( line );
    if ( tag.substr( 0, 5 ) == "MEDIA" && tag.find_first_of( "123456789", 5 ) == 5 ) {
      // MEDIA{N}[.LANG]
      unsigned num = atoi( tag.c_str() + 5 );
      LangCode lang;
      string::size_type dot = tag.rfind( '.' );
      if ( dot != string::npos ) {
	lang = LangCode( tag.substr( dot+1 ) );
      }
      media_labels[num]._datamap[lang] = line;
    }
  }
  ndescr->set_media_labels( media_labels );
	}
    }

    ndescr->set_url( Url (GET_STRING(URL) ));
    ndescr->set_product_dir( Pathname (GET_STRING(PRODUCTDIR)));

    GET_POS(PRODUCT).retrieveData (descrstream, multi);
    if ( multi.size() >= 4 )					// includes distproduct
    {
	std::list<std::string>::iterator multi_pos = multi.begin();
	//  only check if ( !(*multi_pos).empty() ) if an empty string is an error
	ndescr->set_content_product( PkgNameEd::fromString(*multi_pos) );
	++multi_pos;
	ndescr->set_content_distproduct( PkgNameEd::fromString(*multi_pos));
	++multi_pos;
	ndescr->set_content_baseproduct( PkgNameEd::fromString(*multi_pos));
	++multi_pos;
	ndescr->set_content_vendor( Vendor(*multi_pos) );
    }
    else if ( multi.size() == 3 )				// without distproduct
    {
	std::list<std::string>::iterator multi_pos = multi.begin();
	//  only check if ( !(*multi_pos).empty() ) if an empty string is an error
	ndescr->set_content_product( PkgNameEd::fromString(*multi_pos) );
	++multi_pos;
	ndescr->set_content_baseproduct( PkgNameEd::fromString(*multi_pos));
	++multi_pos;
	ndescr->set_content_vendor( Vendor(*multi_pos) );
    }

    ndescr->set_content_defaultbase(GET_STRING(DEFBASE));

    GET_POS(ARCH).retrieveData (descrstream, multi);
    InstSrcDescr::ArchMap arch;
    for (std::list<std::string>::iterator multi_pos = multi.begin(); multi_pos != multi.end(); ++multi_pos )
    {
	vector<std::string> line = TaggedParser::split2words (*multi_pos, " ");
	if ( line.size() ) {
	  list<PkgArch> archs;
	  for ( unsigned int i = 1; i < line.size(); i++ )
	  {
	    archs.push_back( PkgArch(line[i]) );
	  }
	  arch[line[0]] = archs;
	}
    }
    ndescr->set_content_archmap( arch );

    ndescr->set_content_requires( PkgRelation::fromString( GET_STRING(REQUIRES) ) );
    ndescr->set_content_label(GET_STRING(LABEL));

    GET_POS(LABELMAP).retrieveData (descrstream, multi);
    InstSrcDescr::LabelMap label;
    for (std::list<std::string>::iterator multi_pos = multi.begin(); multi_pos != multi.end(); ++multi_pos )
    {
	// split at first blank or tab, *multi_pos contains "<locale><blank><label_with_blanks>"
	string::size_type spcpos = (*multi_pos).find_first_of (" \t");
	if (spcpos != (*multi_pos).npos)
	{
	    label[LangCode ((*multi_pos).substr (0, spcpos))] = (*multi_pos).substr(spcpos+1);
	}
    }
    ndescr->set_content_labelmap( label );

    GET_POS(LINGUAS).retrieveData (descrstream, multi);
    InstSrcDescr::LinguasList linguas;
    for (std::list<std::string>::iterator multi_pos = multi.begin(); multi_pos != multi.end(); ++multi_pos )
    {
	linguas.push_back( LangCode(*multi_pos) );
    }
    ndescr->set_content_linguas( linguas );

    ndescr->set_content_timezone(GET_STRING (TIMEZONE));
    ndescr->set_content_descrdir( Pathname(GET_STRING (DESCRDIR)));
    ndescr->set_content_datadir( Pathname(GET_STRING (DATADIR)));
    ndescr->set_content_flags( GET_STRING (FLAGS));
    ndescr->set_content_relnotesurl( GET_STRING (RELNOTESURL));
    ndescr->set_content_youurl( GET_STRING (YOUURL));
    ndescr->set_content_youtype( GET_STRING (YOUTYPE));
    ndescr->set_content_youpath( GET_STRING (YOUPATH));

    ndescr_r = ndescr;

    return PMError::E_ok;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::readCache
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcDescr::readCache( InstSrcDescrPtr & ndescr_r, const Pathname & cache_dir_r )
{
    ndescr_r = 0;

    ///////////////////////////////////////////////////////////////////
    // parse _cache_file and fill into ndescr
    ///////////////////////////////////////////////////////////////////

    Pathname fileName = cache_dir_r + _cache_file;

    std::ifstream mediaCacheStream( fileName.asString().c_str() );

    if( !mediaCacheStream ) {
      ERR << "Can't open file " << fileName << endl;
      return Error::E_open_file;
    }

    PMError err = readStream( ndescr_r, mediaCacheStream );

    if ( err ) {
      ERR << "Error parsing file " << fileName << " " << err << endl;
      ndescr_r = 0;
      return Error::E_bad_cache_descr;
    }

    return err;
}
