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



namespace PkgTags {
    enum Tags {
	PACKAGE,	// name version release arch
	REQUIRES,	// list of requires tags
	PREREQUIRES,	// list of pre-requires tags
	PROVIDES,	// list of provides tags
	CONFLICTS,	// list of conflicts tags
	OBSOLETES,	// list of obsoletes tags
	RECOMMENDS,	// list of recommends tags
	SUGGESTS,	// list of suggests tags
	LOCATION,	// file location
	SIZE,		// packed and unpacked size
	BUILDTIME,	// buildtime
	SOURCERPM,	// source package
	GROUP,		// rpm group
	LICENSE,	// license
	AUTHORS,	// list of authors
	SHAREWITH,	// package to share data with
	KEYWORDS,	// list of keywords
	NUM_TAGS
    };
};

namespace PkgLangTags {
    enum Tags {
	PACKAGE,	// name version release arch
	SUMMARY,	// short summary (label)
	DESCRIPTION,	// long description
	INSNOTIFY,	// install notification
	DELNOTIFY,	// delete notification
	NUM_TAGS
    };
};

namespace SelTags {
    enum Tags {
	SELECTION,	// name version release arch
	SUMMARY,	// short summary (label)
	CATEGORY,
	VISIBLE,
	ORDER,		// ordering data
	RECOMMENDS,
	SUGGESTS,
	REQUIRES,
	PROVIDES,
	CONFLICTS,
	OBSOLETES,
	SIZE,
	INSPACKS,
	DELPACKS,
	NUM_TAGS
    };
};


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
	string name = *pkgIt;
	std::list<PMPackagePtr> matches;

	// check for alternative package
	string::size_type spacepos = name.find_first_of (" ");
	if (spacepos != string::npos)
	{
	    string wantedname = name.substr (0, spacepos);
	    matches = InstData::findPackages (all_packages, wantedname);
	    if (matches.size() == 0)
	    {
		string::size_type startpos = name.find_first_of ("(", spacepos);
		if (startpos != string::npos)
		{
		    string::size_type endpos = name.find_first_of (")", startpos);
		    if (endpos != string::npos)
		    {
			string alternative = name.substr (startpos+1, endpos-startpos-1);
			matches = InstData::findPackages (all_packages, alternative);
		    }
		}
	    }
	}
	else
	{
#warning lookup in arch order
	    matches = InstData::findPackages (all_packages, name);
	}

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
InstSrcDataUL::PkgTag2Package ( TagCacheRetrievalPtr pkgcache,
				TaggedFile::TagSet& tagset,
				const std::list<PMPackagePtr>& packages )
{
    // PACKAGE
    string single ((tagset.getTagByIndex (PkgTags::PACKAGE))->Data());
    if (single.empty ())
    {
	ERR << "No '=Pkg' value found" << endl;
	return PMPackagePtr();
    }

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

    TaggedFile::Tag *tagptr;	// for SET_MULTI()

#define SET_VALUE(tagname,value) \
    do { dataprovider->_attr_##tagname = value; } while (0)
#define GET_TAG(tagname) \
    tagset.getTagByIndex (PkgTags::tagname)
#define SET_CACHE(tagname) \
    do { tagptr = GET_TAG (tagname); dataprovider->_attr_##tagname = tagptr->Pos(); } while (0)

    // pass PMSolvable data directly to instance

    PMSolvable::PkgRelList_type pkgrellist;
    std::list<std::string> pkglist;
    if (pkgcache->retrieveData (GET_TAG(REQUIRES)->Pos(), pkglist)
	&& Tag2PkgRelList (pkgrellist, pkglist))
    {
	package->setRequires (pkgrellist);
    }
    pkglist.clear();
    if (pkgcache->retrieveData (GET_TAG(PREREQUIRES)->Pos(), pkglist)
	&& Tag2PkgRelList (pkgrellist, pkglist))
    {
	package->addPreRequires (pkgrellist); // pkgrellist is modified after that
    }
    pkglist.clear();
    if (pkgcache->retrieveData (GET_TAG(PROVIDES)->Pos(), pkglist)
	&& Tag2PkgRelList (pkgrellist, pkglist))
    {
	package->setProvides (pkgrellist);
    }
    pkglist.clear();
    if (pkgcache->retrieveData (GET_TAG(CONFLICTS)->Pos(), pkglist)
	&& Tag2PkgRelList (pkgrellist, pkglist))
    {
	package->setConflicts (pkgrellist);
    }
    pkglist.clear();
    if (pkgcache->retrieveData (GET_TAG(OBSOLETES)->Pos(), pkglist)
	&& Tag2PkgRelList (pkgrellist, pkglist))
    {
	package->setObsoletes (pkgrellist);
    }

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
	dataprovider->_attr_LOCATION = TagRetrievalPos (tagptr->posDataStart() + (std::streampos)(locationname-location), tagptr->posDataEnd());
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

    string sharewith ((tagset.getTagByIndex (PkgTags::SHAREWITH))->Data());
    if (!sharewith.empty())
    {
//MIL << "Share " << package->name() << "-" << package->version() << "-" << package->release() << "." << package->arch() << endl;
	stringutil::split (sharewith, splitted, " ", false);
//MIL << "With " << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;
	// findPackages (name, arch, version, release)
	const std::list<PMPackagePtr> candidates = InstData::findPackages (packages, splitted[0], splitted[3], splitted[1], splitted[2]);

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

PMError
InstSrcDataUL::LangTag2Package (TagCacheRetrievalPtr langcache, const std::list<PMPackagePtr>& packages, TaggedFile::TagSet& tagset)
{
    // PACKAGE
    string single ((tagset.getTagByIndex (PkgLangTags::PACKAGE))->Data());
    if (single.empty ())
    {
	ERR << "No '=Pkg' value found" << endl;
	return InstSrcError::E_data_bad_packages_lang;
    }

    std::vector<std::string> splitted;
    stringutil::split (single, splitted, " ", false);
//MIL << "Lang for " << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;

    // findPackages (name, arch, version, release)
    const std::list<PMPackagePtr> candidates = InstData::findPackages (packages, splitted[0], splitted[3], splitted[1], splitted[2]);

    if (candidates.size() < 1)
    {
	ERR << "No package " << single << endl;
	return InstSrcError::E_data_bad_packages_lang;
    }

    PMPackagePtr package = candidates.front();
    PMULPackageDataProviderPtr dataprovider = package->dataProvider();
    dataprovider->setLangCache (langcache);

    TaggedFile::Tag *tagptr;		// for SET_MULTI()

#define GET_TAG(tagname) \
    tagset.getTagByIndex (PkgLangTags::tagname)
#define SET_CACHE(tagname) \
    do { tagptr = GET_TAG (tagname); dataprovider->_attr_##tagname = tagptr->Pos(); } while (0)

    SET_CACHE (SUMMARY);
    SET_CACHE (DESCRIPTION);
    SET_CACHE (INSNOTIFY);
    SET_CACHE (DELNOTIFY);

#undef GET_TAG
#undef SET_CACHE

    return PMError::E_ok;
}

///////////////////////////////////////////////////////////////////
// PRIVATE
//
//	METHOD NAME : InstSrcDataUL::Tag2Selection
//	METHOD TYPE : PMSelectionPtr
//
//	DESCRIPTION : pass selection data from tagset to PMSelection

PMSelectionPtr
InstSrcDataUL::Tag2Selection (PMULSelectionDataProviderPtr dataprovider, TaggedFile::TagSet& tagset)
{
#define GET_TAG(tagname) \
    tagset.getTagByIndex(SelTags::tagname)

    string single (GET_TAG(SELECTION)->Data());
    if (single.empty ())
    {
	ERR << "No '=Sel' value found" << endl;
	return PMSelectionPtr();
    }

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
    TagCacheRetrievalPtr selcache = dataprovider->getCacheRetrieval();
    if (!selcache)
    {
	ERR << "No selcache!" << endl;
	return selection;
    }
    TaggedFile::Tag *tagptr;	// for SET_CACHE
#define SET_VALUE(tagname,value) \
    do { dataprovider->_attr_##tagname = value; } while (0)
#define SET_CACHE(tagname) \
    do { tagptr = GET_TAG (tagname); \
	 dataprovider->_attr_##tagname = tagptr->Pos(); } while (0)
#define SET_LCACHE(tagname) \
    do { tagptr = GET_TAG (tagname); \
	 dataprovider->_attr_##tagname = tagptr->PosMap(); } while (0)

    PMSolvable::PkgRelList_type pkgrellist;
    std::list<std::string> sellist;
    if (selcache->retrieveData (GET_TAG(REQUIRES)->Pos(), sellist)
	&& Tag2PkgRelList (pkgrellist, sellist))
    {
	selection->setRequires (pkgrellist);
    }
    sellist.clear();
    if (selcache->retrieveData (GET_TAG(PROVIDES)->Pos(), sellist)
	&& Tag2PkgRelList (pkgrellist, sellist))
    {
	selection->setProvides (pkgrellist);
    }
    sellist.clear();
    if (selcache->retrieveData (GET_TAG(CONFLICTS)->Pos(), sellist)
	&& Tag2PkgRelList (pkgrellist, sellist))
    {
	selection->setConflicts (pkgrellist);
    }
    sellist.clear();
    if (selcache->retrieveData (GET_TAG(OBSOLETES)->Pos(), sellist)
	&& Tag2PkgRelList (pkgrellist, sellist))
    {
	selection->setObsoletes (pkgrellist);
    }

    SET_LCACHE (SUMMARY);
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

    SET_LCACHE (INSPACKS);
    SET_LCACHE (DELPACKS);

#undef SET_VALUE
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

    TaggedParser::TagType type;
    TaggedParser parser;

    // find initial version tag

    // find any initial tag
    type = parser.lookupTag (package_stream);

    if ((type != TaggedParser::SINGLE)
	|| (parser.currentTag() != "Ver")
	|| (!parser.currentLocale().empty()))
    {
	ERR << "Initial '=Ver:' tag missing" << endl;
	return InstSrcError::E_data_bad_packages;
    }

    string version = parser.data();
    if (version != "2.0")
    {
	ERR << "Version '" << version << "' != 2.0" << endl;
	return InstSrcError::E_data_bad_packages;
    }

    TaggedFile::TagSet tagset;

    tagset.addTag ("Pkg", PkgTags::PACKAGE,	TaggedFile::SINGLE, TaggedFile::START);
    tagset.addTag ("Req", PkgTags::REQUIRES,	TaggedFile::MULTI);
    tagset.addTag ("Prq", PkgTags::PREREQUIRES, TaggedFile::MULTI);
    tagset.addTag ("Prv", PkgTags::PROVIDES,	TaggedFile::MULTI);
    tagset.addTag ("Con", PkgTags::CONFLICTS,	TaggedFile::MULTI);
    tagset.addTag ("Obs", PkgTags::OBSOLETES,	TaggedFile::MULTI);
    tagset.addTag ("Rec", PkgTags::RECOMMENDS,	TaggedFile::MULTI);
    tagset.addTag ("Sug", PkgTags::SUGGESTS,	TaggedFile::MULTI);

    tagset.addTag ("Loc", PkgTags::LOCATION,	TaggedFile::SINGLE);
    tagset.addTag ("Siz", PkgTags::SIZE,	TaggedFile::SINGLE);
    tagset.addTag ("Tim", PkgTags::BUILDTIME,	TaggedFile::SINGLE);
    tagset.addTag ("Src", PkgTags::SOURCERPM,	TaggedFile::SINGLEPOS);
    tagset.addTag ("Grp", PkgTags::GROUP,	TaggedFile::SINGLE);
    tagset.addTag ("Lic", PkgTags::LICENSE,	TaggedFile::SINGLEPOS);
    tagset.addTag ("Aut", PkgTags::AUTHORS,	TaggedFile::MULTI);
    tagset.addTag ("Shr", PkgTags::SHAREWITH,	TaggedFile::SINGLE);
    tagset.addTag ("Key", PkgTags::KEYWORDS,	TaggedFile::MULTI);

    MIL << "start packages parsing" << endl;

    // now repeatedly parse complete tag sets

    for (;;)
    {
	// assign set
	TaggedFile::assignstatus status = tagset.assignSet (parser, package_stream);

	if ((status == TaggedFile::REJECTED_NOMATCH)
	    && (package_stream.eof()))
	{
	    break;
	}

	if (status != TaggedFile::ACCEPTED_FULL)
	{
	    ERR << "Error in packages file at " << parser.lineNumber() << endl;
	    ERR << "Last tag read: " << parser.currentTag();
	    if (!parser.currentLocale().empty())
		ERR << "." << parser.currentLocale();
	    ERR << endl;
	    return InstSrcError::E_data_bad_packages;
	}

	PMPackagePtr package = PkgTag2Package (pkgcache, tagset, packages);
	if (package == NULL)
	{
	    ERR << "Error in packages file at " << parser.lineNumber() << endl;
	    return InstSrcError::E_data_bad_packages;
	}
	packages.push_back (package);
	count++;
    }

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
    TagCacheRetrievalPtr langcache ( new TagCacheRetrieval( fullpath ));

    ///////////////////////////////////////////////////////////////////
    // parse language data
    ///////////////////////////////////////////////////////////////////

    std::ifstream language_stream (fullpath.asString().c_str());
    if( !language_stream.is_open())
    {
	ERR << "Can't open " << fullpath << endl;
	return InstSrcError::E_open_file;
    }

    TaggedParser::TagType type;
    TaggedParser parser;

    // find initial version tag

    // find any initial tag
    type = parser.lookupTag (language_stream);

    if ((type != TaggedParser::SINGLE)
	|| (parser.currentTag() != "Ver")
	|| (!parser.currentLocale().empty()))
    {
	ERR << filename << ": Initial '=Ver:' tag missing" << endl;
	return InstSrcError::E_data_bad_packages_lang;
    }

    string version = parser.data();
    if (version != "2.0")
    {
	ERR << filename << ": Version '" << version << "' != 2.0" << endl;
	return InstSrcError::E_data_bad_packages_lang;
    }

    TaggedFile::TagSet tagset;
    tagset.addTag ("Pkg", PkgLangTags::PACKAGE,	    TaggedFile::SINGLE, TaggedFile::START);
    tagset.addTag ("Sum", PkgLangTags::SUMMARY,	    TaggedFile::SINGLE, TaggedFile::ALLOWLOCALE);
    tagset.addTag ("Des", PkgLangTags::DESCRIPTION, TaggedFile::MULTI,  TaggedFile::ALLOWLOCALE);
    tagset.addTag ("Ins", PkgLangTags::INSNOTIFY,   TaggedFile::MULTI,  TaggedFile::ALLOWLOCALE);
    tagset.addTag ("Del", PkgLangTags::DELNOTIFY,   TaggedFile::MULTI,  TaggedFile::ALLOWLOCALE);

    MIL << "start packages.<lang> parsing" << endl;
    count = 0;

    // now repeatedly parse complete tag sets

    for (;;)
    {
	// assign set

	TaggedFile::assignstatus status = tagset.assignSet (parser, language_stream);

	if ((status == TaggedFile::REJECTED_NOMATCH)
	    && (language_stream.eof()))
	{
	    break;
	}

	if (status != TaggedFile::ACCEPTED_FULL)
	{
	    ERR << filename << ":" << parser.lineNumber() << endl;
	    ERR << "Last tag read: " << parser.currentTag();
	    if (!parser.currentLocale().empty())
		ERR << "." << parser.currentLocale();
	    ERR << endl;
	    return InstSrcError::E_data_bad_packages_lang;
	}
	PMError err = LangTag2Package (langcache, packages, tagset);
	if (err != PMError::E_ok)
	    return err;
	count++;
    }

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

    err = media_r->provideFile ( filename );			// try <descr>/selections first
    if ( err )
    {
	WAR << "No selections cache found" << endl;		// failed, build own cache
	std::list<std::string> all_names;
	err = media_r->dirInfo (all_names, descr_dir_r, false);		// get all names not starting with '.'
	if (err)
	{
	    ERR << "dirInfo failed for '" << descr_dir_r << "' " << err.errstr() << endl;
	    return err;
	}
	for (std::list<std::string>::iterator fileIt = all_names.begin();
	     fileIt != all_names.end(); ++fileIt)
	{
	    if (fileIt->find (".sel") != string::npos)		// filter "*.sel"
		selection_names.push_back (*fileIt);
	}
    }
    else
    {
	Pathname fullpath = media_r->localPath (filename);
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

    std::ifstream selection_stream;

    TaggedParser::TagType type;
    TaggedParser parser;
    TaggedFile::TagSet tagset;

    tagset.addTag ("Sel", SelTags::SELECTION, TaggedFile::SINGLE, TaggedFile::START);
    tagset.addTag ("Sum", SelTags::SUMMARY,   TaggedFile::SINGLE, TaggedFile::ALLOWLOCALE);
    tagset.addTag ("Req", SelTags::REQUIRES,  TaggedFile::MULTI);
    tagset.addTag ("Prv", SelTags::PROVIDES,  TaggedFile::MULTI);
    tagset.addTag ("Obs", SelTags::OBSOLETES, TaggedFile::MULTI);
    tagset.addTag ("Con", SelTags::CONFLICTS, TaggedFile::MULTI);
    tagset.addTag ("Rec", SelTags::RECOMMENDS,TaggedFile::MULTI);
    tagset.addTag ("Sug", SelTags::SUGGESTS,  TaggedFile::MULTI);

    tagset.addTag ("Siz", SelTags::SIZE,      TaggedFile::SINGLE);
    tagset.addTag ("Cat", SelTags::CATEGORY,  TaggedFile::SINGLE);
    tagset.addTag ("Vis", SelTags::VISIBLE,   TaggedFile::SINGLE);
    tagset.addTag ("Ord", SelTags::ORDER,     TaggedFile::SINGLE);
    tagset.addTag ("Ins", SelTags::INSPACKS,  TaggedFile::MULTI, TaggedFile::ALLOWLOCALE);
    tagset.addTag ("Del", SelTags::DELPACKS,  TaggedFile::MULTI, TaggedFile::ALLOWLOCALE);

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
	TaggedParser parser;

	// find initial version tag

	// find any initial tag
	type = parser.lookupTag (selection_stream);
	if ((type != TaggedParser::SINGLE)
	    || (parser.currentTag() != "Ver")
	    || (!parser.currentLocale().empty()))
	{
	    ERR << filename << ": Initial '=Ver:' tag missing" << endl;
	    return InstSrcError::E_data_bad_packages_lang;
	}

	string version = parser.data();
	if (version != "3.0")
	{
	    ERR << filename << ": Version '" << version << "' != 2.0" << endl;
	    return InstSrcError::E_data_bad_packages_lang;
	}

	// assign set

	tagset.restart ();
	TaggedFile::assignstatus status = tagset.assignSet (parser, selection_stream);

	if ((status == TaggedFile::REJECTED_NOMATCH)
	     && (selection_stream.eof())
	    || (status == TaggedFile::ACCEPTED_FULL))
	{
	    PMSelectionPtr selection = Tag2Selection (dataprovider, tagset);
	    if (selection)
	    {
		selections.push_back (selection);
	    }
	}
	else
	{
	    ERR << filename << ":" << parser.lineNumber() << endl;
	    ERR << "Status " << (int)status << ", Last tag read: " << parser.currentTag();
	    if (!parser.currentLocale().empty())
		ERR << "." << parser.currentLocale();
	    ERR << endl;
	}

	selection_stream.clear();
	// close own copy of selection_stream
	selection_stream.close();
    } // for ()

    MIL << "*** parsed " << selections.size() << " selections ***" << std::endl;

    return PMError::E_ok;
}


//-------------------------------------------------------------------
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
//	MIL << "fillSelection (" << (*selIt)->name() << ")" << endl;
	PMULSelectionDataProviderPtr selDp = (*selIt)->dataProvider();

	selDp->_ptrs_attr_SUGGESTS = lookupSelections (all_selections, (*selIt)->suggests());
	selDp->_ptrs_attr_RECOMMENDS = lookupSelections (all_selections, (*selIt)->recommends());

	for (TaggedFile::Tag::posmaptype::iterator tagIt = selDp->_attr_INSPACKS.begin();
	     tagIt != selDp->_attr_INSPACKS.end(); ++tagIt)
	{
	    // get language packages
	    std::list<std::string> inspackages = (*selIt)->inspacks (tagIt->first);
	    if (!inspackages.empty())
	    {
		selDp->_ptrs_attr_INSPACKS[tagIt->first] = lookupPackages (all_packages, inspackages);
	    }
	}
	for (TaggedFile::Tag::posmaptype::iterator tagIt = selDp->_attr_DELPACKS.begin();
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

    // parse <DESCRDIR>/selections and <DESCRDIR>/ *.sel
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
