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

    CommonPkdParser::Tag *tagptr;	// for SET_MULTI()

#define SET_VALUE(tagname,value) \
    if (!value.empty()) dataprovider->setAttributeValue (package, (PMPackage::PMPackageAttribute)PMPackage::ATTR_##tagname, value)
#define SET_POS(tagname,begin,end) \
    dataprovider->setAttributeValue (package, (PMPackage::PMPackageAttribute)PMPackage::ATTR_##tagname, begin, end)
#define GET_TAG(tagname) \
    tagset->getTagByIndex(InstSrcData_ULPkgTags::tagname)
#define SET_MULTI(tagname) \
    do { tagptr = GET_TAG (tagname); SET_POS (tagname, tagptr->posDataStart(), tagptr->posDataEnd()); } while (0)
#define SET_SINGLE(tagname) \
    SET_VALUE (tagname, (GET_TAG(tagname))->Data())

    SET_VALUE (NAME, splitted[0]);
    SET_VALUE (VERSION, splitted[1]);
    SET_VALUE (RELEASE, splitted[2]);
    SET_VALUE (ARCH, splitted[3]);

    SET_MULTI (REQUIRES);
    SET_MULTI (PREREQUIRES);
    SET_MULTI (PROVIDES);
    SET_MULTI (CONFLICTS);
    SET_MULTI (OBSOLETES);

    SET_MULTI (RECOMMENDS);
    SET_MULTI (SUGGESTS);
    SET_SINGLE (LOCATION);

    stringutil::split ((tagset->getTagByIndex(InstSrcData_ULPkgTags::SIZE))->Data(), splitted, " ", false);
    SET_VALUE (ARCHIVESIZE, splitted[0]);
    SET_VALUE (SIZE, splitted[1]);
    SET_SINGLE (BUILDTIME);
    SET_SINGLE (SOURCERPM);
    SET_SINGLE (GROUP);
    SET_SINGLE (LICENSE);
    SET_MULTI (AUTHOR);
    SET_MULTI (KEYWORDS);

#undef SET_VALUE
#undef SET_POS
#undef GET_TAG
#undef SET_SINGLE
#undef SET_MULTI

    // SHAREWITH, package to share data with
    // FIXME: does not support forwared shared declarations

    string sharewith ((tagset->getTagByIndex(InstSrcData_ULPkgTags::SHAREWITH))->Data());
    if (!sharewith.empty())
    {
	stringutil::split (sharewith, splitted, " ", false);
	const std::list<PMPackagePtr>* candidates = InstData::findPackages (packagelist, splitted[0], splitted[1], splitted[2], splitted[3]);

	if (!candidates
	    || candidates->size() != 1)
	{
	    ERR << "No shared package " << sharewith << endl;
	}
	else
	{
	    // MIL << "Share " << single << " with " << sharewith << endl;
	    dataprovider->setShared (candidates->front()->dataProvider());
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

    const std::list<PMPackagePtr>* candidates = InstData::findPackages (packagelist, splitted[0], splitted[1], splitted[2], splitted[3]);

    if (!candidates
	|| candidates->size() != 1)
    {
	ERR << "Ambiguous package " << single << endl;
	return;
    }

    PMPackagePtr package = candidates->front();
    PMULPackageDataProviderPtr dataprovider = package->dataProvider();

    CommonPkdParser::Tag *tagptr;		// for SET_MULTI()

#define SET_VALUE(tagname,value) \
    dataprovider->setAttributeValue (package, (PMPackage::PMPackageAttribute)PMPackage::ATTR_##tagname, value)
#define SET_POS(tagname,begin,end) \
    dataprovider->setAttributeValue (package, (PMPackage::PMPackageAttribute)PMPackage::ATTR_##tagname, begin, end)
#define GET_TAG(tagname) \
    tagset->getTagByIndex(InstSrcData_ULLangTags::tagname)
#define SET_MULTI(tagname) \
    do { tagptr = GET_TAG (tagname); SET_POS (tagname, tagptr->posDataStart(), tagptr->posDataEnd()); } while (0)
#define SET_SINGLE(tagname) \
    SET_VALUE (tagname, (GET_TAG(tagname))->Data())

    SET_SINGLE (SUMMARY);
    SET_MULTI (DESCRIPTION);
    SET_MULTI (INSNOTIFY);
    SET_MULTI (DELNOTIFY);

#undef SET_VALUE
#undef SET_POS
#undef GET_TAG
#undef SET_SINGLE
#undef SET_MULTI

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
InstSrcData_UL::Tag2Selection (TagCacheRetrieval *selcache, CommonPkdParser::TagSet * tagset)
{
    // SELECTION
    string single ((tagset->getTagByIndex(InstSrcData_ULSelTags::SELECTION))->Data());

    std::vector<std::string> splitted;

    stringutil::split (single, splitted, " ", false);
    if (splitted.size() < 4)
	splitted.push_back("");

//MIL << "-----------------------------" << endl;
//MIL << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;

    // Pkg -> PMSelection
    PkgName name (splitted[0]);
    PkgEdition edition (splitted[1].c_str(), splitted[2].c_str());
    PkgArch arch (splitted[3]);

    PMULSelectionDataProviderPtr dataprovider ( new PMULSelectionDataProvider (selcache));
    PMSelectionPtr selection( new PMSelection (name, edition, arch, dataprovider));

    CommonPkdParser::Tag *tagptr;	// for SET_MULTI()

#define SET_VALUE(tagname,value) \
    if (!value.empty()) dataprovider->setAttributeValue (selection, (PMSelection::PMSelectionAttribute)PMSelection::ATTR_##tagname, value)
#define SET_POS(tagname,begin,end) \
    dataprovider->setAttributeValue (selection, (PMSelection::PMSelectionAttribute)PMSelection::ATTR_##tagname, begin, end)
#define GET_TAG(tagname) \
    tagset->getTagByIndex(InstSrcData_ULSelTags::tagname)
#define SET_MULTI(tagname) \
    do { tagptr = GET_TAG (tagname); SET_POS (tagname, tagptr->posDataStart(), tagptr->posDataEnd()); } while (0)
#define SET_SINGLE(tagname) \
    SET_VALUE (tagname, (GET_TAG(tagname))->Data())

    SET_VALUE (NAME, splitted[0]);
    SET_VALUE (VERSION, splitted[1]);
    SET_VALUE (RELEASE, splitted[2]);
    SET_VALUE (ARCH, splitted[3]);

    SET_SINGLE (SUMMARY);
    SET_SINGLE (CATEGORY);
    SET_SINGLE (VISIBLE);

    SET_MULTI (REQUIRES);
    SET_MULTI (PROVIDES);
    SET_MULTI (CONFLICTS);
    SET_MULTI (OBSOLETES);

    stringutil::split ((tagset->getTagByIndex(InstSrcData_ULSelTags::SIZE))->Data(), splitted, " ", false);
    SET_VALUE (ARCHIVESIZE, splitted[0]);
    SET_VALUE (SIZE, splitted[1]);

    SET_MULTI (INSTALL);
    SET_MULTI (DELETE);

#undef SET_VALUE
#undef SET_POS
#undef GET_TAG
#undef SET_SINGLE
#undef SET_MULTI

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
    : InstSrcData ("")
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

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : InstSrcData_UL::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & InstSrcData_UL::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

///////////////////////////////////////////////////////////////////
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

    Pathname contentname = product_dir_r + "/content";
    MIL << "provideFile (" << contentname << ")" << endl;
    err = media_r->provideFile (contentname);
    if (err != Error::E_ok)
    {
	return err;
    }

    contentname = media_r->getAttachPoint() + contentname;
    MIL << "ifstream(" << contentname << ")" << endl;
    std::ifstream content (contentname.asString().c_str());
    if (!content)
    {
	return Error::E_open_file;
    }

    err = Error::E_ok;

    PkgName pname, bname;
    PkgEdition pversion, bversion;	// base product
    InstSrcDescr::ArchMap archmap;
    InstSrcDescr::LabelMap labelmap;

  while (content.good())
  {
    char lbuf[201];

    if (!content.getline (lbuf, 200, '\n'))
    {
      if (content.eof())
	break;
      MIL << "getline() failed" << endl;
      err = Error::E_no_instsrc_on_media;
      break;
    }

    char *lptr = lbuf;
    while (!isblank (*lptr)) lptr++;
    if (*lptr == 0)		// empty value
	continue;
    *lptr++ = 0;
    while (isblank (*lptr)) lptr++;
    if (*lptr == 0)		// empty value
	continue;
    char *vptr = lptr;		// vptr == value
    while (*lptr) lptr++;
    lptr--;

    if (strcmp (lbuf, "PRODUCT") == 0)
    {
	pname = PkgName (vptr);
    }
    else if (strcmp (lbuf, "VERSION") == 0)
    {
	pversion = PkgEdition (vptr);
    }
    else if (strcmp (lbuf, "BASEPRODUCT") == 0)
    {
	bname = PkgName (vptr);
    }
    else if (strcmp (lbuf, "BASEVERSION") == 0)
    {
	bversion = PkgEdition (vptr);
    }
    else if (strcmp (lbuf, "VENDOR") == 0)
    {
	ndescr->set_content_vendor (vptr);
    }
    else if (strcmp (lbuf, "DEFAULTBASE") == 0)
    {
	ndescr->set_content_defaultbase (vptr);
    }
    else if (strncmp (lbuf, "ARCH.", 5) == 0)
    {
	std::list<Pathname> pathlist;
	char *path = vptr;
	while (*vptr)
	{
	    if (isblank (*vptr))
	    {
		*vptr++ = 0;
		pathlist.push_back (Pathname (path));
		path = vptr;
	    }
	    vptr++;
	}
	pathlist.push_back (Pathname (path));
	archmap[lbuf+5] = pathlist;
    }
    else if (strcmp (lbuf, "LINGUAS") == 0)
    {
	std::list<LangCode> langlist;
	char *lang = vptr;
	while (*vptr)
	{
	    if (isblank (*vptr))
	    {
		*vptr++ = 0;
		langlist.push_back (LangCode (lang));
		lang = vptr;
	    }
	    vptr++;
	}
	langlist.push_back (LangCode (lang));
	ndescr->set_content_linguas (langlist);
    }
    else if (strcmp (lbuf, "LANGUAGE") == 0)
    {
	ndescr->set_content_language (LangCode (vptr));
    }
    else if (strcmp (lbuf, "LABEL") == 0)
    {
	ndescr->set_content_label (vptr);
    }
    else if (strncmp (lbuf, "LABEL.", 6) == 0)
    {
	labelmap[LangCode(lbuf+6)] = vptr;
    }
    else if (strcmp (lbuf, "TIMEZONE") == 0)
    {
	ndescr->set_content_timezone (vptr);
    }
    else if (strcmp (lbuf, "DESCRDIR") == 0)
    {
	ndescr->set_content_descrdir (vptr);
    }
    else if (strcmp (lbuf, "DATADIR") == 0)
    {
	ndescr->set_content_datadir (vptr);
    }
    else if (strcmp (lbuf, "REQUIRES") == 0)
    {
	ndescr->set_content_requires (PkgRelation (PkgName (vptr), NONE, PkgEdition ()));
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
  if ( !err ) {
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
    InstSrcDataPtr ndata( new InstSrcData (media_r) );
    std::string tagstr;

    ///////////////////////////////////////////////////////////////////
    // parse package data
    ///////////////////////////////////////////////////////////////////

    // --------------------------------
    // setup packages access

    Pathname packagesname = descr_dir_r + "/packages";
    MIL << "provideFile (" << packagesname << ")" << endl;
    err = media_r->provideFile (packagesname);
    if (err != Error::E_ok)
    {
	return err;
    }
    packagesname = media_r->getAttachPoint() + packagesname;
    MIL << "fopen(" << packagesname << ")" << endl;
    TagCacheRetrieval *pkgcache = new TagCacheRetrieval (packagesname);

    // --------------------------------
    // setup packages.<lang> access
    // check for packages.<lang> file

    char *lang = getenv ("LANG");
    if (lang == 0) lang = "en";
    if (strlen (lang) > 2) lang = strndup (lang, 2);

    Pathname langname = packagesname.asString() + "." + lang;
    TagCacheRetrieval *langcache = 0;
    err = media_r->provideFile (langname);
    if (err != Error::E_ok)
    {
	return err;
    }
    MIL << "fopen(" << langname << ")" << endl;
    langcache = new TagCacheRetrieval (langname);

    // --------------------------------
    // read package data

    std::ifstream& package_stream = pkgcache->getStream();
    if( !package_stream)
    {
	return Error::E_open_file;
    }

    CommonPkdParser::TagSet* tagset;
    tagset = new InstSrcData_ULPkgTags ();
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
    tagset->clear();

    MIL << "done packages parsing" << endl;

    if( !parse )
	std::cerr << "*** parsing was aborted ***" << std::endl;
    else
	std::cerr << "*** parsed " << count << " packages ***" << std::endl;


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
    tagset->clear();

    MIL << "done packages.<lang> parsing" << endl;

    if( !parse )
	std::cerr << "*** parsing was aborted ***" << std::endl;
    else
	std::cerr << "*** parsed " << count << " language entries ***" << std::endl;


    ///////////////////////////////////////////////////////////////////
    // parse selection data
    ///////////////////////////////////////////////////////////////////

    // --------------------------------
    // setup selections access
    // read <DESCRDIR>/selections to std::list<std::string> selection_names

    std::list<std::string> selection_names;
    Pathname selectionname = descr_dir_r + "/selections";
    MIL << "provideFile (" << selectionname << ")" << endl;
    err = media_r->provideFile (selectionname);
    while (err == Error::E_ok)
    {
	std::ifstream selstream (selectionname.asString().c_str());
	if (!selstream)
	{
	    ERR << "Cant open " << selectionname << ": " << Error::E_open_file << endl;
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

    std::list<PMSelectionPtr> *selectionlist = new (std::list<PMSelectionPtr>);

    count = 0;

    std::list<std::string>::iterator selfile = selection_names.begin();
    std::ifstream selection_stream;

    while (selfile != selection_names.end())
    {
	Pathname selectionname = descr_dir_r + *selfile;
	selection_stream.open (selectionname.asString().c_str());

	if( !selection_stream)
	{
	    ERR << "Cant open " << selectionname << ": " << Error::E_open_file << endl;
	    ++selfile;
	    continue;
	}

	tagset = new InstSrcData_ULSelTags ();
	TagCacheRetrieval *selcache = new TagCacheRetrieval (selectionname);
	parser = selcache->getParser();

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
		    selectionlist->push_back (Tag2Selection (selcache, tagset));
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

	selection_stream.close();
	MIL << "done " << *selfile << " parsing" << endl;
	if (parse)
	{
	    selectionlist->push_back (Tag2Selection (selcache, tagset));
	    count++;
	}
	tagset->clear();

    } // while selfile

    std::cerr << "*** parsed " << count << " selections ***" << std::endl;

    ///////////////////////////////////////////////////////////////////
    // done
    ///////////////////////////////////////////////////////////////////

    if ( !err )
    {
	ndata_r = ndata;
    }
    else
    {
	ERR << "tryGetData failed: " << err << endl;
    }

    return err;
}


