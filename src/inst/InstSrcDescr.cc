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
    , _default_rank       ( NO_RANK )
    , _content_product    ( PkgName(), PkgEdition() )
    , _content_baseproduct( PkgName(), PkgEdition() )
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
//
//	METHOD NAME : InstSrcDescr::label
//	METHOD TYPE : std::string
//
//	DESCRIPTION :
//
std::string InstSrcDescr::label() const
{
  LabelMap::const_iterator found = _content_labelmap.find( Y2PM::getPreferredLocale() );
  if ( found == _content_labelmap.end() && Y2PM::getPreferredLocale().hasCountry() )
    found = _content_labelmap.find( Y2PM::getPreferredLocale().languageOnly() );

  if ( found != _content_labelmap.end() )
    return found->second;

  return content_label();
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
  return( rhs->content_product().edition == lhs->content_product().edition );
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

static const std::string TypeTag      = "Type";
static const std::string UrlTag       = "URL";
static const std::string ProdDirTag   = "ProductDir";
static const std::string DefActTag    = "Default_activate";
static const std::string DefRankTag   = "Default_rank";
static const std::string MediaTag     = "Media";
static const std::string ContentTag   = "Product";
static const std::string ArchTag      = "Arch";
static const std::string RequiresTag  = "Requires";
static const std::string DefBaseTag   = "DefaultBase";
static const std::string LabelMapTag  = "LabelMap";
static const std::string LinguasTag   = "Linguas";
static const std::string LabelTag     = "Label";
static const std::string LangTag      = "Language";
static const std::string TimeTag      = "Timezone";
static const std::string DescrDirTag  = "DescriptionDir";
static const std::string DataDirTag   = "DataDir";
static const std::string YouUrlTag    = "YouUrl";
static const std::string YouTypeTag   = "YouType";
static const std::string YouPathTag   = "YouPath";

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
  str << "=" << UrlTag << ": " << _url << endl;
  str << "=" << ProdDirTag << ": " << _product_dir << endl;
  str << "=" << DefActTag << ": " << (_default_activate?"1":"0") << endl;
  str << "=" << DefRankTag << ": " << _default_rank << endl;
  // data from media file
  str << "+" << MediaTag << ":" << endl;
  str << _media_vendor << endl;
  str << _media_id << endl;
  str << _media_count << endl;
  str << "-" << MediaTag << ":" << endl;

  // product data from content file
  str << "+" << ContentTag << ":" << endl;
  str << PkgNameEd::toString(_content_product) << endl;
  str << PkgNameEd::toString(_content_baseproduct) << endl;
  str << _content_vendor << endl;
  str << "-" << ContentTag << ":" << endl;

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
  str << "=" << LinguasTag << ":" << endl;
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
	        ACTIVATE, RANK,
	        MEDIA, PRODUCT, DEFBASE, ARCH,
		REQUIRES, LANGUAGE, LABEL, LABELMAP, LINGUAS, TIMEZONE, DESCRDIR, DATADIR,
		YOUURL, YOUTYPE, YOUPATH };

    // for the 'packages' file

    tagset.addTag (TypeTag,	TYPE,	    TaggedFile::SINGLE, TaggedFile::START);
    tagset.addTag (UrlTag,	URL,	    TaggedFile::SINGLE);
    tagset.addTag (ProdDirTag,  PRODUCTDIR, TaggedFile::SINGLE);	// product dir below _url
    tagset.addTag (DefActTag,	ACTIVATE,   TaggedFile::SINGLE);	// 1 = true (default activated), 0 = false
    tagset.addTag (DefRankTag,	RANK,       TaggedFile::SINGLE);	// RankValue, if provided
    tagset.addTag (MediaTag,	MEDIA,	    TaggedFile::MULTI);		// _media_vendor, _media_id, _media_count
    tagset.addTag (ContentTag,	PRODUCT,    TaggedFile::MULTI);		// _content_product, _content_baseproduct, _content_vendor
    tagset.addTag (DefBaseTag,	DEFBASE,    TaggedFile::SINGLE);	// _content_defaultbase
    tagset.addTag (ArchTag,	ARCH,	    TaggedFile::MULTI);		// _content_archmap
    tagset.addTag (RequiresTag, REQUIRES,   TaggedFile::SINGLE);	// _content_requires
    tagset.addTag (LangTag,	LANGUAGE,   TaggedFile::SINGLE);	// _content_language
    tagset.addTag (LabelTag,	LABEL,	    TaggedFile::SINGLE);	// _content_label
    tagset.addTag (LabelMapTag,	LABELMAP,   TaggedFile::MULTI);	        // _content_labelmap
    tagset.addTag (LinguasTag,	LINGUAS,    TaggedFile::MULTI);		// _content_linguas
    tagset.addTag (TimeTag,	TIMEZONE,   TaggedFile::SINGLE);	// _content_timezone
    tagset.addTag (DescrDirTag, DESCRDIR,   TaggedFile::SINGLE);	// _content_descrdir
    tagset.addTag (DataDirTag,	DATADIR,    TaggedFile::SINGLE);	// _content_datadir
    tagset.addTag (YouUrlTag,	YOUURL,     TaggedFile::SINGLE);	// _content_youurl
    tagset.addTag (YouTypeTag,	YOUTYPE,    TaggedFile::SINGLE);	// _content_youtype
    tagset.addTag (YouPathTag,	YOUPATH,    TaggedFile::SINGLE);	// _content_youpath

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

    ndescr->set_default_activate (GET_STRING(ACTIVATE) == "1");
    ndescr->set_type ( InstSrc::fromString (GET_STRING(TYPE)));

    string val = GET_STRING(RANK);
    if ( val.size() ) {
      ndescr->set_default_rank( atoi( val.c_str() ) );
    }

    std::list<std::string> multi;
    GET_POS(MEDIA).retrieveData (descrstream, multi);
    if ( multi.size() >= 3 )
    {
	std::list<std::string>::iterator multi_pos = multi.begin();
	//  only check if ( !(*multi_pos).empty() ) if an empty string is an error
	ndescr->set_media_vendor( Vendor(*multi_pos++) );
	ndescr->set_media_id( *multi_pos++ );
	ndescr->set_media_count( atoi(multi_pos->c_str()) );
    }

    ndescr->set_url( Url (GET_STRING(URL) ));
    ndescr->set_product_dir( Pathname (GET_STRING(PRODUCTDIR)));

    GET_POS(PRODUCT).retrieveData (descrstream, multi);
    if ( multi.size() >= 3 )
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
	vector<std::string> line = TaggedParser::split2words (*multi_pos, " ");
	if ( line.size() >= 2 )
	{
	    label[ LangCode(line[0])] = line[1];
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
