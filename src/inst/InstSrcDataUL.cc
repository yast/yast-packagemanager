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

  File:       InstSrcDataUL.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:	parse content and packages (packages.<lang>) file
		from an UnitedLinux compatible media
/-*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <cstdlib>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>
#include <y2util/stringutil.h>
#include <y2util/TaggedFile.h>

#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/InstSrcDataUL.h>

#include <y2pm/ULSelectionParser.h>
#include <y2pm/ULPackagesParser.h>

#include <y2pm/InstSrcDescr.h>
#include <y2pm/InstSrcError.h>
#include <y2pm/MediaAccess.h>

#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgArch.h>

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcDataULPtr
//	CLASS NAME : constInstSrcDataULPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(InstSrcDataUL,InstSrcData,InstSrcData);



PMError
InstSrcDataUL::readMediaFile(const Pathname& product_dir, MediaAccessPtr media_r, unsigned number, std::string& vendor, std::string& id, unsigned& count)
{
    vendor = id = string();
    count = 0;

    Pathname filename = product_dir + stringutil::form("/media.%d/media", number);

    MediaAccess::FileProvider contentfile( media_r, filename );
    if ( contentfile.error() ) {
	ERR << "Media can't provide '" << filename << "' " << contentfile.error() << endl;
	return contentfile.error();
    }

    std::ifstream content( contentfile().asString().c_str());
    if( ! content ) {
	ERR << "Can't open '" << filename << "' for reading." << endl;
	return InstSrcError::E_open_file;
    }

    enum MF_state { MF_VENDOR = 0, MF_ID, MF_COUNT, MF_NUM_STATES } state = MF_VENDOR;

    string value;
    // If not reading trimmed, at least rtrim value
    for ( value = stringutil::getline( content, true );
	content.good() && state < MF_NUM_STATES;
	value = stringutil::getline( content, true ),
	    state = MF_state(state+1))
    {
	switch (state)
	{
	    case MF_VENDOR:
		    vendor = value;
		    DBG << "vendor " << value << endl;
		break;
	    case MF_ID:
		    id = value;
		    DBG << "id " << value << endl;
		break;
	    case MF_COUNT:
		    count = atoi(value.c_str());
		    DBG << "count " << count << endl;
		break;
	    case MF_NUM_STATES:
		break;
	}

    }

    if ( content.bad() )
    {
	ERR << "Error parsing " << contentfile() << endl;
	return InstSrcError::E_no_instsrc_on_media;
    }

    return InstSrcError::E_ok;
}

//////////////////////////////////////////////////////////////////
// static
//
//	METHOD NAME : InstSrcDataUL::tryGetDescr
//	METHOD TYPE : PMError
//
//	DESCRIPTION : try to read content data (describing the product)
//			and fill InstSrcDescrPtr class
//
PMError InstSrcDataUL::tryGetDescr( InstSrcDescrPtr & ndescr_r,
				     MediaAccessPtr media_r, const Pathname & product_dir_r )
{
    MIL << "InstSrcDataUL::tryGetDescr(" << product_dir_r << ")" << endl;

    ndescr_r = 0;
    PMError err;

    InstSrcDescrPtr ndescr( new InstSrcDescr );

    ///////////////////////////////////////////////////////////////////
    // parse InstSrcDescr from media_r and fill ndescr
    ///////////////////////////////////////////////////////////////////

    {
	// read media.1/media
	string vendor;
	string id;
	unsigned count,
	err = readMediaFile(product_dir_r, media_r, 1, vendor, id, count);
	if(err != PMError::E_ok )
	    return err;

	if(vendor.empty() || id.empty())
	{
	    ERR << "Media must have vendor and id" << endl;
	    return InstSrcError::E_no_instsrc_on_media;
	}

	if(count == 0)
	{
	    WAR << "Media count in media.1/media may not be zero. Assume 1" << endl;
	    count = 1;
	}

	ndescr->set_media_vendor(vendor);
	ndescr->set_media_id(id);
	ndescr->set_media_count(count);
    }

    Pathname filename = product_dir_r + "/content";

    MediaAccess::FileProvider contentfile( media_r, filename );
    if ( contentfile.error() ) {
	ERR << "Media can't provide '" << filename << "' " << contentfile.error() << endl;
	return contentfile.error();
    }

    std::ifstream content( contentfile().asString().c_str());
    if( ! content ) {
	ERR << "Can't open '" << filename << "' for reading." << endl;
	return InstSrcError::E_open_file;
    }

    PkgName pname, bname;
    PkgEdition pversion, bversion;	// base product
    InstSrcDescr::ArchMap archmap;
    InstSrcDescr::LabelMap labelmap;

    MIL << "Parsing " << contentfile() << endl;
    while (content.good())
    {
	// If not reading trimmed, at least rtrim value
	string value = stringutil::getline( content, true );

	if ( ! (content.fail() || content.bad()) ) {
	string tag = stringutil::stripFirstWord( value );

	if (tag == "PRODUCT")
	{
	    pname = PkgName (value);
	}
	else if (tag == "VERSION")
	{
	    pversion = PkgEdition (value);
	}
	else if (tag == "BASEPRODUCT")
	{
	    bname = PkgName (value);
	}
	else if (tag == "BASEVERSION")
	{
	    bversion = PkgEdition (value);
	}
	else if (tag == "VENDOR")
	{
	    ndescr->set_content_vendor (value);
	}
	else if (tag == "DEFAULTBASE")
	{
	    ndescr->set_content_defaultbase (value);
	}
	else if (tag.find( "ARCH." ) == 0)
	{
	    std::list<PkgArch> archlist;
	    string archstr;
	    while ( ! (archstr = stringutil::stripFirstWord( value )).empty() ) {
	      archlist.push_back( PkgArch (archstr) );
	    }
	    archmap[tag.substr( 5 )] = archlist;
	}
	else if (tag == "LINGUAS")
	{
	    std::list<LangCode> langlist;
	    string lang;
	    while ( ! (lang = stringutil::stripFirstWord( value )).empty() ) {
		langlist.push_back( LangCode(lang) );
	    }
	    ndescr->set_content_linguas (langlist);
	}
	else if (tag == "LANGUAGE")
	{
	    ndescr->set_content_language (LangCode (value));
	}
	else if (tag == "LABEL")
	{
	    ndescr->set_content_label (value);
	}
	else if (tag.find( "LABEL." ) == 0)
	{
	    labelmap[LangCode(tag.substr( 6 ))] = value;
	}
	else if (tag == "TIMEZONE")
	{
	    ndescr->set_content_timezone (value);
	}
	else if (tag == "DESCRDIR")
	{
	    ndescr->set_content_descrdir (value);
	}
	else if (tag == "DATADIR")
	{
	    ndescr->set_content_datadir (value);
	}
	else if (tag == "YOUURL")
	{
	    ndescr->set_content_youurl (value);
	}
	else if (tag == "YOUTYPE")
	{
	    ndescr->set_content_youtype (value);
	}
	else if (tag == "YOUPATH")
	{
	    ndescr->set_content_youpath (value);
	}
	else if (tag == "REQUIRES")
	{
	    ndescr->set_content_requires (PkgRelation (PkgName (value), NONE, PkgEdition ()));
	}

      } else if ( content.bad() ) {
	ERR << "Error parsing " << contentfile() << endl;
	return InstSrcError::E_no_instsrc_on_media;
      }
    }

    ndescr->set_content_product (PkgNameEd (pname, pversion));
    ndescr->set_content_baseproduct (PkgNameEd (bname, bversion));
    ndescr->set_content_archmap (archmap);
    ndescr->set_content_labelmap (labelmap);

    ndescr->set_product_dir (product_dir_r);

    ///////////////////////////////////////////////////////////////////
    // done
    ///////////////////////////////////////////////////////////////////
    if ( ! err ) {
	ndescr_r = ndescr;
    }

    return err;
}


///////////////////////////////////////////////////////////////////
// static
//
//	METHOD NAME : InstSrcDataUL::tryGetData
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcDataUL::tryGetData( const InstSrcPtr source, InstSrcDataPtr& ndata_r,
				MediaAccessPtr media_r, const Pathname & descr_dir_r,
				const std::list<PkgArch>& allowed_archs, const LangCode& locale)
{
    MIL << "InstSrcDataUL::tryGetData(" << descr_dir_r << ")" << endl;

    ndata_r = 0;

    //-----------------------------------------------------
    // create instance of _own_ class
    //-----------------------------------------------------

    InstSrcDataULPtr ndata( new InstSrcDataUL() );

    // parse <descr_dir_r>/packages and <descr_dir_r>/packages.<locale>

    ULPackagesParser pkg_parser (source);
    PMError pkgerr = pkg_parser.fromMediaDir (ndata->_packages, media_r, descr_dir_r, allowed_archs, locale);

    // parse <descr_dir_r>/selections and <descr_dir_r>/*.sel

    ULSelectionParser sel_parser (source);
    PMError selerr = sel_parser.fromMediaDir (ndata->_selections, media_r, descr_dir_r);

    if (!pkgerr
	|| !selerr )
    {
	//-----------------------------------------------------
	// keep instance of _own_ class
	//-----------------------------------------------------

	ndata_r = ndata;			// keep ndata alive
	MIL << "tryGetDataUL sucessful" << endl;
	MIL << ndata->getPackages().size() << " packages" << endl;
	MIL << ndata->getSelections().size() << " selections" << endl;
    }
    else
    {
	//-----------------------------------------------------
	// destroy instance of _own_ class
	//-----------------------------------------------------
						// destroy ndata
	ERR << "tryGetData failed pkg: " << pkgerr << endl;
	ERR << "tryGetData failed sel: " << selerr << endl;
    }
    if (pkgerr) return pkgerr;
    if (selerr) return selerr;
    return PMError::E_ok;
}

///////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataUL::InstSrcDataUL
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstSrcDataUL::InstSrcDataUL()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataUL::~InstSrcDataUL
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcDataUL::~InstSrcDataUL()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataUL::loadObjects
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError
InstSrcDataUL::loadObjects()
{
  return Error::E_error;
#if 0
  if ( !_instSrc->descr() ) {
    return E_src_no_description;
  }

  ///////////////////////////////////////////////////////////////////
  // init global settings
  ///////////////////////////////////////////////////////////////////

  _descr_dir = _instSrc->descr()->descrdir();
  _data_dir  = _instSrc->descr()->datadir();

#warning Check lang setup. Should use SrcDescr?
  char * lang = getenv ("LANG");
  if ( lang ) {
    _default_langcode = lang;
    if ( _default_langcode.length() > 3 )
      _default_langcode.erase( 3 );
  } else {
    _default_langcode = _fallback_langcode;
  }

  ///////////////////////////////////////////////////////////////////
  // determine whether to read data from cache or media
  ///////////////////////////////////////////////////////////////////

  bool useCache = false;

  return Error::E_no_instsrcdata_on_media;
#endif
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataUL::writeCache
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError
InstSrcDataUL::writeCache( const Pathname & cache_dir_r ) const
{
  PathInfo destdir( cache_dir_r );
  if ( !destdir.isDir() ) {
    WAR << "Cache disabled: cachedir does not exist: " << destdir << endl;
    return Error::E_ok;
  }

  destdir( cache_dir_r + "descr" );
  if ( destdir.isExist() ) {
    MIL << "Cache data already exist: " << destdir << endl;
    return Error::E_ok;
  }

  if ( !attached() || !_instSrc->media() || !_instSrc->media()->isOpen() ) {
    ERR << "Not attached to InstSrc or instSrc media not open" << endl;
    return Error::E_error;
  }

  // uggly: missing provide stuff...
  PathInfo srcdir( _instSrc->media()->localPath( _instSrc->descr()->descrdir() ) );
  if ( !srcdir.isDir() ) {
    ERR << "Cannot access descr dir on media: " << srcdir << endl;
    return Error::E_error;
  }

  int ret = PathInfo::copy_dir( srcdir.path(), cache_dir_r );
  if ( ret ) {
    ERR << "Copy cache data failed: copyDir returned " << ret << endl;
    return Error::E_error;
  }

  MIL << *this << " wrote cache " << cache_dir_r << endl;
  return Error::E_ok;
}

