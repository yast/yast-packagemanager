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

#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMSelectionPtr.h>
#include <y2pm/InstSrcDataUL.h>
#include <y2pm/PMULPackageDataProvider.h>
#include <y2pm/PMULPackageDataProviderPtr.h>
#include <y2pm/PMULSelectionDataProvider.h>
#include <y2pm/PMULSelectionDataProviderPtr.h>

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

///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : PMULSelectionDataProvider::lookupSelections
//	METHOD TYPE : std::list<PMPackagePtr>
//
//	DESCRIPTION : lookup selection names to PMSelectionPtr
//
std::list<PMSelectionPtr>
InstSrcDataUL::lookupSelections (const std::list<PMSelectionPtr> all_selections, const std::list<std::string>& selections)
{
    std::list<PMSelectionPtr> selection_ptrs;

    for (std::list<std::string>::const_iterator selIt = selections.begin();
	 selIt != selections.end(); ++selIt)
    {
	std::list<PMSelectionPtr> matches = InstData::findSelections (all_selections, *selIt);
	// silently ignore selections not found
	if (matches.size() > 0)
	{
	    selection_ptrs.push_back (matches.front());
	}
    }

    return selection_ptrs;
}

///////////////////////////////////////////////////////////////////
// private
//
//	METHOD NAME : PMULSelectionDataProvider::lookupPackages
//	METHOD TYPE : std::list<PMPackagePtr>
//
//	DESCRIPTION : lookup package names to PMPackagePtr
//
std::list<PMPackagePtr>
InstSrcDataUL::lookupPackages (const std::list<PMPackagePtr> all_packages, const std::list<std::string>& packages)
{
    std::list<PMPackagePtr> package_ptrs;

    for (std::list<std::string>::const_iterator pkgIt = packages.begin();
	 pkgIt != packages.end(); ++pkgIt)
    {
	std::list<PMPackagePtr> matches = InstData::findPackages (all_packages, *pkgIt);
	// silently ignore packages not found
	if (matches.size() > 0)
	{
	    package_ptrs.push_back (matches.front());
	}
    }

    return package_ptrs;
}


///////////////////////////////////////////////////////////////////
// PRIVATE
//
//	METHOD NAME : InstSrcDataUL::Tag2PkgRelList
//	METHOD TYPE : int
//
//	DESCRIPTION : convert list of strings (denoting dependencies)
//		      to PMSolvable::PkgRelList_type&
//		      return number of dependencies found

int
InstSrcDataUL::Tag2PkgRelList (PMSolvable::PkgRelList_type& pkgrellist, const std::list<std::string>& relationlist)
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
//	METHOD NAME : InstSrcDataUL::PkgTag2Package
//	METHOD TYPE : PMPackagePtr
//
//	DESCRIPTION : pass packages data from tagset to pgkcache
//		 * Single line values are passed by value
//		 * Multi line values are passed by file position (on-demand read)
//
//		 * packagelist is used for finding shared packages

PMPackagePtr
InstSrcDataUL::PkgTag2Package( TagCacheRetrievalPtr pkgcache,
				CommonPkdParser::TagSet * tagset,
				const std::list<PMPackagePtr>& packages )
{
    // PACKAGE
    string single ((tagset->getTagByIndex(InstSrcDataULPkgTags::PACKAGE))->Data());

    std::vector<std::string> splitted;

    stringutil::split (single, splitted, " ", false);
//MIL << "-----------------------------" << endl;
//MIL << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;

    // Pkg -> PMPackage
    PkgName name (splitted[0]);
    PkgEdition edition (splitted[1].c_str(), splitted[2].c_str());
    PkgArch arch (splitted[3]);

    PMULPackageDataProviderPtr dataprovider ( new PMULPackageDataProvider (pkgcache));
    PMPackagePtr package( new PMPackage (name, edition, arch, dataprovider));
    dataprovider->setPackage (package);

    CommonPkdParser::Tag *tagptr;	// for SET_MULTI()

#define SET_VALUE(tagname,value) \
    do { dataprovider->_attr_##tagname = value; } while (0)
#define SET_POS(tagname,start,stop) \
    do { dataprovider->_attr_##tagname.set (start, stop); } while (0)
#define GET_TAG(tagname) \
    tagset->getTagByIndex(InstSrcDataULPkgTags::tagname)
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

    // split =Loc: <medianr> <filename>
    // and adapt position for filename accordingly
    tagptr = GET_TAG (LOCATION);
    const char *location = tagptr->Data().c_str();
    const char *locationname = location;
    while (*locationname && isblank (*locationname)) locationname++;
    if ((dataprovider->_attr_MEDIANR = atoi (locationname)) <= 0)
    {
	WAR << "** suspiciuous media nr '" << locationname << "'" << endl;
    }
    while (*locationname && isdigit (*locationname)) locationname++;
    while (*locationname && isblank (*locationname)) locationname++;
    if (*locationname)
    {
	SET_POS (LOCATION, tagptr->posDataStart() + (std::streampos)(locationname-location), tagptr->posDataEnd());
    }
    else
    {
	ERR << "No location for " << package->name() << endl;
    }

    stringutil::split ((GET_TAG(SIZE))->Data(), splitted, " ", false);
    if (splitted.size() <= 0)
    {
	ERR << "No archivesize for " << package->name() << endl;
    }
    else
    {
	SET_VALUE (ARCHIVESIZE, FSize (atoll(splitted[0].c_str())));
	if (splitted.size() < 2)
        {
	    ERR << "No size for " << package->name() << endl;
	}
	else
	{
	    SET_VALUE (SIZE, FSize (atoll(splitted[1].c_str())));
	}
    }
    SET_VALUE (BUILDTIME, Date (GET_TAG(BUILDTIME)->Data()));
    SET_CACHE (SOURCERPM);

    SET_VALUE (GROUP, Y2PM::packageManager().addRpmGroup (GET_TAG(GROUP)->Data()));
    SET_CACHE (LICENSE);
    SET_CACHE (AUTHORS);
    SET_CACHE (KEYWORDS);

#undef SET_VALUE
#undef SET_POS
#undef GET_TAG
#undef SET_CACHE

    // SHAREWITH, package to share data with
    // FIXME: does not support forwared shared declarations

    string sharewith ((tagset->getTagByIndex(InstSrcDataULPkgTags::SHAREWITH))->Data());
    if (!sharewith.empty())
    {
//MIL << "Share " << package->name() << "-" << package->version() << "-" << package->release() << "." << package->arch() << endl;
	stringutil::split (sharewith, splitted, " ", false);
//MIL << "With " << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;
	const std::list<PMPackagePtr> candidates = InstData::findPackages (packages, splitted[0], splitted[1], splitted[2], splitted[3]);

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
//	METHOD NAME : InstSrcDataUL::LangTag2Package
//	METHOD TYPE : void
//
//	DESCRIPTION : * pass packages.lang data from tagset to langcache
//		 * Single line values are passed by value
//		 * Multi line values are passed by file position (on-demand read)

void
InstSrcDataUL::LangTag2Package (TagCacheRetrievalPtr langcache, const std::list<PMPackagePtr>& packages, CommonPkdParser::TagSet * tagset)
{
    // PACKAGE
    string single ((tagset->getTagByIndex(InstSrcDataULLangTags::PACKAGE))->Data());

    std::vector<std::string> splitted;
    stringutil::split (single, splitted, " ", false);
//MIL << "Lang for " << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;

    const std::list<PMPackagePtr> candidates = InstData::findPackages (packages, splitted[0], splitted[1], splitted[2], splitted[3]);

    if (candidates.size() < 1)
    {
	ERR << "No package " << single << endl;
	return;
    }

    PMPackagePtr package = candidates.front();
    PMULPackageDataProviderPtr dataprovider = package->dataProvider();
    dataprovider->setLangCache (langcache);

    CommonPkdParser::Tag *tagptr;		// for SET_MULTI()

#define SET_POS(tagname,start,stop) \
    do { dataprovider->_attr_##tagname.set (start, stop); } while (0)
#define GET_TAG(tagname) \
    tagset->getTagByIndex(InstSrcDataULLangTags::tagname)
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
//	METHOD NAME : InstSrcDataUL::Tag2Selection
//	METHOD TYPE : PMSelectionPtr
//
//	DESCRIPTION : pass selection data from tagset to PMSelection

PMSelectionPtr
InstSrcDataUL::Tag2Selection (PMULSelectionDataProviderPtr dataprovider, CommonPkdParser::TagSet * tagset)
{
    // SELECTION
    string single ((tagset->getTagByIndex(InstSrcDataULSelTags::SELECTION))->Data());

    std::vector<std::string> splitted;

    stringutil::split (single, splitted, " ", false);
    while (splitted.size() < 4)
	splitted.push_back("");

//MIL << "-----------------------------" << endl;
//MIL << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;

    // Pkg -> PMSelection
    PkgName name (splitted[0]);
    PkgEdition edition (splitted[1].c_str(), splitted[2].c_str());
    PkgArch arch (splitted[3]);

    PMSelectionPtr selection( new PMSelection (name, edition, arch, dataprovider));
    dataprovider->setSelection (selection);

    CommonPkdParser::Tag *tagptr;	// for SET_MULTI()

#define SET_VALUE(tagname,value) \
    do { dataprovider->_attr_##tagname = value; } while (0)
#define SET_LVALUE(tagname,value,lang) \
    do { dataprovider->_attr_##tagname[lang] = value; } while (0)
#define SET_POS(tagname,start,stop) \
    do { dataprovider->_attr_##tagname.set (start, stop); } while (0)
#define SET_LPOS(tagname,start,stop,lang) \
    do { dataprovider->_attr_##tagname[lang].set (start, stop); } while (0)
#define GET_TAG(tagname) \
    tagset->getTagByIndex(InstSrcDataULSelTags::tagname)
#define SET_CACHE(tagname) \
    do { tagptr = GET_TAG (tagname); \
	 SET_POS (tagname, tagptr->posDataStart(), tagptr->posDataEnd()); } while (0)
#define SET_LCACHE(tagname,lang) \
    do { tagptr = GET_TAG (tagname); \
	 SET_LPOS (tagname, tagptr->posDataStart(), tagptr->posDataEnd(), lang); } while (0)

    PMSolvable::PkgRelList_type pkgrellist;
    if (Tag2PkgRelList (pkgrellist, (GET_TAG(REQUIRES))->MultiData()))
	selection->setRequires (pkgrellist);
    if (Tag2PkgRelList (pkgrellist, (GET_TAG(PROVIDES))->MultiData()))
	selection->setProvides (pkgrellist);
    if (Tag2PkgRelList (pkgrellist, (GET_TAG(CONFLICTS))->MultiData()))
	selection->setConflicts (pkgrellist);
    if (Tag2PkgRelList (pkgrellist, (GET_TAG(OBSOLETES))->MultiData()))
	selection->setObsoletes (pkgrellist);

    SET_LCACHE (SUMMARY, "");
    std::string category = GET_TAG(CATEGORY)->Data();
    SET_VALUE (CATEGORY, category);
    SET_VALUE (ISBASE, (strncmp (category.c_str(), "base", 4) == 0));
    SET_VALUE (VISIBLE, GET_TAG(VISIBLE)->Data() == "true");
    SET_CACHE (RECOMMENDS);
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

    SET_LCACHE (INSPACKS, "");
#warning Fix language specific inspacks
    tagptr = GET_TAG (INSLANGPACKS);
    SET_LPOS (INSPACKS, tagptr->posDataStart(), tagptr->posDataEnd(), Y2PM::getPreferredLocale());
    SET_LCACHE (DELPACKS, "");
    SET_VALUE (ORDER, (GET_TAG(ORDER))->Data());

#undef SET_VALUE
#undef SET_LVALUE
#undef SET_POS
#undef SET_LPOS
#undef GET_TAG
#undef SET_CACHE
#undef SET_LCACHE

    return selection;
}


PMError
InstSrcDataUL::parsePackages (std::list<PMPackagePtr>& packages,
		MediaAccessPtr media_r, const Pathname& descr_dir_r )
{
    PMError err;
    int count = 0;
    std::string tagstr;

    ///////////////////////////////////////////////////////////////////
    // parse package data
    ///////////////////////////////////////////////////////////////////

    // --------------------------------
    // setup packages access

    Pathname filename = descr_dir_r + "/packages";

    err = media_r->provideFile ( filename );
    if ( err ) {
	ERR << "Media can't provide '" << filename << "' : " << err.errstr() << endl;
	return err;
    }

    Pathname fullpath = media_r->localPath (filename);
    MIL << "fopen(" << fullpath << ")" << endl;
    TagCacheRetrievalPtr pkgcache ( new TagCacheRetrieval( fullpath ));

    // --------------------------------
    // read package data

    std::ifstream package_stream (fullpath.asString().c_str());
    if( !package_stream.is_open())
    {
	ERR << "Can't open " << fullpath << endl;
	return InstSrcError::E_open_file;
    }

    CommonPkdParser::TagSet* tagset = new InstSrcDataULPkgTags ();
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
		    err = PMError::E_ok;
		    break;
		case CommonPkdParser::Tag::REJECTED_NOMATCH:
		    repeatassign = false;
		    break;
		case CommonPkdParser::Tag::REJECTED_FULL:
		    packages.push_back (PkgTag2Package( pkgcache, tagset, packages ));
		    count++;
		    tagset->clear();
		    repeatassign = true;
		    err = PMError::E_ok;
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

	packages.push_back (PkgTag2Package( pkgcache, tagset, packages ));
	count++;
    }

    delete tagset;

    if( !parse )
	ERR << "*** parsing packages was aborted ***" << endl;
    else
	MIL << "*** parsed " << count << " packages ***" << std::endl;

    // implict stream close

    return PMError::E_ok;
}


PMError
InstSrcDataUL::parsePackagesLang (std::list<PMPackagePtr>& packages,
		MediaAccessPtr media_r, const Pathname& descr_dir_r)
{
    PMError err;
    int count = 0;
    std::string tagstr;

    // --------------------------------
    // setup packages.<lang> access
    // check for packages.<lang> file

#warning Check lang setup. Should use SrcDescr?
    char * lang = getenv ("LANG");
    string langext( "." );
    langext += ( lang ? lang : "en" );
    if ( langext.length() > 3 )
	langext.erase( 3 );

    Pathname filename = descr_dir_r + "/packages";
    filename = filename.extend( langext );

    err = media_r->provideFile ( filename );

    Pathname fullpath = media_r->localPath (filename);
    if ( err )
    {
	ERR << "Media can't provide '" << fullpath << "' " << err.errstr() << endl;
	return err;
    }


    MIL << "fopen(" << fullpath << ")" << endl;
    TagCacheRetrievalPtr langcache ( new TagCacheRetrieval( filename ));

    ///////////////////////////////////////////////////////////////////
    // parse language data
    ///////////////////////////////////////////////////////////////////

    std::ifstream language_stream (fullpath.asString().c_str());
    if( !language_stream.is_open())
    {
	ERR << "Can't open " << fullpath << endl;
	return InstSrcError::E_open_file;
    }

    CommonPkdParser::TagSet* tagset = new InstSrcDataULLangTags ();
    bool parse = true;
    TagParser & parser = langcache->getParser();

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
		    LangTag2Package (langcache, packages, tagset);
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
	LangTag2Package (langcache, packages, tagset);
	count++;
    }

    delete tagset;

    if( !parse )
	ERR << "*** parsing packages.<lang> was aborted ***" << std::endl;
    else
	MIL << "*** parsed " << count << " packages.<lang> entries ***" << std::endl;

    // implicit stream close

    return PMError::E_ok;
}


PMError
InstSrcDataUL::parseSelections (std::list<PMSelectionPtr>& selections,
		MediaAccessPtr media_r, const Pathname& descr_dir_r )
{
    PMError err;

    ///////////////////////////////////////////////////////////////////
    // parse selection data
    ///////////////////////////////////////////////////////////////////

    // --------------------------------
    // setup selections access
    // read <DESCRDIR>/selections to std::list<std::string> selection_names

    std::list<std::string> selection_names;

    Pathname filename = descr_dir_r + "/selections";

    err = media_r->provideFile ( filename );

    Pathname fullpath = media_r->localPath (filename);
    if ( err )
    {
	WAR << "Media can't provide '" << fullpath << "' " << err.errstr() << endl;
    }
    else
    {
	while ( true )
	{
	    std::ifstream selstream (fullpath.asString().c_str());
	    if (!selstream)
	    {
		ERR << "Cant open " << fullpath << ": " << Error::E_open_file << endl;
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
		{
		    continue;
		}
		selection_names.push_back (lbuf);
	    }
	    break;
	}
	// implicit close of selstream
    }
    MIL << "*** Expecting " << selection_names.size() << " selections ***" << endl;

    int count = 0;
    std::ifstream selection_stream;

    std::string tagstr;
    CommonPkdParser::TagSet* tagset = new InstSrcDataULSelTags ();
    bool parse = true;

    for (std::list<std::string>::iterator selfile = selection_names.begin();
	 selfile != selection_names.end();
	 ++selfile)
    {

	Pathname filename = descr_dir_r + *selfile;
	err = media_r->provideFile ( filename );

	Pathname fullpath = media_r->localPath (filename);
	selection_stream.open (fullpath.asString().c_str());

	if (!selection_stream.is_open())
	{
	    ERR << "Cant open " << fullpath.asString() << endl;
	    continue;
	}

	MIL << "Reading " << fullpath.asString() << endl;

	PMULSelectionDataProviderPtr dataprovider ( new PMULSelectionDataProvider (fullpath));
	TagParser& parser = dataprovider->getParser();

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
		    selections.push_back (Tag2Selection (dataprovider, tagset));
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
	    selections.push_back (Tag2Selection (dataprovider, tagset));
	    count++;
	}
	tagset->clear();

	selection_stream.clear();
	// close own copy of selection_stream
	selection_stream.close();
	MIL << "done " << *selfile << " parsing" << endl;
    } // for ()

    delete tagset;
    MIL << "*** parsed " << selections.size() << " selections ***" << std::endl;

    return PMError::E_ok;
}


// fill selections with caching data
// set up lists of PMSelectionPtr and PMPackagePtr
// for suggests, inspacks, delpacks

PMError
InstSrcDataUL::fillSelections (std::list<PMSelectionPtr>& all_selections, std::list<PMPackagePtr>& all_packages)
{
    PMError err;

    for (std::list<PMSelectionPtr>::iterator selIt = all_selections.begin();
	 selIt != all_selections.end(); ++selIt)
    {
	MIL << "fillSelection (" << (*selIt)->name() << ")" << endl;
	PMULSelectionDataProviderPtr selDp = (*selIt)->dataProvider();

	selDp->_ptrs_attr_SUGGESTS = lookupSelections (all_selections, (*selIt)->suggests());
	selDp->_ptrs_attr_RECOMMENDS = lookupSelections (all_selections, (*selIt)->recommends());

	for (map <std::string,TagCacheRetrievalPos>::iterator tagIt = selDp->_attr_INSPACKS.begin();
	     tagIt != selDp->_attr_INSPACKS.end(); ++tagIt)
	{
	    // get language packages
	    std::list<std::string> inspackages = (*selIt)->inspacks (tagIt->first);
	    if (!inspackages.empty())
	    {
		selDp->_ptrs_attr_INSPACKS[tagIt->first] = lookupPackages (all_packages, inspackages);
	    }
	}
	for (map <std::string,TagCacheRetrievalPos>::iterator tagIt = selDp->_attr_DELPACKS.begin();
	     tagIt != selDp->_attr_DELPACKS.end(); ++tagIt)
	{
	    // get language packages
	    std::list<std::string> delpackages = (*selIt)->delpacks (tagIt->first);
	    if (!delpackages.empty())
	    {
		selDp->_ptrs_attr_DELPACKS[tagIt->first] = lookupPackages (all_packages, delpackages);
	    }
	}
    }
    return err;
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

PMError InstSrcDataUL::readMediaFile(const Pathname& product_dir, MediaAccessPtr media_r, unsigned number, std::string& vendor, std::string& id, unsigned& count)
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
PMError InstSrcDataUL::tryGetData( InstSrcDataPtr& ndata_r,
				    MediaAccessPtr media_r, const Pathname & descr_dir_r )
{
    MIL << "InstSrcDataUL::tryGetData(" << descr_dir_r << ")" << endl;

    ndata_r = 0;
    PMError err;

    //-----------------------------------------------------
    // create instance of _own_ class
    //-----------------------------------------------------

    InstSrcDataULPtr ndata( new InstSrcDataUL() );

    // parse <DESCRDIR>/packages
    if (!parsePackages (ndata->_packages, media_r, descr_dir_r))
    {
	// parse <DESCRDIR>/packages.<lang>
	parsePackagesLang (ndata->_packages, media_r, descr_dir_r);
    }

    // parse <DESCRDIR>/selections and <DESCRDIR>/*.sel
    parseSelections (ndata->_selections, media_r, descr_dir_r);

    // fill selections with caching data
    // set up lists of PMSelectionPtr and PMPackagePtr
    // for suggests, inspacks, delpacks
    fillSelections (ndata->_selections, ndata->_packages);

    ///////////////////////////////////////////////////////////////////
    // done
    ///////////////////////////////////////////////////////////////////

    if ( !err )
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
	ERR << "tryGetData failed: " << err << endl;
    }

    return err;
}

//---------------------------------------------------------------------...
// protected

/**
 * write media content data to cache file
 * @param pathname of corresponding InstSrcDescr cache file
 * @return pathname of written cache
 * writes content cache data to an ascii file
 */

const Pathname
InstSrcDataUL::writeCache (const Pathname &descrpathname)
{
    return InstSrcData::writeCache (descrpathname);
}

//---------------------------------------------------------------------...
// public

/**
 * generate PMSelection objects for each Item on the source
 * @return list of PMSelectionPtr on this source
 * */
const std::list<PMSelectionPtr>&
InstSrcDataUL::getSelections() const
{
    return _selections;
}

/**
 * generate PMPackage objects for each Item on the source
 * @return list of PMPackagePtr on this source
 * */
const std::list<PMPackagePtr>&
InstSrcDataUL::getPackages() const
{
    MIL << "InstSrcDataUL::getPackages(" << _packages.size() << ")" << endl;
    return _packages;
}

/**
 * generate PMSolvable objects for each patch on the target
 * @return list of PMSolvablePtr on this target
 */
const std::list<PMYouPatchPtr>&
InstSrcDataUL::getPatches (void) const
{
    MIL << "InstSrcDataUL::getPatches()" << endl;
    return _patches;
}


std::ostream &
InstSrcDataUL::dumpOn( std::ostream & str ) const
{
    Rep::dumpOn( str );
    return str;
}




InstSrcDataULSelTags::InstSrcDataULSelTags( )
    : TagSet()
{
	const std::string& preferred_locale = (const std::string &)(Y2PM::getPreferredLocale());
	CommonPkdParser::Tag* t;
	createTag( "=Ver", InstSrcDataULSelTags::VERSION);		// general file format version
	createTag( "=Sel", InstSrcDataULSelTags::SELECTION);		// name version release arch
	t = createTag( "=Sum", InstSrcDataULSelTags::SUMMARY);
	t->setType(CommonPkdParser::Tag::ACCEPTPREFERREDLOCALE);
	t->setPreferredLocale(preferred_locale);
	createTag( "=Cat", InstSrcDataULSelTags::CATEGORY);
	createTag( "=Vis", InstSrcDataULSelTags::VISIBLE);
	createTag( "=Ord", InstSrcDataULSelTags::ORDER);
	t = createTag( "+Rec", InstSrcDataULSelTags::RECOMMENDS);	// list of recommends tags
	t->setEndTag("-Rec");
	t = createTag( "+Sug", InstSrcDataULSelTags::SUGGESTS);	// list of suggests tags
	t->setEndTag("-Sug");
	t = createTag( "+Req", InstSrcDataULSelTags::REQUIRES);	// list of requires tags
	t->setEndTag("-Req");
	t = createTag( "+Prv", InstSrcDataULSelTags::PROVIDES);	// list of provides tags
	t->setEndTag("-Prv");
	t = createTag( "+Con", InstSrcDataULSelTags::CONFLICTS);	// list of conflicts tags
	t->setEndTag("-Con");
	t = createTag( "+Obs", InstSrcDataULSelTags::OBSOLETES);	// list of obsoletes tags
	t->setEndTag("-Obs");
	createTag( "=Siz", InstSrcDataULSelTags::SIZE);		// packed and unpacked size
	t = createTag( "+Ins", InstSrcDataULSelTags::INSPACKS);
	t->setEndTag("-Ins");
	if (!preferred_locale.empty())
	{
	    t = createTag (std::string ("+Ins" + preferred_locale).c_str(), INSLANGPACKS);
	    t->setEndTag (std::string ("-Ins" + preferred_locale).c_str());
	}
	t = createTag( "+Del", InstSrcDataULSelTags::DELPACKS);
	t->setEndTag("-Del");
}

