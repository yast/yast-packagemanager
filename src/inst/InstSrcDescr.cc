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

#include <y2pm/InstSrcDescr.h>
#include <y2util/CommonPkdParser.h>

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
    , _default_activate   ( false )
    , _content_product    ( PkgName(), PkgEdition() )
    , _content_baseproduct( PkgName(), PkgEdition() )
    , _content_requires   ( PkgName(), NONE, PkgEdition() )
{
#warning must init _base_arch (from hwinfo?)
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
  str << " type: "        << _type;
  str << " url: "         << _url;
  str << " product dir: " << _product_dir;
  str << " product: "     << _content_product;
  str << " vendor: "      << _content_vendor;

  return str << ")";
}

///////////////////////////////////////////////////////////////////
//
// Cache writing / reading
//
///////////////////////////////////////////////////////////////////

static const std::string TypeTag      = "=Type";
static const std::string UrlTag       = "=URL";
static const std::string ProdDirTag   = "=ProductDir";
static const std::string DefActTag    = "=Default_activate";
static const std::string MediaBTag    = "+Media";
static const std::string MediaETag    = "-Media";
static const std::string ContentBTag  = "+Product";
static const std::string ContentETag  = "-Product";
static const std::string ArchBTag     = "+Arch";
static const std::string ArchETag     = "-Arch";
static const std::string RequiresTag  = "=Requires";
static const std::string DefBaseTag   = "=DefaultBase";
static const std::string LabelMapBTag = "+LabelMap";
static const std::string LabelMapETag = "-LabelMap";
static const std::string LinguasBTag  = "+Linguas";
static const std::string LinguasETag  = "-Linguas";
static const std::string LabelTag     = "=Label";
static const std::string LangTag      = "=Language";
static const std::string TimeTag      = "=Timezone";
static const std::string DescrDirTag  = "=DescriptionDir";
static const std::string DataDirTag   = "=DataDir";

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : DescrTagSet
/**
 * @short provides the tag set for the media file
 * (to feed the tag parser)
 *
 **/
class DescrTagSet : public CommonPkdParser::TagSet
{

public:
    enum Tags {
	TYPE,		// type found on medium
	URL,		// URL
	PRODUCTDIR,	// product dir below _url
	ACTIVATE,	// 1 = true (default activated), 0 = false
	MEDIA,		// _media_vendor, _media_id, _media_count
	PRODUCT, 	// _content_product, _content_baseproduct, _content_vendor
	DEFBASE,	// _content_defaultbase
	ARCH,		// _content_archmap
	REQUIRES,       // _content_requires
	LANGUAGE,	// _content_language
	LABEL,		// _content_label
	LABELMAP,	// _content_labelmap
	LINGUAS,	// _content_linguas
	TIMEZONE,	// _content_timezone
	DESCRDIR,	// _content_descrdir
	DATADIR,	// _content_datadir
	NUM_TAGS
    };

public:
    DescrTagSet( )
	: TagSet()	{

	CommonPkdParser::Tag* t;
	createTag( TypeTag, TYPE );
	createTag( UrlTag, URL );
	createTag( ProdDirTag, PRODUCTDIR );
	createTag( DefActTag, ACTIVATE );
	t = createTag( MediaBTag, MEDIA );
	t->setEndTag( MediaETag );
	t = createTag( ContentBTag, PRODUCT );
	t->setEndTag( ContentETag );
	t = createTag( ArchBTag, ARCH );
	t->setEndTag( ArchETag );
	createTag( RequiresTag, REQUIRES );
	createTag( DefBaseTag, DEFBASE );
	t = createTag( LabelMapBTag, LABELMAP);
	t->setEndTag( LabelMapETag );
	t = createTag( LinguasBTag, LINGUAS );
	t->setEndTag( LinguasETag );
	createTag( LabelTag, LABEL );
	createTag( LangTag, LANGUAGE );
	createTag( TimeTag, TIMEZONE );
	createTag( DescrDirTag, DESCRDIR );
	createTag( DataDirTag, DATADIR );
    };

private:

    CommonPkdParser::Tag* createTag( std::string tagName, Tags tagEnum ) {

	CommonPkdParser::Tag* t;
	t = new CommonPkdParser::Tag( tagName, CommonPkdParser::Tag::ACCEPTONCE );
	this->addTag(t);
	addTagByIndex( tagEnum, t );

	return t;
    }
};
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::writeStream
//	METHOD TYPE : std::ostream & str
//
//	DESCRIPTION :
//
std::ostream & InstSrcDescr::writeStream( std::ostream & str ) const
{
  str << TypeTag << ": " << InstSrc::toString(_type) << endl;
  str << UrlTag << ": " << _url << endl;
  str << ProdDirTag << ": " << _product_dir << endl;
  str << DefActTag << ": " << (_default_activate?"1":"0") << endl;
  // data from media file
  str << MediaBTag << ":" << endl;
  str << _media_vendor << endl;
  str << _media_id << endl;
  str << _media_count << endl;
  str << MediaETag << ":" << endl;

  // product data from content file
  str << ContentBTag << ":" << endl;
  str << PkgNameEd::toString(_content_product) << endl;
  str << PkgNameEd::toString(_content_baseproduct) << endl;
  str << _content_vendor << endl;
  str << ContentETag << ":" << endl;

  str << DefBaseTag << ":" << _content_defaultbase << endl;

  // content file archmap
  str << ArchBTag << ":" << endl;
  ArchMap::const_iterator arch_pos;
  std::list<Pathname>::iterator pos;

  for ( arch_pos = _content_archmap.begin(); arch_pos != _content_archmap.end(); ++arch_pos )
  {
    str << (*arch_pos).first << " ";
    std::list<Pathname> paths =  (*arch_pos).second;

    for ( pos = paths.begin(); pos != paths.end(); ++pos  )
    {
      str  << (*pos) << " ";
    }
    str << endl;
  }
  str << ArchETag << ":" << endl;

  // content requires
  str << RequiresTag << ":" << PkgRelation::toString(_content_requires) << endl;

  // content file language
  str << LangTag << ": " << _content_language << endl;

  // content file linguas list
  str << LinguasBTag << ":" << endl;
  std::list<LangCode>::const_iterator ling_pos;

  for ( ling_pos = _content_linguas.begin(); ling_pos !=  _content_linguas.end(); ++ling_pos )
  {
    str << (*ling_pos) << endl;
  }
  str << LinguasETag << ":" << endl;

  // content label
  str << LabelTag << ":" << _content_label << endl;

  // content file labelmap
  str << LabelMapBTag << ": " << endl;
  LabelMap::const_iterator label_pos;

  for ( label_pos = _content_labelmap.begin(); label_pos != _content_labelmap.end(); ++label_pos )
  {
    str << (*label_pos).first << " " << (*label_pos).second << endl;
  }
  str << LabelMapETag << ":" << endl;

  // timezone
  str << TimeTag << ": " << _content_timezone << endl;

  // descrdir
  str << DescrDirTag << ": " << _content_descrdir << endl;

  // datadir
  str << DataDirTag << ": " << _content_datadir << endl;

  return str;
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
#warning TBD writeCache: protect existing cache against write errors

    Pathname fileName = cache_dir_r + _cache_file;

    ofstream file( fileName.asString().c_str() );
    if ( !file ) {
      ERR << *this << " writeCache " << fileName << " " << Error::E_create_file << endl;
      return Error::E_create_file;
    }

    writeStream( file );

    if ( !file ) {
      ERR << *this << " writeCache " << fileName << " " << Error::E_write_file << endl;
      return Error::E_write_file;
    }

    MIL << *this << " wrote cache " << cache_dir_r << endl;
    return Error::E_ok;
}

/******************************************************************
**
**
**	FUNCTION NAME : fillInstSrcDescr
**	FUNCTION TYPE : static bool
**
**	DESCRIPTION :
*/
static bool fillInstSrcDescr( InstSrcDescrPtr & ndescr, CommonPkdParser::TagSet * tagset )
{
    bool ok = true;
    CommonPkdParser::Tag* t = 0;

    if ( !tagset )
	return false;

    // TODO: set ok=false if the result string is empty (are all values required ???)

    // default activate
    t = tagset->getTagByIndex(DescrTagSet::ACTIVATE);
    if ( t )
    {
	if (  t->Data() == "1" )
	{
	    ndescr->set_default_activate( true );
	}
	else
	{
	    ndescr->set_default_activate( false );
	}
    }
    else
    {
	ok = false;
    }

    // type
    t = tagset->getTagByIndex(DescrTagSet::TYPE);
    if ( t )
    {
	InstSrc::Type type = InstSrc::fromString( t->Data() );
	ndescr->set_type( type );
    }
    else
    {
	ok = false;
    }

    // media data
    t = tagset->getTagByIndex(DescrTagSet::MEDIA);
    if ( t )
    {
	std::list<std::string> multi = t->MultiData();

	if ( multi.size() >= 3 )
	{
	    std::list<std::string>::iterator multi_pos = multi.begin();
	    //  only check if ( !(*multi_pos).empty() ) if an empty string is an error
	    ndescr->set_media_vendor( *multi_pos++ );
	    ndescr->set_media_id( *multi_pos++ );
	    ndescr->set_media_count( atoi(multi_pos->c_str()) );
	    multi_pos++;
	}
	else
	{
	    ok = false;
	}
    }
    else
    {
	ok = false;
    }

    // URL
    t = tagset->getTagByIndex(DescrTagSet::URL);
    if ( t )
    {
	Url url( t->Data() );
	ndescr->set_url( url );
    }
    else
    {
	ok = false;
    }

    // product dir
    t = tagset->getTagByIndex(DescrTagSet::PRODUCTDIR);
    if ( t )
    {
	Pathname dir( t->Data() );
	ndescr->set_product_dir( dir );
    }
    else
    {
	ok = false;
    }

    // content file data
    t = tagset->getTagByIndex(DescrTagSet::PRODUCT);
    if ( t )
    {
	std::list<std::string> multi = t->MultiData();

	if ( multi.size() >= 3 )
	{
	    std::list<std::string>::iterator multi_pos = multi.begin();
	    //  only check if ( !(*multi_pos).empty() ) if an empty string is an error
	    ndescr->set_content_product( PkgNameEd::fromString(*multi_pos) );
	    *multi_pos++;
	    ndescr->set_content_baseproduct( PkgNameEd::fromString(*multi_pos));
	    *multi_pos++;
	    ndescr->set_content_vendor( *multi_pos++ );
	}
	else
	{
	    ok = false;
	}
    }
    else
    {
	ok = false;
    }

    // default base
    t = tagset->getTagByIndex(DescrTagSet::DEFBASE);
    if ( t )
    {
	ndescr->set_content_defaultbase( t->Data() );
    }
    else
    {
	ok = false;
    }

    // archmap
    t = tagset->getTagByIndex(DescrTagSet::ARCH);
    if ( t )
    {
        InstSrcDescr::ArchMap arch;
	std::list<std::string> multi = t->MultiData();
	std::list<std::string>::iterator multi_pos;
	for ( multi_pos = multi.begin(); multi_pos != multi.end(); ++multi_pos )
	{
	    vector<std::string> line = TagParser::split2words (*multi_pos, " ");
	    list<Pathname> paths;
	    unsigned int i;
	    for ( i = 1; i < line.size(); i++ )
	    {
		paths.push_back( Pathname(line[i]) );
	    }
	    arch[line[0]] = paths;
	}
	ndescr->set_content_archmap( arch );
    }
    else
    {
	ok = false;
    }

    // _content_requires
    t = tagset->getTagByIndex(DescrTagSet::REQUIRES);
    if ( t )
    {
	ndescr->set_content_requires( PkgRelation::fromString( t->Data() ) );
    }
    else
    {
	ok = false;
    }

    // content label
     t = tagset->getTagByIndex(DescrTagSet::LABEL);
    if ( t )
    {
	ndescr->set_content_label( t->Data() );
    }
    else
    {
	ok = false;
    }

    // labelmap
    t = tagset->getTagByIndex(DescrTagSet::LABELMAP);
    if ( t )
    {
	InstSrcDescr::LabelMap label;
	std::list<std::string> multi = t->MultiData();
	std::list<std::string>::iterator multi_pos;
	for ( multi_pos = multi.begin(); multi_pos != multi.end(); ++multi_pos )
	{
	    vector<std::string> line = TagParser::split2words (*multi_pos, " ");
	    if ( line.size() >= 2 )
	    {
		label[ LangCode(line[0])] = line[1];
	    }
	}
	ndescr->set_content_labelmap( label );
    }
    else
    {
	ok = false;
    }

    // linguas
    t = tagset->getTagByIndex(DescrTagSet::LINGUAS);
    if ( t )
    {
	std::list<std::string> multi = t->MultiData();
	std::list<std::string>::iterator multi_pos;
	InstSrcDescr::LinguasList linguas;

	for ( multi_pos = multi.begin(); multi_pos != multi.end(); ++multi_pos )
	{
	    linguas.push_back( LangCode(*multi_pos) );
	}
	ndescr->set_content_linguas( linguas );
    }
    else
    {
	ok = false;
    }

    // timezone
     t = tagset->getTagByIndex(DescrTagSet::TIMEZONE);
    if ( t )
    {
	ndescr->set_content_timezone( t->Data() );
    }
    else
    {
	ok = false;
    }

    // descrdir
    t = tagset->getTagByIndex(DescrTagSet::DESCRDIR);
    if ( t )
    {
	ndescr->set_content_descrdir( Pathname(t->Data()) );
    }
    else
    {
	ok = false;
    }

    // datadir
    t = tagset->getTagByIndex(DescrTagSet::DATADIR);
    if ( t )
    {
	ndescr->set_content_datadir( Pathname(t->Data()) );
    }
    else
    {
	ok = false;
    }
    return ok;
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
    PMError err = Error::E_ok;

    InstSrcDescrPtr ndescr( new InstSrcDescr );

    ///////////////////////////////////////////////////////////////////
    // parse _cache_file and fill into ndescr
    ///////////////////////////////////////////////////////////////////

    Pathname fileName = cache_dir_r + _cache_file;

    TagParser parser;
    std::string tagstr;

    std::ifstream mediaCacheStream( fileName.asString().c_str() );

    if( !mediaCacheStream )
    {
	return Error::E_open_file;
    }

    CommonPkdParser::TagSet* tagset;
    tagset = new DescrTagSet();

    bool repeatassign = false;
    bool parse = true;

    while( parse && parser.lookupTag( mediaCacheStream ) )
    {
	tagstr = parser.startTag();

	do
	{
	    switch( tagset->assign( tagstr.c_str(), parser, mediaCacheStream ) )
	    {
		case CommonPkdParser::Tag::ACCEPTED:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOMATCH:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_FULL:
		    tagset->clear();
		    repeatassign = true;
		    err = Error::E_error;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOENDTAG:
		    repeatassign = false;
		    parse = false;
		    break;
	    }
	} while( repeatassign );

    }

    if( parse )
    {
	// fill the InstSrcDescr object
	bool ok = fillInstSrcDescr( ndescr, tagset );

	MIL << "Parsing data from " <<  fileName << ", result: " << (ok?"true":"false") << std::endl;

	if ( !ok )
	{
	    err = Error::E_error;
	}
    }
    else
	MIL << "*** parsing was aborted ***" << std::endl;

    tagset->clear();


    ///////////////////////////////////////////////////////////////////
    // done
    ///////////////////////////////////////////////////////////////////

    if ( err == Error::E_ok )
    {
	ndescr_r = ndescr;
    }

    return err;
}
