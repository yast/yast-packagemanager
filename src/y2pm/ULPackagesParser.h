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

  File:		ULPackagesParser.h

  Author:	Klaus Kaempf <kkaempf@suse.de>
  Maintainer:	Klaus Kaempf <kkaempf@suse.de>

  Purpose: Parse UnitedLinux compatible selection files

/-*/
#ifndef ULPackagesParser_h
#define ULPackagesParser_h

#include <string>
#include <list>
#include <map>
#include <utility>
#include <fstream>

#include <y2util/Pathname.h>
#include <y2util/LangCode.h>
#include <y2util/TaggedParser.h>
#include <y2util/TaggedFile.h>
#include <y2util/TagCacheRetrievalPtr.h>

#include <y2pm/PMError.h>
#include <y2pm/MediaAccessPtr.h>

#include <y2pm/PkgArch.h>

#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMULPackageDataProviderPtr.h>
#include <y2pm/InstSrcPtr.h>

#include <y2pm/ULPackagesParserPtr.h>

class ULPackagesParser : public CountedRep {
    private:
	// the source
	const InstSrcPtr _source;
		
	// tag ids for the TaggedParser
	enum Tags {
	    PACKAGE,	// name version release arch
	    REQUIRES,	// list of requires tags
	    PREREQUIRES,// list of pre-requires tags
	    PROVIDES,	// list of provides tags
	    CONFLICTS,	// list of conflicts tags
	    OBSOLETES,	// list of obsoletes tags
	    RECOMMENDS,	// list of recommends tags
	    SUGGESTS,	// list of suggests tags
	    LOCATION,	// file location
	    SIZE,	// packed and unpacked size
	    BUILDTIME,	// buildtime
	    SOURCERPM,	// source package
	    GROUP,	// rpm group
	    LICENSE,	// license
	    AUTHORS,	// list of authors
	    SHAREWITH,	// package to share data with
	    KEYWORDS,	// list of keywords

	    // packages.<locale>

	    SUMMARY,	// short summary (label)
	    DESCRIPTION,// long description
	    INSNOTIFY,	// install notification
	    DELNOTIFY,	// delete notification

	    // packages.DU
	    DU,		// disk usage data

	    NUM_TAGS
	};


	// our parser
	TaggedParser _parser;
	// our set of tags, initialized in constructor
	TaggedFile::TagSet _tagset;

	// list of currently allowed archs
	std::list<PkgArch> _allowed_archs;

	// map of our currently known packages
	// key == "package version release architecture" as used in "=Pkg:"
	// and "=Shr:" tags
	typedef std::map <std::string, std::pair<PMPackagePtr, PMULPackageDataProviderPtr> > pkgmaptype;
	pkgmaptype _pkgmap;

	// look if arch is allowed
	bool allowedArch (const PkgArch& arch) const;

	// create package from tagset and cache and add to _pkgmap  
	PMError fromCache (TagCacheRetrievalPtr pkgcache, TagCacheRetrievalPtr localecache, TagCacheRetrievalPtr ducache);

	// augment package data (_pkgmap) with data from packages.<locale> cache
	PMError fromLocale ();

	// augment package data (_pkgmap) with data from packages.DU cache
	PMError fromDU ();

	// parse 'packages' from path to _pkgmap
	// localepath is only for initialization of the dataprovider
	// fromPathLocale() does the parsing of the locale file
	PMError fromPath (const Pathname& path, const Pathname& localepath, const Pathname& dupath);

	// augment _pkgmap data from 'packages.<locale>' at path
	PMError fromPathLocale (const Pathname& path);

	// augment _pkgmap data from 'packages.DU' at path
	PMError fromPathDU (const Pathname& path);

    public:
	ULPackagesParser (const InstSrcPtr source);
	~ULPackagesParser();

	/**
	 * parse packages + packages.<locale> file found below
	 * descr_dir_r of media_r to packages.
	 * discard packages architectures not in allowed_archs
	 */
	PMError fromMediaDir (std::list<PMPackagePtr>& packages,
		MediaAccessPtr media_r, const Pathname& descr_dir_r,
		const std::list<PkgArch>& allowed_archs, const LangCode& locale);

};

#endif //ULPackagesParser_h
