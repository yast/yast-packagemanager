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

#include <y2util/Y2SLog.h>

#include <iostream>
#include <fstream>

#include <y2pm/InstSrcDescr.h>


using namespace std;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDescr
//
///////////////////////////////////////////////////////////////////

IMPL_BASE_POINTER(InstSrcDescr);

///////////////////////////////////////////////////////////////////

const std::string InstSrcDescr::_cache_file( "description" );

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
  str << " type:" << _type;
  str << " url:" << _url;
  str << " product dir: " << _product_dir;
  str << " media vendor: " << _media_vendor;
  str << " product:" << _content_product;
  str << " vendor:" << _content_vendor;
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

    TagParser parser;
    std::string tagstr;

    std::ifstream mediaCacheStream( cache_dir_r.asString().c_str() );

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

	MIL << "Parsing data from " <<  cache_dir_r << " ,result: " << (ok?"true":"false") << std::endl;
	
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
    
    // architecture
    t = tagset->getTagByIndex( InstSrcMediaTags::ARCH );
    if ( t )
    {
	ndescr->set_base_arch( PkgArch(t->Data()) );
    }
    else
    {
	ok = false;
    }
    
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
	    if ( !(*multi_pos).empty() )	ndescr->set_media_vendor( *multi_pos++ );
	    if ( !(*multi_pos).empty() )	ndescr->set_media_id( *multi_pos++ );
	    if ( !(*multi_pos).empty() ) 	ndescr->set_media_count( *multi_pos++ );
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
#warning TBD InstSrcDescr cache write

    ofstream file( cache_dir_r.asString().c_str() );

    if ( !file )
    {
	return Error::E_create_file;
    }

    file << ArchTag << ": "<< _base_arch << endl;
    file << TypeTag << ": " << InstSrc::toString(_type) << endl;
    file << UrlTag << ": " << _url << endl;
    file << ProdDirTag << ": " << _product_dir << endl;
    file << DefActTag << ": " << (_default_activate?"1":"0") << endl;
    file << MediaBTag << ": " << endl;
    file << _media_vendor << endl;
    file << _media_id << endl;
    file << _media_count << endl;
    file << MediaETag << ":" << endl;
    
    return Error::E_ok;
}

