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
//	METHOD NAME : InstSrcDescr::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrcDescr::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str ) << "[====================" << endl;
  writeStream( str );
  return str << "====================]" << endl;

  Rep::dumpOn( str ) << "(";
  str << " type: " << _type;
  str << " url: " << _url;
  str << " product dir: " << _product_dir;
  str << " media vendor: " << _media_vendor;
  str << " product: " << _content_product;
  str << " vendor: " << _content_vendor;

  map< string, list<Pathname> >::const_iterator it;
  std::list<Pathname> paths;
  str << " archmap: ";
  for ( it = _content_archmap.begin(); it != _content_archmap.end(); ++it )
  {
      str << (*it).first << ": ";
      paths = (*it).second;
      str << paths.front() << " " << paths.back() << " ";
  }
  str << "labelmap: ";
  LabelMap::const_iterator labelIt;
  for ( labelIt = _content_labelmap.begin(); labelIt != _content_labelmap.end(); ++labelIt )
  {
      str << (*labelIt).first << " " << (*labelIt).second << " ";
  }
  str << "linguas: ";
  list<LangCode>::const_iterator lingIt;
  for ( lingIt = _content_linguas.begin(); lingIt != _content_linguas.end(); ++lingIt )
  {
      str << (*lingIt) << " ";
  }
  return str << ")";
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
    tagset = new InstSrcMediaTags();

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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDescr::fillInstSrcDescr
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool InstSrcDescr::fillInstSrcDescr( InstSrcDescrPtr & ndescr, CommonPkdParser::TagSet * tagset )
{
    bool ok = true;
    CommonPkdParser::Tag* t = 0;

    if ( !tagset )
	return false;

    // TODO: set ok=false if the result string is empty (are all values required ???)

    // default activate
    t = tagset->getTagByIndex(InstSrcMediaTags::ACTIVATE);
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
    t = tagset->getTagByIndex(InstSrcMediaTags::TYPE);
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
    t = tagset->getTagByIndex(InstSrcMediaTags::MEDIA);
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
    t = tagset->getTagByIndex(InstSrcMediaTags::URL);
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
    t = tagset->getTagByIndex(InstSrcMediaTags::PRODUCTDIR);
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
    t = tagset->getTagByIndex(InstSrcMediaTags::PRODUCT);
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
    t = tagset->getTagByIndex(InstSrcMediaTags::DEFBASE);
    if ( t )
    {
	ndescr->set_content_defaultbase( t->Data() );
    }
    else
    {
	ok = false;
    }

    // archmap
    t = tagset->getTagByIndex(InstSrcMediaTags::ARCH);
    if ( t )
    {
	ArchMap arch;
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

    // content label
     t = tagset->getTagByIndex(InstSrcMediaTags::LABEL);
    if ( t )
    {
	ndescr->set_content_label( t->Data() );
    }
    else
    {
	ok = false;
    }

    // labelmap
    t = tagset->getTagByIndex(InstSrcMediaTags::LABELMAP);
    if ( t )
    {
	LabelMap label;
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
    t = tagset->getTagByIndex(InstSrcMediaTags::LINGUAS);
    if ( t )
    {
	std::list<std::string> multi = t->MultiData();
	std::list<std::string>::iterator multi_pos;
	LinguasList linguas;

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
     t = tagset->getTagByIndex(InstSrcMediaTags::TIMEZONE);
    if ( t )
    {
	ndescr->set_content_timezone( t->Data() );
    }
    else
    {
	ok = false;
    }

    // descrdir
    t = tagset->getTagByIndex(InstSrcMediaTags::DESCRDIR);
    if ( t )
    {
	ndescr->set_content_descrdir( Pathname(t->Data()) );
    }
    else
    {
	ok = false;
    }

    // datadir
    t = tagset->getTagByIndex(InstSrcMediaTags::DATADIR);
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
      ERR << "writeCache " << fileName << " " << Error::E_create_file << endl;
      return Error::E_create_file;
    }

    writeStream( file );

    if ( !file ) {
      ERR << "writeCache " << fileName << " " << Error::E_write_file << endl;
      return Error::E_write_file;
    }

    return Error::E_ok;
}

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

#warning TODO: write _content_requires
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


#if 0
void
InstSrcDescr::parseSuSEFile (const Pathname & mountpoint, const Pathname & susefile, bool new_media)
{
    char buf[101];
    const char *sptr = susefile.asString().c_str();
    char *dot;

    sptr += 14;		// skip ".S.u.S.E-disk-"

    // find CD number "NNN."

    _number = 0;
    while (isdigit (*sptr))
    {
	_number *= 10;
	_number += (*sptr - '0');
	sptr++;
    }

    if (*sptr != '.')		// '.' is separator between number and ID
	return;
    sptr++;
    _id = string (sptr);

    if (!new_media)
	return;

    // if not CD1, dont expect further info
    if (_number != 1)
	return;

    // CD1, look for suse/setup/descr/info

    FILE *info = fopen ((mountpoint + "suse/setup/descr/info").asString().c_str(), "r");

    fprintf (stderr, "parseSuSEFile(%s) = %p\n", filename.asString().c_str(), info);
    if (info == 0)
	return;

    while (!feof (info))
    {
	char *ptr;
	char *value;

	if (fgets (buf, 100, info) != buf)
	{
	    break;
	}
	ptr = buf;
	while (*ptr)			// skip keywork
	{
	    if ((*ptr == ' ')
		|| (*ptr == '\t'))
	    {
		*ptr++ = 0;
		break;
	    }
	    ptr++;
	}
	while ((*ptr == ' ')		// skip whitespace
		|| (*ptr == '\t'))
	{
	    ptr++;
	}
	value = ptr;

	while (*ptr)
	{
	    if (*ptr == '\n')		// delete trailing \n
	    {
		*ptr = 0;
		break;
	    }
	    ptr++;
	}

	// check keys, copy value

	if (strcmp (key, "PRODUKT_NAME") == 0)
	{
	    _product = value;
	}
	else if (strcmp (key, "PRODUKT_VERSION") == 0)
	{
	    _version = value;
	}
	else if (strcmp (key, "DISTRIBUTION_RELEASE") == 0)
	{
	    _release = atoi (value);
	}
	else if (strcmp (key, "DIST_STRING") == 0)
	{
	    _vendor = value;
	}
	else if (strcmp (key, "DISTRIBUTION_NAME") == 0)
	{
	    _label = value;
	}
    } // while

    fclose (info);

    return;
}


/**
 * write media description to cache file
 * @return pathname of written cache
 * writes private data to an ascii file
 */
const Pathname
InstSrcDescr::writeCache (void)
{
    return Pathname ("");	// empty == error
}

#endif

