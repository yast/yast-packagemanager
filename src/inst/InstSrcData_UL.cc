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

  File:       InstSrcData_UL.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:	parse content and packages (packages.<lang>) file
		from an UnitedLinux compatible media
/-*/

#include <iostream>
#include <fstream>

#include <ctype.h>

#include <y2util/Y2SLog.h>
#include <y2util/PathInfo.h>
#include <y2util/stringutil.h>

#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/InstSrcData_UL.h>
#include <y2pm/PMULPackageDataProvider.h>
#include <y2pm/PMULPackageDataProviderPtr.h>
#include <y2pm/PMULSelectionDataProvider.h>
#include <y2pm/PMULSelectionDataProviderPtr.h>

#include <y2pm/InstSrcDescr.h>
#include <y2pm/MediaAccess.h>

#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgArch.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : InstSrcData_ULPtr
//	CLASS NAME : constInstSrcData_ULPtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(InstSrcData_UL,InstSrcData,InstSrcData);


///////////////////////////////////////////////////////////////////
// PRIVATE
//
//	METHOD NAME : InstSrcData_UL::Tag2PkgRelList
//	METHOD TYPE : int
//
//	DESCRIPTION : convert list of strings (denoting dependencies)
//		      to PMSolvable::PkgRelList_type&
//		      return number of dependencies found

int
InstSrcData_UL::Tag2PkgRelList (PMSolvable::PkgRelList_type& pkgrellist, const std::list<std::string>& relationlist)
{
    int count = 0;
    pkgrellist.clear();
    if (!relationlist.empty())
    {
    for (list<string>::const_iterator relation_str_iter = relationlist.begin();
	 relation_str_iter != relationlist.end();
	 ++relation_str_iter)
    {
	pkgrellist.push_back (PkgRelation::fromString (*relation_str_iter));
	count++;
    }
    }
    return count;
}

///////////////////////////////////////////////////////////////////
// PRIVATE
//
//	METHOD NAME : InstSrcData_UL::PkgTag2Package
//	METHOD TYPE : PMPackagePtr
//
//	DESCRIPTION : pass packages data from tagset to pgkcache
//		 * Single line values are passed by value
//		 * Multi line values are passed by file position (on-demand read)
//
//		 * langcache is only used for PMULPackageDataProvider() constructor
//		 * packagelist is used for finding shared packages

PMPackagePtr
InstSrcData_UL::PkgTag2Package( TagCacheRetrieval *pkgcache,
				TagCacheRetrieval *langcache,
				CommonPkdParser::TagSet * tagset,
				const std::list<PMPackagePtr>* packagelist )
{
    // PACKAGE
    string single ((tagset->getTagByIndex(InstSrcData_ULPkgTags::PACKAGE))->Data());

    std::vector<std::string> splitted;

    stringutil::split (single, splitted, " ", false);
//MIL << "-----------------------------" << endl;
//MIL << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;

    // Pkg -> PMPackage
    PkgName name (splitted[0]);
    PkgEdition edition (splitted[1].c_str(), splitted[2].c_str());
    PkgArch arch (splitted[3]);

    PMULPackageDataProviderPtr dataprovider ( new PMULPackageDataProvider (pkgcache, langcache));
    PMPackagePtr package( new PMPackage (name, edition, arch, dataprovider));
    dataprovider->setPackage (package);

    CommonPkdParser::Tag *tagptr;	// for SET_MULTI()

#define SET_VALUE(tagname,value) \
    do { dataprovider->_attr_##tagname = value; } while (0)
#define SET_POS(tagname,start,stop) \
    do { dataprovider->_attr_##tagname.set (start, stop); } while (0)
#define GET_TAG(tagname) \
    tagset->getTagByIndex(InstSrcData_ULPkgTags::tagname)
#define SET_CACHE(tagname) \
    do { tagptr = GET_TAG (tagname); SET_POS (tagname, tagptr->posDataStart(), tagptr->posDataEnd()); } while (0)

    PMSolvable::PkgRelList_type pkgrellist;

    if (Tag2PkgRelList (pkgrellist, (GET_TAG(REQUIRES))->MultiData()))
	package->setRequires (pkgrellist);
    if (Tag2PkgRelList (pkgrellist, (GET_TAG(PREREQUIRES))->MultiData()))
	package->addPreRequires (pkgrellist); // pkgrellist is modified after that
    if (Tag2PkgRelList (pkgrellist, (GET_TAG(PROVIDES))->MultiData()))
	package->setProvides (pkgrellist);
    if (Tag2PkgRelList (pkgrellist, (GET_TAG(CONFLICTS))->MultiData()))
	package->setConflicts (pkgrellist);
    if (Tag2PkgRelList (pkgrellist, (GET_TAG(OBSOLETES))->MultiData()))
	package->setObsoletes (pkgrellist);

    SET_CACHE (RECOMMENDS);
    SET_CACHE (SUGGESTS);
    SET_CACHE (LOCATION);

    stringutil::split ((GET_TAG(SIZE))->Data(), splitted, " ", false);
    if (splitted.size() > 0)
    {
	SET_VALUE (ARCHIVESIZE, FSize (atoll(splitted[0].c_str())));
	if (splitted.size() > 1)
	{
	    SET_VALUE (SIZE, FSize (atoll(splitted[1].c_str())));
	}
    }
    SET_VALUE (BUILDTIME, Date (GET_TAG(BUILDTIME)->Data()));
    SET_CACHE (SOURCERPM);
    SET_CACHE (GROUP);
    SET_CACHE (LICENSE);
    SET_CACHE (AUTHORS);
    SET_CACHE (KEYWORDS);

#undef SET_VALUE
#undef SET_POS
#undef GET_TAG
#undef SET_CACHE

    // SHAREWITH, package to share data with
    // FIXME: does not support forwared shared declarations

    string sharewith ((tagset->getTagByIndex(InstSrcData_ULPkgTags::SHAREWITH))->Data());
    if (!sharewith.empty())
    {
//MIL << "Share " << package->name() << "-" << package->version() << "-" << package->release() << "." << package->arch() << endl;
	stringutil::split (sharewith, splitted, " ", false);
//MIL << "With " << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;
	const std::list<PMPackagePtr> candidates = InstData::findPackages (packagelist, splitted[0], splitted[1], splitted[2], splitted[3]);

	if (candidates.size() != 1)
	{
	    ERR << "No shared package " << sharewith << endl;
	}
	else
	{
	    // MIL << "Share " << single << " with " << sharewith << endl;
	    dataprovider->setShared (candidates.front()->dataProvider());
	}
    }

    return package;
}


///////////////////////////////////////////////////////////////////
// PRIVATE
//
//	METHOD NAME : InstSrcData_UL::LangTag2Package
//	METHOD TYPE : void
//
//	DESCRIPTION : * pass packages.lang data from tagset to langcache
//		 * Single line values are passed by value
//		 * Multi line values are passed by file position (on-demand read)

void
InstSrcData_UL::LangTag2Package (TagCacheRetrieval *langcache, const std::list<PMPackagePtr>* packagelist, CommonPkdParser::TagSet * tagset)
{
    // PACKAGE
    string single ((tagset->getTagByIndex(InstSrcData_ULLangTags::PACKAGE))->Data());

    std::vector<std::string> splitted;
    stringutil::split (single, splitted, " ", false);
//MIL << "Lang for " << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;

    const std::list<PMPackagePtr> candidates = InstData::findPackages (packagelist, splitted[0], splitted[1], splitted[2], splitted[3]);

    if (candidates.size() != 1)
    {
	ERR << "Ambiguous package " << single << endl;
	return;
    }

    PMPackagePtr package = candidates.front();
    PMULPackageDataProviderPtr dataprovider = package->dataProvider();

    CommonPkdParser::Tag *tagptr;		// for SET_MULTI()

#define SET_POS(tagname,start,stop) \
    do { dataprovider->_attr_##tagname.set (start, stop); } while (0)
#define GET_TAG(tagname) \
    tagset->getTagByIndex(InstSrcData_ULLangTags::tagname)
#define SET_CACHE(tagname) \
    do { tagptr = GET_TAG (tagname); SET_POS (tagname, tagptr->posDataStart(), tagptr->posDataEnd()); } while (0)

    SET_CACHE (SUMMARY);
    SET_CACHE (DESCRIPTION);
    SET_CACHE (INSNOTIFY);
    SET_CACHE (DELNOTIFY);

#undef SET_POS
#undef GET_TAG
#undef SET_CACHE

    return;
}

///////////////////////////////////////////////////////////////////
// PRIVATE
//
//	METHOD NAME : InstSrcData_UL::Tag2Selection
//	METHOD TYPE : PMSelectionPtr
//
//	DESCRIPTION : pass selection data from tagset to PMSelection

PMSelectionPtr
InstSrcData_UL::Tag2Selection (PMULSelectionDataProviderPtr dataprovider, CommonPkdParser::TagSet * tagset)
{
    // SELECTION
    string single ((tagset->getTagByIndex(InstSrcData_ULSelTags::SELECTION))->Data());

    std::vector<std::string> splitted;

    stringutil::split (single, splitted, " ", false);
    while (splitted.size() < 4)
	splitted.push_back("");

//cerr << "-----------------------------" << endl;
//cerr << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;

    // Pkg -> PMSelection
    PkgName name (splitted[0]);
    PkgEdition edition (splitted[1].c_str(), splitted[2].c_str());
    PkgArch arch (splitted[3]);

    PMSelectionPtr selection( new PMSelection (name, edition, arch, dataprovider));
    dataprovider->setSelection (selection);

    CommonPkdParser::Tag *tagptr;	// for SET_MULTI()

#define SET_VALUE(tagname,value) \
    do { dataprovider->_attr_##tagname = value; } while (0)
#define SET_POS(tagname,start,stop) \
    do { dataprovider->_attr_##tagname.set (start, stop); } while (0)
#define GET_TAG(tagname) \
    tagset->getTagByIndex(InstSrcData_ULSelTags::tagname)
#define SET_CACHE(tagname) \
    do { tagptr = GET_TAG (tagname); \
	 SET_POS (tagname, tagptr->posDataStart(), tagptr->posDataEnd()); } while (0)

    PMSolvable::PkgRelList_type pkgrellist;
    if (Tag2PkgRelList (pkgrellist, (GET_TAG(REQUIRES))->MultiData()))
	selection->setRequires (pkgrellist);
    if (Tag2PkgRelList (pkgrellist, (GET_TAG(PROVIDES))->MultiData()))
	selection->setProvides (pkgrellist);
    if (Tag2PkgRelList (pkgrellist, (GET_TAG(CONFLICTS))->MultiData()))
	selection->setConflicts (pkgrellist);
    if (Tag2PkgRelList (pkgrellist, (GET_TAG(OBSOLETES))->MultiData()))
	selection->setObsoletes (pkgrellist);

    SET_CACHE (SUMMARY);
    SET_VALUE (CATEGORY, GET_TAG(CATEGORY)->Data());
    SET_VALUE (VISIBLE, GET_TAG(VISIBLE)->Data() == "true");
    SET_CACHE (SUGGESTS);

    stringutil::split ((GET_TAG(SIZE))->Data(), splitted, " ", false);
    if (splitted.size() > 0)
    {
	SET_VALUE (ARCHIVESIZE, FSize (atoll(splitted[0].c_str())));
	if (splitted.size() > 1)
	{
	    SET_VALUE (SIZE, FSize (atoll(splitted[1].c_str())));
	}
    }

    SET_CACHE (INSPACKS);
    SET_CACHE (DELPACKS);
    SET_VALUE (ORDER, (GET_TAG(ORDER))->Data());

#undef SET_VALUE
#undef SET_POS
#undef GET_TAG
#undef SET_CACHE

    return selection;
}


///////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData_UL::InstSrcData_UL
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
InstSrcData_UL::InstSrcData_UL()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData_UL::~InstSrcData_UL
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
InstSrcData_UL::~InstSrcData_UL()
{
}

//////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData_UL::tryGetDescr
//	METHOD TYPE : PMError
//
//	DESCRIPTION : try to read content data (describing the product)
//			and fill InstSrcDescrPtr class
//
PMError InstSrcData_UL::tryGetDescr( InstSrcDescrPtr & ndescr_r,
				     MediaAccessPtr media_r, const Pathname & product_dir_r )
{
  MIL << "InstSrcData_UL::tryGetDescr(" << product_dir_r << ")" << endl;

  ndescr_r = 0;
  PMError err;

  InstSrcDescrPtr ndescr( new InstSrcDescr );

  ///////////////////////////////////////////////////////////////////
  // parse InstSrcDescr from media_r and fill ndescr
  ///////////////////////////////////////////////////////////////////

#warning TBD must parse media.1/media too

  Pathname filename = product_dir_r + "/content";

  MediaAccess::FileProvider contentfile( media_r, filename );
  if ( contentfile.error() ) {
    ERR << "Media can't provide '" << filename << "' " << contentfile.error() << endl;
    return contentfile.error();
  }

  std::ifstream content( contentfile().asString().c_str());
  if( ! content ) {
    ERR << "Can't open '" << filename << "' for reading." << endl;
    return Error::E_open_file;
  }

  PkgName pname, bname;
  PkgEdition pversion, bversion;	// base product
  InstSrcDescr::ArchMap archmap;
  InstSrcDescr::LabelMap labelmap;

  MIL << "Parsing " << contentfile() << endl;
  while (content.good())
    {
      // If not reding trimed, at least rtrim value
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
	  std::list<Pathname> pathlist;
	  string path;
	  while ( ! (path = stringutil::stripFirstWord( value )).empty() ) {
	    pathlist.push_back( path );
	  }
	  archmap[tag.substr( 5 )] = pathlist;
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
        else if (tag == "REQUIRES")
        {
	  ndescr->set_content_requires (PkgRelation (PkgName (value), NONE, PkgEdition ()));
        }

      } else if ( content.bad() ) {
	ERR << "Error parsing " << contentfile() << endl;
	return Error::E_no_instsrc_on_media;
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
//
//
//	METHOD NAME : InstSrcData_UL::tryGetData
//	METHOD TYPE : PMError
//
//	DESCRIPTION :
//
PMError InstSrcData_UL::tryGetData( InstSrcDataPtr & ndata_r,
				    MediaAccessPtr media_r, const Pathname & descr_dir_r )
{
    MIL << "InstSrcData_UL::tryGetData(" << descr_dir_r << ")" << endl;
    int count = 0;

    ndata_r = 0;
    PMError err;

    std::list<PMPackagePtr> *packagelist = new (std::list<PMPackagePtr>);
    InstSrcDataPtr ndata( new InstSrcData );
    std::string tagstr;

    ///////////////////////////////////////////////////////////////////
    // parse package data
    ///////////////////////////////////////////////////////////////////

    // --------------------------------
    // setup packages access

    Pathname filename = descr_dir_r + "/packages";

    MediaAccess::FileProvider packages( media_r, filename );
    if ( packages.error() ) {
      ERR << "Media can't provide '" << filename << "' " << packages.error() << endl;
      return packages.error();
    }

#warning UNFREED MEMORY?
    MIL << "fopen(" << packages() << ")" << endl;
    TagCacheRetrieval * pkgcache = new TagCacheRetrieval( packages() );

    // --------------------------------
    // setup packages.<lang> access
    // check for packages.<lang> file

#warning Check lang setup. Should use SrcDescr?
    char * lang = getenv ("LANG");
    string langext( "." );
    langext += ( lang ? lang : "en" );
    if ( langext.length() > 3 )
      langext.erase( 3 );

    filename = filename.extend( langext );

    MediaAccess::FileProvider packages_lang( media_r, filename );
    if ( packages_lang.error() ) {
      ERR << "Media can't provide '" << filename << "' " << packages_lang.error() << endl;
      return packages_lang.error();
    }

#warning UNFREED MEMORY?
    MIL << "fopen(" << packages_lang() << ")" << endl;
    TagCacheRetrieval * langcache = new TagCacheRetrieval( packages_lang() );

    // --------------------------------
    // read package data

    std::ifstream& package_stream = pkgcache->getStream();
    if( !package_stream)
    {
	return Error::E_open_file;
    }

    CommonPkdParser::TagSet* tagset = new InstSrcData_ULPkgTags ();
    bool parse = true;
    TagParser & parser = pkgcache->getParser();

    MIL << "start packages parsing" << endl;

    while( parse && parser.lookupTag (package_stream))
    {
	bool repeatassign = false;

	tagstr = parser.startTag();

	do
	{
	    switch( tagset->assign (tagstr.c_str(), parser, package_stream))
	    {
		case CommonPkdParser::Tag::ACCEPTED:
		    repeatassign = false;
		    err = Error::E_ok;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOMATCH:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_FULL:
		    packagelist->push_back (PkgTag2Package( pkgcache, langcache, tagset, packagelist ));
		    count++;
		    tagset->clear();
		    repeatassign = true;
		    err = Error::E_ok;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOENDTAG:
		    repeatassign = false;
		    parse = false;
		    break;
	    }
	} while( repeatassign );
    }

    if (parse)
    {
	// insert final package

	packagelist->push_back (PkgTag2Package( pkgcache, langcache, tagset, packagelist ));
	count++;

	// =============================================================
	// pass packages list to InstSrcData

	ndata->setPackages (packagelist);
    }

    delete tagset;
    tagset = 0;

    if( !parse )
	ERR << "*** parsing packages was aborted ***" << endl;
    else
	MIL << "*** parsed " << count << " packages ***" << std::endl;


    ///////////////////////////////////////////////////////////////////
    // parse language data
    ///////////////////////////////////////////////////////////////////

    std::ifstream& language_stream = langcache->getStream();
    if( !language_stream)
    {
	return Error::E_open_file;
    }

    tagset = new InstSrcData_ULLangTags ();
    parser = langcache->getParser();

    MIL << "start packages.<lang> parsing" << endl;
    count = 0;

    while( parse && parser.lookupTag (language_stream))
    {
	bool repeatassign = false;

	tagstr = parser.startTag();

	do
	{
	    switch( tagset->assign (tagstr.c_str(), parser, language_stream))
	    {
		case CommonPkdParser::Tag::ACCEPTED:
		    repeatassign = false;
		    err = Error::E_ok;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOMATCH:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_FULL:
		    LangTag2Package (langcache, packagelist, tagset);
		    count++;
		    tagset->clear();
		    repeatassign = true;
		    err = Error::E_ok;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOENDTAG:
		    repeatassign = false;
		    parse = false;
		    break;
	    }
	} while( repeatassign );
    }

    if (parse)
    {
	LangTag2Package (langcache, packagelist, tagset);
	count++;
    }

    delete tagset;
    tagset = 0;

    if( !parse )
	ERR << "*** parsing packages.<lang> was aborted ***" << std::endl;
    else
	MIL << "*** parsed " << count << " packages.<lang> entries ***" << std::endl;


    ///////////////////////////////////////////////////////////////////
    // parse selection data
    ///////////////////////////////////////////////////////////////////

    // --------------------------------
    // setup selections access
    // read <DESCRDIR>/selections to std::list<std::string> selection_names

    std::list<std::string> selection_names;

    filename = descr_dir_r + "/selections";

    MediaAccess::FileProvider selections( media_r, filename );
    if ( selections.error() ) {
      WAR << "Media can't provide '" << filename << "' " << selections.error() << endl;
    } else {

      while ( true )
      {
	std::ifstream selstream (selections().asString().c_str());
	if (!selstream)
	{
	    ERR << "Cant open " << selections() << ": " << Error::E_open_file << endl;
	    break;
	}

	while (selstream.good())
	{
	    char lbuf[201];

	    if (!selstream.getline (lbuf, 200, '\n'))
	    {
		if (selstream.eof())
		    break;
		MIL << "getline() failed" << endl;
		break;
	    }
	    if ((lbuf[0] == '#')		// comment
		|| (lbuf[0] == 0))		// empty
		continue;
	    selection_names.push_back (lbuf);
	}
	break;
      }

    }
    MIL << "*** Expecting " << selection_names.size() << " selections ***" << endl;

    std::list<PMSelectionPtr> *selectionlist = new (std::list<PMSelectionPtr>);

    count = 0;

    std::ifstream selection_stream;

    tagset = new InstSrcData_ULSelTags ();
    for (std::list<std::string>::iterator selfile = selection_names.begin();
	 selfile != selection_names.end();
	 ++selfile)
    {
#warning Code does not work for FTP/HTTP. Missing media->provideFile
	Pathname selectionname = descr_dir_r + *selfile;
	selection_stream.open (selectionname.asString().c_str());

	if (!selection_stream)
	{
	    ERR << "Cant open " << selectionname << endl;
	    continue;
	}

	MIL << "Reading " << selectionname << endl;

	PMULSelectionDataProviderPtr dataprovider ( new PMULSelectionDataProvider (selectionname));
	parser = dataprovider->getParser();

	MIL << "start " << *selfile << " parsing" << endl;

	while( parse && parser.lookupTag (selection_stream))
	{
	    bool repeatassign = false;

	    tagstr = parser.startTag();

	do
	{
	    switch( tagset->assign (tagstr.c_str(), parser, selection_stream))
	    {
		case CommonPkdParser::Tag::ACCEPTED:
		    repeatassign = false;
		    err = Error::E_ok;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOMATCH:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_FULL:
		    selectionlist->push_back (Tag2Selection (dataprovider, tagset));
		    count++;
		    tagset->clear();
		    repeatassign = false;	// only single match
		    err = Error::E_ok;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOENDTAG:
		    repeatassign = false;
		    parse = false;
		    break;
	    }
	} while( repeatassign );
        }

	if (parse)
	{
	    selectionlist->push_back (Tag2Selection (dataprovider, tagset));
	    count++;
	}
	tagset->clear();

	delete tagset;
	tagset = 0;

	selection_stream.clear();
	selection_stream.close();
	MIL << "done " << *selfile << " parsing" << endl;
    } // for ()

    ndata->setSelections(selectionlist);

    MIL << "*** parsed " << count << " selections ***" << std::endl;

    ///////////////////////////////////////////////////////////////////
    // done
    ///////////////////////////////////////////////////////////////////

    if ( !err )
    {
	ndata_r = ndata;
	MIL << "tryGetDataUL sucessful" << endl;
    }
    else
    {
	ERR << "tryGetData failed: " << err << endl;
    }

    return err;
}


