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
IMPL_DERIVED_POINTER(InstSrcDataUL,InstSrcData);


#warning Put MediaFile parsing and data into some class or struct!
/**
 * read media.X/media file
 *
 * @param media_r MediaAccessPtr
 * @param number the X in media.X
 * @param vendor where to store vendor
 * @param id where to store id
 * @param count where to store count
 **/
static PMError readMediaFile( MediaAccessPtr media_r,
			      unsigned number, std::string& vendor, std::string& id,
			      unsigned& count, bool & doublesided )
{
    vendor = id = string();
    count = 0;
    doublesided = false;

    Pathname filename = stringutil::form("/media.%d/media", number);

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

    enum MF_state { MF_VENDOR = 0, MF_ID, MF_COUNT, MF_FLAG, MF_NUM_STATES } state = MF_VENDOR;

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
	    case MF_FLAG:
	            doublesided = (value == "doublesided");
		    DBG << "doublesided " << doublesided << endl;
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
	unsigned count;
	bool doublesided;

	err = readMediaFile(media_r, 1, vendor, id, count, doublesided);
	if(err != PMError::E_ok )
	    return err;

	if(vendor.empty() || id.empty())
	{
	    ERR << "Media must have vendor and id" << endl;
	    return InstSrcError::E_no_instsrc_on_media;
	}

	if(count == 0)
	{
	    WAR << "Media count in /media.1/media may not be zero. Assume 1" << endl;
	    count = 1;
	}

	ndescr->set_media_vendor( Vendor(vendor) );
	ndescr->set_media_id(id);
	ndescr->set_media_count(count);
	ndescr->set_media_doublesided(doublesided);
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

    PkgName pname, dname, bname;		// product, dist, base
    PkgEdition pversion, dversion, bversion;	// product, dist, base
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
	else if (tag == "DISTPRODUCT")
	{
	    dname = PkgName (value);
	}
	else if (tag == "DISTVERSION")
	{
	    dversion = PkgEdition (value);
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
	    ndescr->set_content_vendor ( Vendor(value) );
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
	else if (tag == "FLAGS")
	{
	    ndescr->set_content_flags (value);
	}
	else if (tag == "RELNOTESURL")
	{
	    ndescr->set_content_relnotesurl (value);
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
    ndescr->set_content_distproduct (PkgNameEd (dname, dversion));
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
				   MediaAccessPtr media_r, Pathname descr_dir_r,
				   const std::list<PkgArch>& allowed_archs, const LangCode& locale)
{
    MIL << "InstSrcDataUL::tryGetData(" << descr_dir_r << ")" << endl;

    ndata_r = 0;

    //-----------------------------------------------------
    // Check or create cache, if we're allowed to use one.
    //-----------------------------------------------------
    bool late_cache = false;
    {
      PathInfo cpath( source->cache_data_dir() );
      if ( cpath.isDir() && !initDataCache( cpath.path(), source ) ) {
	// fake to setup from cache
	media_r     = new MediaAccess();
	descr_dir_r = "/descr";

	string f_url( "dir:///" );
	f_url += source->cache_data_dir().asString();

	PMError err;
	if ( (err = media_r->open( f_url, source->cache_media_dir() )) ) {
	  ERR << "(F)Failed to open " << f_url << " " << err << endl;
	  return err;
	}

	if ( (err = media_r->attach()) ) {
	  ERR << "(F)Failed to attach media: " << err << endl;
	  return err;
	}
	MIL << "(F)Use cache " << media_r << endl;
      } else {
	MIL << "(F)No cache. Use media " << media_r << endl;
	if ( cpath.isDir() && source->isRemote() ) {
	  // create cache after everything needed is downloaded.
	  late_cache = true;
	}
      }
    }

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

    if ( late_cache ) {
      PathInfo srcdir( source->media()->localPath( source->descr()->descrdir() ) );
      if ( !srcdir.isDir() ) {
	ERR << "Cannot access descr dir on media: " << srcdir << endl;
      } else {
	int ret = PathInfo::copy_dir( srcdir.path(), source->cache_data_dir() );
	if ( ret ) {
	  ERR << "Copy cache data failed: copyDir returned " << ret << endl;
	  PathInfo::recursive_rmdir( source->cache_data_dir() + "descr" );
	} else {
	  MIL << source << " wrote late cache " << source->cache_data_dir() << endl;
	}
      }
    }

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
//	METHOD NAME : InstSrcDataUL::initDataCache
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcDataUL::initDataCache( const Pathname & cache_dir_r, const InstSrcPtr source_r )
{
  PathInfo destdir( cache_dir_r );
  if ( !destdir.isDir() ) {
    WAR << "Cache disabled: cachedir does not exist: " << destdir << endl;
    return Error::E_error;
  }

  destdir( cache_dir_r + "descr" );
  if ( destdir.isExist() ) {
    MIL << "Cache data already exist: " << destdir << endl;
    return Error::E_ok;
  }

  if ( !source_r || !source_r->media() ) {
      ERR << "No InstSrc or no instSrc media" << endl;
      return Error::E_error;
  }

  if ( !source_r->media()->isOpen() ) {
    PMError ret = source_r->media()->open( source_r->descr()->url(),
					   source_r->cache_media_dir() );
    if ( ret ) {
      ERR << "Failed to open media " << source_r->descr()->url() << ": " << ret << endl;
      return Error::E_error;
    }
  }

  if ( !source_r->media()->isAttached() ) {
    PMError ret = source_r->media()->attach();
    if ( ret ) {
      ERR << "Failed to attach media: " << ret << endl;
      return Error::E_error;
    }
  }

  ///////////////////////////////////////////////////////////////////
  // Now copy data to cache_dir_r
  ///////////////////////////////////////////////////////////////////

#warning Fix: late provide for FTP/HTTP
  PathInfo srcdir( source_r->media()->localPath( source_r->descr()->descrdir() ) );
  if ( !srcdir.isDir() ) {
    ERR << "Cannot access descr dir on media: " << srcdir << endl;
    return Error::E_error;
  }

#warning Fix: assume dir named descr
  int ret = PathInfo::copy_dir( srcdir.path(), cache_dir_r );
  if ( ret ) {
    ERR << "Copy cache data failed: copyDir returned " << ret << endl;
    PathInfo::recursive_rmdir( cache_dir_r + "descr" );
    return Error::E_error;
  }

  MIL << source_r << " wrote cache " << cache_dir_r << endl;
  return Error::E_ok;
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

  if ( !attached() ) {
      ERR << "Not attached to InstSrc" << endl;
      return Error::E_error;
  }

  ///////////////////////////////////////////////////////////////////
  // If we've got a local cache, copy it's content.
  // Otherwise init from media.
  ///////////////////////////////////////////////////////////////////

  PathInfo srcdir( _instSrc->cache_data_dir() + "descr" );
  if ( srcdir.isDir() ) {

    int ret = PathInfo::copy_dir( srcdir.path(), cache_dir_r );
    if ( ret ) {
      ERR << "Copy cache data failed: copyDir returned " << ret << endl;
      PathInfo::recursive_rmdir( cache_dir_r + "descr" );
      return Error::E_error;
    }

  } else {

    PMError err = initDataCache( cache_dir_r, _instSrc );
    if ( err ) {
      return err;
    }

  }

  MIL << *this << " wrote cache " << cache_dir_r << endl;
  return Error::E_ok;
}
