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

#include <y2pm/F_Media.h>

#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/InstSrcDataUL.h>

#include <y2pm/ULSelectionParser.h>
#include <y2pm/ULPackagesParser.h>
#include <y2pm/ULParsePackagesLang.h>
#include <y2pm/PMULPackageDataProvider.h>

#include <y2pm/InstSrcDescr.h>
#include <y2pm/InstSrcError.h>
#include <y2pm/MediaAccess.h>

#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgArch.h>

#include <Y2PM.h>
#include <y2pm/InstSrcManager.h>


using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcDataULPtr
//	CLASS NAME : constInstSrcDataULPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(InstSrcDataUL,InstSrcData);

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

    // read media.1/media
    {
      Pathname filename = stringutil::form("/media.1/media" );
      MediaAccess::FileProvider mediafile( media_r, filename );
      if ( mediafile.error() ) {
	ERR << "Media can't provide '" << filename << "' " << mediafile.error() << endl;
	return mediafile.error();
      }

      F_Media f_media;
      err = f_media.read( mediafile() );
      if( err ) {
	return InstSrcError::E_no_instsrc_on_media;
      }

#warning Provide F_Media assign in InstSrcDescr
      ndescr->set_media_vendor( f_media.vendor() );
      ndescr->set_media_id( f_media.ident() );
      ndescr->set_media_count( f_media.count() );
      ndescr->set_media_doublesided( f_media.doublesided() );
      ndescr->set_media_labels( f_media.labels() );
    }

    // read content file
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
	string value = stringutil::getline( content, stringutil::TRIM );

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

    // Finaly check whether there are gpg-pubkeys in the media rootdir
    // and let the ISM store them.
    list<string> files;
    PMError res = media_r->dirInfo( files, "/", /*dots*/false );
    if ( res ) {
      WAR << "Unable to scan media root for gpg-pubkeys: " << res << endl;
    } else {
      PkgPubkeyCache & keyCache( Y2PM::instSrcManager().pkgPubkeyCache() );

      for ( list<string>::const_iterator it = files.begin(); it != files.end(); ++it ) {
	if ( keyCache.isKey( *it ) ) {
	  MediaAccess::FileProvider pubkey( media_r, *it );
	  if ( pubkey.error() ) {
	    WAR << "Media can't provide '" << pubkey() << "' " << pubkey.error() << endl;
	    continue;
	  }
	  res = keyCache.storeKey( pubkey() );
	  if ( res ) {
	    WAR << "Unable to store gpg-pubkey " << pubkey() << ": " << res << endl;
	  }
	}
      }
    }

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

    // parse <descr_dir_r>/selections and <descr_dir_r>/ *.sel

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

    //-----------------------------------------------------
    // slideshow via ftp/http hack
    //-----------------------------------------------------
    if ( ! Y2PM::runningFromSystem() && source->isRemote() ) {
      if ( source->descr()->hasFlag( "no_remote_slides" ) ) {
	MIL << "'no_remote_slides' flag is set. No slide show data download." << endl;
      } else {
	MIL << "Downloading slide show data..." << endl;
	Pathname slide_dir( descr_dir_r.dirname() + "slide" );
	media_r->provideDirTree( slide_dir );
	// shot to nothing: ignore any error
      }
    }
    //-----------------------------------------------------

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
//
//
//	METHOD NAME : InstSrcDataUL::tryGetMediaId
//	METHOD TYPE : PMError
//
PMError InstSrcDataUL::tryGetMediaId( const Url & url_r, const Pathname & product_dir_r,
                                      std::string & mediaId_r )
{
  // reset
  mediaId_r = string();

  // We check the media ID loacted url. There is no product specific
  // test here. In case of multiple products on one media all or nothing
  // will be refreshed. We should try to change this in the future.

  // access media at url
  MediaAccessPtr media( new MediaAccess );
  PMError err;

  if ( (err = media->open( url_r )) )
    {
      ERR << "Failed to open " << url_r << " " << err << endl;
      return err;
    }

  if ( (err = media->attach()) )
    {
      ERR << "Failed to attach media: " << err << endl;
      return err;
    }

  // retrieve /media.1/media
  Pathname filename( "/media.1/media" );
  MediaAccess::FileProvider mediafile( media, filename );
  if ( (err = mediafile.error()) )
    {
      ERR << "Media can't provide '" << filename << "' " << err << endl;
      return err;
    }

  // parse /media.1/media
  F_Media f_media;
  if( (err = f_media.read( mediafile() )) )
    {
      return InstSrcError::E_no_instsrc_on_media;
    }

  // return
  mediaId_r = f_media.ident();
  return Error::E_ok;
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
//	METHOD NAME : InstSrcDataUL::preferredLocaleChanged
//	METHOD TYPE : void
//
void InstSrcDataUL::preferredLocaleChanged() const
{
  if ( _packages.empty() ) {
    MIL << "Nothing to be done: No packages held" << endl;
    return;
  }

  string ofile;
  {
    PMULPackageDataProviderPtr dpt( getDataProvider( *_packages.begin() ) );
    ofile = dpt->_locale_retrieval->getName();
  }
  string stem( ofile );
  string::size_type sep = stem.rfind( "." );
  if ( sep != string::npos ) {
    stem.erase( sep );
  }

  PM::LocaleOrder langs( Y2PM::getLocaleFallback() );
  for ( PM::LocaleOrder::const_iterator lang = langs.begin(); lang != langs.end(); ++lang ) {

    string nfile( stem + "." + lang->code() );
    if ( nfile == ofile ) {
      MIL << "Nothing to be done:  New locale '" << *lang << "' is current locale." << endl;
      return;
    }

    ULParsePackagesLang parser( nfile );
    if ( parser.getRetrieval() ) {
      // parse it...
      reparsePackagesLang( parser );
      MIL << "Reparsed new locale '" << *lang << "'" << endl;
      return;
    }
  }

  MIL << "NOP: Nothing appropriate for  '" << Y2PM::getPreferredLocale() << "'" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataUL::reparsePackagesLang
//	METHOD TYPE : void
//
void InstSrcDataUL::reparsePackagesLang( ULParsePackagesLang & parser_r ) const
{
  map<PkgIdent,PMPackagePtr> table;

  // fill lookuptable with packages
  for ( list<PMPackagePtr>::const_iterator it = _packages.begin(); it != _packages.end(); ++it ) {
    table[PkgIdent(*it)] = *it;
  }

  // parse file and adjust packages data
  for ( ULParsePackagesLang::Entry entry; parser_r.getEntry( entry ) == TaggedFile::ACCEPTED_FULL; /*empty*/ ) {
    PMPackagePtr pkg = table[entry];
    if ( pkg ) {
      adjustDpLangData( pkg, entry );
      table[entry] = 0; // processed
    }
  }

  ULParsePackagesLang::Entry noEntry( parser_r.noEntry() );
  // check for unprocessed packages
  for ( map<PkgIdent,PMPackagePtr>::const_iterator it = table.begin(); it != table.end(); ++it ) {
    if ( it->second ) {
      // unprocessed
      adjustDpLangData( it->second, noEntry );
    }
  }

  return;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcDataUL::adjustDpLangData
//	METHOD TYPE : 	void
//
void InstSrcDataUL::adjustDpLangData( PMPackagePtr pkg_r,
				      const ULParsePackagesLang::Entry & entry_r ) const
{
  PMULPackageDataProviderPtr dp( getDataProvider( pkg_r ) );
  if ( ! dp ) {
    INT << "NO DataProvider for package " << pkg_r << endl;
    return;
  }
  dp->assignLocaleData( entry_r );
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
