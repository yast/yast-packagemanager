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

  File:		ULPackagesParser.cc

  Author:	Klaus Kaempf <kkaempf@suse.de>
  Maintainer:	Klaus Kaempf <kkaempf@suse.de>

  Purpose:	parse UnitedLinux compatible selection file

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
#include <y2pm/ULPackagesParser.h>
#include <y2pm/PMULPackageDataProvider.h>
#include <y2pm/PMULPackageDataProviderPtr.h>

#include <y2pm/InstSrcError.h>
#include <y2pm/MediaAccess.h>

#include <y2pm/PkgName.h>
#include <y2pm/PkgEdition.h>
#include <y2pm/PkgArch.h>

#include <Y2PM.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : ULPackagesParserPtr
//	CLASS NAME : constULPackagesParserPtr
///////////////////////////////////////////////////////////////////
IMPL_BASE_POINTER(ULPackagesParser);

///////////////////////////////////////////////////////////////////
// public
//	METHOD NAME : ULPackagesParser::ULPackagesParser
//	METHOD TYPE : Constructor
//
//	DESCRIPTION : initialize parser and tag set for selections

ULPackagesParser::ULPackagesParser(const InstSrcPtr source)
    : _source (source)
{
    // initialize tagset

    _tagset.setAllowMultipleSets (true);	// multiple tagsets per file
    _tagset.setAllowUnknownTags (true);		// skip unknown tags

    // for the 'packages' file

    _tagset.addTag ("Pkg", PACKAGE,	TaggedFile::SINGLE, TaggedFile::START);
    _tagset.addTag ("Req", REQUIRES,	TaggedFile::MULTI);
    _tagset.addTag ("Prq", PREREQUIRES, TaggedFile::MULTI);
    _tagset.addTag ("Prv", PROVIDES,	TaggedFile::MULTI);
    _tagset.addTag ("Con", CONFLICTS,	TaggedFile::MULTI);
    _tagset.addTag ("Obs", OBSOLETES,	TaggedFile::MULTI);
    _tagset.addTag ("Rec", RECOMMENDS,	TaggedFile::MULTI);
    _tagset.addTag ("Sug", SUGGESTS,	TaggedFile::MULTI);

    _tagset.addTag ("Loc", LOCATION,	TaggedFile::SINGLE);
    _tagset.addTag ("Siz", SIZE,	TaggedFile::SINGLE);
    _tagset.addTag ("Tim", BUILDTIME,	TaggedFile::SINGLE);
    _tagset.addTag ("Src", SOURCERPM,	TaggedFile::SINGLEPOS);
    _tagset.addTag ("Grp", GROUP,	TaggedFile::SINGLE);
    _tagset.addTag ("Lic", LICENSE,	TaggedFile::SINGLEPOS);
    _tagset.addTag ("Aut", AUTHORS,	TaggedFile::MULTI);
    _tagset.addTag ("Shr", SHAREWITH,	TaggedFile::SINGLE);
    _tagset.addTag ("Key", KEYWORDS,	TaggedFile::MULTI);

    // for the 'packages.<locale>' file

    _tagset.addTag ("Sum", SUMMARY,	TaggedFile::SINGLE);
    _tagset.addTag ("Des", DESCRIPTION, TaggedFile::MULTI);
    _tagset.addTag ("Ins", INSNOTIFY,   TaggedFile::MULTI);
    _tagset.addTag ("Del", DELNOTIFY,   TaggedFile::MULTI);
}


///////////////////////////////////////////////////////////////////
// public
//	METHOD NAME : ULPackagesParser::~ULPackagesParser
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
ULPackagesParser::~ULPackagesParser()
{
    // _tagset destructor will clean up _tagset
}


///////////////////////////////////////////////////////////////////
// private
//	METHOD NAME : allowedArch
//	METHOD TYPE : bool
//
//	DESCRIPTION : check if arch is allowed

bool
ULPackagesParser::allowedArch (const PkgArch& arch) const
{
    for (std::list<PkgArch>::const_iterator it = _allowed_archs.begin();
	 it != _allowed_archs.end(); ++it)
    {
	if (*it == arch)
	    return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////
// private
//	METHOD NAME : ULPackagesParser::fromCache
//	METHOD TYPE : PMError
//
//	DESCRIPTION : pass package data from tagset to dataprovider
//			and add package to _pkgmap

PMError
ULPackagesParser::fromCache (TagCacheRetrievalPtr pkgcache, TagCacheRetrievalPtr localecache)
{
    //---------------------------------------------------------------
    // PACKAGE
    string single ((_tagset.getTagByIndex (PACKAGE))->Data());
    if (single.empty ())
    {
	ERR << "No '=Pkg' value found" << endl;
	return InstSrcError::E_data_bad_packages;
    }

    pkgmaptype::iterator pkgpos = _pkgmap.find (single);
    if (pkgpos != _pkgmap.end())
    {
	ERR << "Duplicate '" << single << "'" << endl;
	return InstSrcError::E_data_bad_packages;
    }

    std::vector<std::string> splitted;

    stringutil::split (single, splitted, " ", false);
//MIL << "-----------------------------" << endl;
//MIL << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;

    PkgArch arch (splitted[3]);

    //---------------------------------------------------------------
    // drop packages not allowed for current architecture
#warning src/nosrc are currently dropped
    if (!allowedArch (arch))
	return InstSrcError::E_ok;

    //---------------------------------------------------------------
    // Pkg -> PMPackage
    PkgName name (splitted[0].c_str());
    PkgEdition edition (splitted[1].c_str(), splitted[2].c_str());

    PMULPackageDataProviderPtr dataprovider ( new PMULPackageDataProvider (pkgcache, localecache));
    PMPackagePtr package (new PMPackage (name, edition, arch, dataprovider, _source));

    //---------------------------------------------------------------
    // enter package to map for faster "=Shr:" (share) and packages.local lookup
    // must save dataprovider too since package does not allow access to it later :-(
    _pkgmap[single] = pair<PMPackagePtr, PMULPackageDataProviderPtr>(package, dataprovider);

    TaggedFile::Tag *tagptr;	// for SET_MULTI()

#define SET_VALUE(tagname,value) \
    do { dataprovider->_attr_##tagname = value; } while (0)
#define GET_TAG(tagname) \
    _tagset.getTagByIndex (tagname)
#define SET_CACHE(tagname) \
    do { tagptr = GET_TAG (tagname); dataprovider->_attr_##tagname = tagptr->Pos(); } while (0)

    //---------------------------------------------------------------
    // pass PMSolvable data directly to instance

    std::list<std::string> pkglist;
    PMSolvable::PkgRelList_type rellist;

    if (pkgcache->retrieveData (GET_TAG(REQUIRES)->Pos(), pkglist))
    {
	rellist = PMSolvable::StringList2PkgRelList (pkglist);
	package->setRequires (rellist);
    }
    pkglist.clear();
    if (pkgcache->retrieveData (GET_TAG(PREREQUIRES)->Pos(), pkglist))
    {
	rellist = PMSolvable::StringList2PkgRelList (pkglist);
	package->addPreRequires (rellist); // rellist is modified after that
    }
    pkglist.clear();
    if (pkgcache->retrieveData (GET_TAG(PROVIDES)->Pos(), pkglist))
    {
	rellist = PMSolvable::StringList2PkgRelList (pkglist);
	package->setProvides (rellist);
    }
    pkglist.clear();
    if (pkgcache->retrieveData (GET_TAG(CONFLICTS)->Pos(), pkglist))
    {
	rellist = PMSolvable::StringList2PkgRelList (pkglist);
	package->setConflicts (rellist);
    }
    pkglist.clear();
    if (pkgcache->retrieveData (GET_TAG(OBSOLETES)->Pos(), pkglist))
    {
	rellist = PMSolvable::StringList2PkgRelList (pkglist);
	package->setObsoletes (rellist);
    }

    SET_CACHE (RECOMMENDS);
    SET_CACHE (SUGGESTS);

    //---------------------------------------------------------------
    // split =Loc: <medianr> <filename>
    // and adapt position for filename accordingly

    tagptr = GET_TAG (LOCATION);
    const char *location = tagptr->Data().c_str();
    const char *locationname = location;

    while (*locationname && isblank (*locationname)) locationname++;
    if ((dataprovider->_attr_MEDIANR = atoi (locationname)) == 0)
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

    //---------------------------------------------------------------
    // SIZE

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

    //---------------------------------------------------------------
    // SHAREWITH, package to share data with
    // FIXME: does not support forwared shared declarations

    string sharewith ((_tagset.getTagByIndex (SHAREWITH))->Data());
    if (!sharewith.empty())
    {
	pkgpos = _pkgmap.find (sharewith);
	if (pkgpos == _pkgmap.end())
	{
	    WAR << "Share package '" << sharewith << "' not found" << endl;
	}
	else
	{
	    // tell my dataprovider to share data with another dataprovider
	    // all data not present in my dataprovider will be taken
	    // from this dataprovider
	    dataprovider->setShared ( pkgpos->second.second );
	}
    }

    return InstSrcError::E_ok;
}


///////////////////////////////////////////////////////////////////
// private
//	METHOD NAME : ULPackagesParser::fromLocale
//	METHOD TYPE : PMPackagePtr
//
//	DESCRIPTION : pass package data from locale tagset to package

PMError
ULPackagesParser::fromLocale ()
{
    //---------------------------------------------------------------
    // PACKAGE.<locale>

    string single ((_tagset.getTagByIndex (PACKAGE))->Data());
    if (single.empty ())
    {
	ERR << "No '=Pkg' value found" << endl;
	return InstSrcError::E_data_bad_packages_lang;
    }

    std::vector<std::string> splitted;
    stringutil::split (single, splitted, " ", false);
//MIL << "Lang for " << splitted[0] << "-" << splitted[1] << "-" << splitted[2] << "." << splitted[3] << endl;

    //---------------------------------------------------------------
    // find corresponding package

    pkgmaptype::iterator pkgpos = _pkgmap.find (single);
    if (pkgpos == _pkgmap.end())
    {
#if 0
	ERR << "No package " << single << endl;
	return InstSrcError::E_data_bad_packages_lang;
#else
	return InstSrcError::E_ok;
#endif
    }
    PMULPackageDataProviderPtr dataprovider = pkgpos->second.second;

    TaggedFile::Tag *tagptr;		// for SET_MULTI()

#define GET_TAG(tagname) \
    _tagset.getTagByIndex (tagname)
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
// public
//
//	METHOD NAME : ULPackagesParser::fromPath
//	METHOD TYPE : PMError
//
//	DESCRIPTION : pass packages data from path to _pkgmap

PMError
ULPackagesParser::fromPath (const Pathname& path, const Pathname& localepath)
{
    std::ifstream pkgstream (path.asString().c_str());

    if (!pkgstream.is_open())
    {
	ERR << "Cant open " << path.asString() << endl;
	return InstSrcError::E_open_file;
    }

    //---------------------------------------------------------------
    // find initial version tag

    // find any initial tag
    TaggedParser::TagType type = _parser.lookupTag (pkgstream);
    if ((type != TaggedParser::SINGLE)
	|| (_parser.currentTag() != "Ver")
	|| (!_parser.currentLocale().empty()))
    {
	ERR << path << ": Initial '=Ver:' tag missing" << endl;
	return InstSrcError::E_data_bad_packages;
    }

    string version = _parser.data();
    if (version != "2.0")
    {
	ERR << path << ": Version '" << version << "' != 2.0" << endl;
	return InstSrcError::E_data_bad_packages;
    }

    //---------------------------------------------------------------
    // assign set repeatedly

    // create a single cache for all packages
    TagCacheRetrievalPtr pkgcache (new TagCacheRetrieval (path));
    TagCacheRetrievalPtr localecache (new TagCacheRetrieval (localepath));

    for (;;)
    {
	TaggedFile::assignstatus status = _tagset.assignSet (_parser, pkgstream);

	if (status == TaggedFile::REJECTED_EOF)
	    break;

	if (status == TaggedFile::ACCEPTED_FULL)
	{
	    PMError err = fromCache (pkgcache, localecache);
	    if (err)
		ERR << path << ":" << _parser.lineNumber() << ":" << err.errstr() << endl;
	}
	else
	{
	    ERR << path << ":" << _parser.lineNumber() << endl;
	    ERR << "Status " << (int)status << ", Last tag read: " << _parser.currentTag();
	    if (!_parser.currentLocale().empty()) ERR << "." << _parser.currentLocale();
	    ERR << endl;
	    return InstSrcError::E_data_bad_packages;
	}
    }

    return PMError::E_ok;
}


///////////////////////////////////////////////////////////////////
// public
//
//	METHOD NAME : ULPackagesParser::fromPathLocale
//	METHOD TYPE : PMError
//
//	DESCRIPTION : pass packages.<locale> data from path to _pkgmap

PMError
ULPackagesParser::fromPathLocale (const Pathname& path)
{
    std::ifstream localestream (path.asString().c_str());

    if (!localestream.is_open())
    {
	ERR << "Cant open " << path.asString() << endl;
	return InstSrcError::E_open_file;
    }

    //---------------------------------------------------------------
    // find initial version tag

    // find any initial tag
    TaggedParser::TagType type = _parser.lookupTag (localestream);
    if ((type != TaggedParser::SINGLE)
	|| (_parser.currentTag() != "Ver")
	|| (!_parser.currentLocale().empty()))
    {
	ERR << path << ": Initial '=Ver:' tag missing" << endl;
	return InstSrcError::E_data_bad_packages_lang;
    }

    string version = _parser.data();
    if (version != "2.0")
    {
	ERR << path << ": Version '" << version << "' != 2.0" << endl;
	return InstSrcError::E_data_bad_packages_lang;
    }

    //---------------------------------------------------------------
    // assign set repeatedly

    PMError err;

    for (;;)
    {
	TaggedFile::assignstatus status = _tagset.assignSet (_parser, localestream);

	if (status == TaggedFile::REJECTED_EOF)
	    break;

	if (status == TaggedFile::ACCEPTED_FULL)
	{
	    err = fromLocale ();
	    if (err)
		ERR << path << ":" << _parser.lineNumber() << ":" << err.errstr() << endl;
	}
	else
	{
	    ERR << path << ":" << _parser.lineNumber() << endl;
	    ERR << "Status " << (int)status << ", Last tag read: " << _parser.currentTag();
	    ERR << endl;
	    return InstSrcError::E_data_bad_packages_lang;
	}
    }

    return PMError::E_ok;
}



///////////////////////////////////////////////////////////////////
// public
//
//	METHOD NAME : ULPackagesParser::fromMediaDir
//	METHOD TYPE : PMError
//
//	DESCRIPTION : pass packages data from media directory
//		to packages list
//		Parses packages and packages.<locale> in given directory

PMError
ULPackagesParser::fromMediaDir (std::list<PMPackagePtr>& packages,
		MediaAccessPtr media_r, const Pathname& descr_dir_r,
		const std::list<PkgArch>& allowed_archs, const LangCode& locale)
{
    PMError err;

    _allowed_archs = allowed_archs;

    _pkgmap.clear();

    // --------------------------------
    // setup packages access

    Pathname filename = descr_dir_r + "/packages";

    err = media_r->provideFile ( filename );
    if ( err )
    {
	ERR << "Media can't provide '" << filename << "' : " << err.errstr() << endl;
	return err;
    }

    bool have_locale = true;
    Pathname localename = filename.extend ("." + (const std::string&)(locale));
    err = media_r->provideFile ( localename );
    if ( err )
    {
	WAR << "Media can't provide '" << localename << "' : " << err.errstr() << endl;

	if (locale == "en")
	{
	    have_locale = false;
	}
	else
	{
	    localename = filename.extend (".en");		// fallback to packages.en
	    err = media_r->provideFile ( localename );
	    {
		have_locale = false;
		WAR << "Media can't provide '" << localename << "' : " << err.errstr() << endl;
	    }
	}
    }

    Pathname fullpath = media_r->localPath (filename);
    Pathname fulllocalepath = media_r->localPath (localename);

    err = fromPath (fullpath, fulllocalepath);
    if (!err
	&& have_locale)
    {
	fromPathLocale (media_r->localPath (localename));
    }

    packages.clear ();

    for (pkgmaptype::iterator it = _pkgmap.begin();
	 it != _pkgmap.end(); ++it)
    {
	packages.push_back (it->second.first);
	_pkgmap.erase (it);
    }

    return err;
}
