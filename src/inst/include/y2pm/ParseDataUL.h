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

  File:       ParseDataUL.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Concrete InstSrcData able to handle UnitedLinux style layout.

/-*/
#ifndef ParseDataUL_h
#define ParseDataUL_h

#include <iosfwd>
#include <fstream>
#include <y2util/CommonPkdParser.h>
#include <y2util/TagCacheRetrieval.h>

#include <y2pm/PMULPackageDataProviderPtr.h>
#include <y2pm/PMULSelectionDataProviderPtr.h>
#include <y2pm/ParseDataULPtr.h>
#include <y2pm/InstSrcData.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMSelection.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : ParseDataUL
/**
 * @short Concrete InstSrcData able to handle UnitedLinux style layout.
 **/
class ParseDataUL : virtual public Rep {
  REP_BODY(ParseDataUL);

    private:

	/**
	 * fill tagset from packages to PMPackage
	 *
	 */
	int Tag2PkgRelList (PMSolvable::PkgRelList_type& pkgrellist, const std::list<std::string>& relationlist);

	/**
	 * fill tagset from packages to PMPackage
	 *
	 */
	PMPackagePtr PkgTag2Package( TagCacheRetrievalPtr pkgcache, CommonPkdParser::TagSet * tagset, const std::list<PMPackagePtr>& packags );

	/**
	 * fill tagset from packages.<lang> to PMPackage
	 *
	 */
	void LangTag2Package( TagCacheRetrievalPtr langcache, const std::list<PMPackagePtr>& packages, CommonPkdParser::TagSet * tagset );

	/**
	 * fill tagset from <name>.sel to PMSelection
	 *
	 */
	PMSelectionPtr Tag2Selection ( PMULSelectionDataProviderPtr dataprovider, CommonPkdParser::TagSet * tagset );


	/*
	 * parse the 'packages' file
	 */
	PMError parsePackages (InstSrcDataPtr & ndata, MediaAccessPtr media_r, const Pathname & descr_dir_r );

	/*
	 * parse the 'packages.<lang>' file
	 */
	PMError parsePackagesLang (InstSrcDataPtr & ndata, MediaAccessPtr media_r, const Pathname & descr_dir_r);

	/*
	 * parse the 'selections' and '*.sel* files
	 */
	PMError parseSelections (InstSrcDataPtr & ndata, MediaAccessPtr media_r, const Pathname & descr_dir_r );

	/*
	 * fill the PMSelectionPtr and PMPackagePtr lists
	 */
	PMError fillSelections (InstSrcDataPtr & ndata);

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;


  public:

    ParseDataUL();

    virtual ~ParseDataUL();

  public:

    /**
     * Any concrete InstSrcData must realize this, as it knows the expected
     * layout on the media. Expect MediaAccessPtr to be open and attached.
     *
     * try to find product/content information on the media
     *
     * Return the InstSrcDescr retrieved from the media via ndescr_r,
     * or NULL and PMError set.
     **/
    PMError tryGetDescr( InstSrcDescrPtr & ndescr_r,
				MediaAccessPtr media_r, const Pathname & produduct_dir_r );

    /**
     * Any concrete InstSrcData must realize this, as it knows the expected
     * layout on the media. Expect MediaAccessPtr to be open and attached.
     *
     * try to find selection/package/patch information on the media
     *
     * Return the InstSrcData retrieved from the media via ndata_r,
     * or NULL and PMError set.
     **/
    PMError tryGetData( InstSrcDataPtr & ndata_r,
				MediaAccessPtr media_r, const Pathname & descr_dir_r );

};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : ParseDataULPkgTags
/**
 * @short provides the tag set for the packages file
 * (to feed the tag parser)
 *
 **/
class ParseDataULPkgTags : public CommonPkdParser::TagSet
{

public:

    enum Tags {
	VERSION,	// general file format version
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

public:
    ParseDataULPkgTags( )
	: TagSet()	{

	CommonPkdParser::Tag* t;
	createTag( "=Ver", VERSION);		// general file format version
	createTag( "=Pkg", PACKAGE);		// name version release arch
	t = createTag( "+Req", REQUIRES);	// list of requires tags
	t->setEndTag("-Req");
	t = createTag( "+Prq", PREREQUIRES);	// list of pre-requires tags
	t->setEndTag("-Prq");
	t = createTag( "+Prv", PROVIDES);	// list of provides tags
	t->setEndTag("-Prv");
	t = createTag( "+Con", CONFLICTS);	// list of conflicts tags
	t->setEndTag("-Con");
	t = createTag( "+Obs", OBSOLETES);	// list of obsoletes tags
	t->setEndTag("-Obs");
	t = createTag( "+Rec", RECOMMENDS);	// list of recommends tags
	t->setEndTag("-Rec");
	t = createTag( "+Sug", SUGGESTS);	// list of suggests tags
	t->setEndTag("-Sug");
	createTag( "=Loc", LOCATION);		// file location
	createTag( "=Siz", SIZE);		// packed and unpacked size
	createTag( "=Tim", BUILDTIME);		// buildtime
	createTag( "=Src", SOURCERPM);		// source package
	createTag( "=Grp", GROUP);		// rpm group
	createTag( "=Lic", LICENSE);		// license
	t = createTag( "+Aut", AUTHORS);	// list of authors
	t->setEndTag("-Aut");
	createTag( "=Shr", SHAREWITH);		// package to share data with
	t = createTag( "+Key", KEYWORDS);	// list of keywords
	t->setEndTag("-Key");
    };

private:

    CommonPkdParser::Tag* createTag( std::string tagName, Tags tagEnum ) {

	CommonPkdParser::Tag* t;
	t = new CommonPkdParser::Tag( tagName, CommonPkdParser::Tag::ACCEPTONCE );
	this->addTag(t);
	addTagByIndex( tagEnum, t );

	return t;
    }

};

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : ParseDataULLangTags
/**
 * @short provides the tag set for the packages.<lang> file
 * (to feed the tag parser)
 *
 **/
class ParseDataULLangTags : public CommonPkdParser::TagSet
{

public:

    enum Tags {
	VERSION,	// general file format version
	PACKAGE,	// name version release arch
	SUMMARY,	// short summary (label)
	DESCRIPTION,	// long description
	INSNOTIFY,	// install notification
	DELNOTIFY,	// delete notification
	NUM_TAGS
    };

public:
    ParseDataULLangTags( )
	: TagSet()	{

	CommonPkdParser::Tag* t;
	createTag( "=Ver", VERSION);		// general file format version
	createTag( "=Pkg", PACKAGE);		// name version release arch
	createTag( "=Sum", SUMMARY);
	t = createTag( "+Des", DESCRIPTION);
	t->setEndTag("-Des");
	t = createTag( "+Ins", INSNOTIFY);
	t->setEndTag("-Ins");
	t = createTag( "+Del", DELNOTIFY);
	t->setEndTag("-Del");
    };

private:

    CommonPkdParser::Tag* createTag( std::string tagName, Tags tagEnum ) {

	CommonPkdParser::Tag* t;
	t = new CommonPkdParser::Tag( tagName, CommonPkdParser::Tag::ACCEPTONCE );
	this->addTag(t);
	addTagByIndex( tagEnum, t );

	return t;
    }

};


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : ParseDataULSelTags
/**
 * @short provides the tag set for a <name>.sel file
 * (to feed the tag parser)
 *
 **/
class ParseDataULSelTags : public CommonPkdParser::TagSet
{

public:

    enum Tags {
	VERSION,	// general file format version
	SELECTION,	// name version release arch
	SUMMARY,	// short summary (label)
	CATEGORY,
	VISIBLE,
	ORDER,		// ordering data
	SUGGESTS,
	RECOMMENDS,
	REQUIRES,
	PROVIDES,
	CONFLICTS,
	OBSOLETES,
	SIZE,
	INSPACKS,
	DELPACKS,
	NUM_TAGS
    };

public:
    ParseDataULSelTags( )
	: TagSet()	{

	CommonPkdParser::Tag* t;
	createTag( "=Ver", VERSION);		// general file format version
	createTag( "=Sel", SELECTION);		// name version release arch
	createTag( "=Sum", SUMMARY);
	createTag( "=Cat", CATEGORY);
	createTag( "=Vis", VISIBLE);
	createTag( "=Ord", ORDER);
	t = createTag( "+Sug", SUGGESTS);	// list of suggests tags
	t->setEndTag("-Sug");
	t = createTag( "+Rec", RECOMMENDS);	// list of recommends tags
	t->setEndTag("-Rec");
	t = createTag( "+Req", REQUIRES);	// list of requires tags
	t->setEndTag("-Req");
	t = createTag( "+Prv", PROVIDES);	// list of provides tags
	t->setEndTag("-Prv");
	t = createTag( "+Con", CONFLICTS);	// list of conflicts tags
	t->setEndTag("-Con");
	t = createTag( "+Obs", OBSOLETES);	// list of obsoletes tags
	t->setEndTag("-Obs");
	createTag( "=Siz", SIZE);		// packed and unpacked size
	t = createTag( "+Ins", INSPACKS);
	t->setEndTag("-Ins");
	t = createTag( "+Del", DELPACKS);
	t->setEndTag("-Del");
    };

private:

    CommonPkdParser::Tag* createTag( std::string tagName, Tags tagEnum ) {

	CommonPkdParser::Tag* t;
	t = new CommonPkdParser::Tag( tagName, CommonPkdParser::Tag::ACCEPTONCE );
	this->addTag(t);
	addTagByIndex( tagEnum, t );

	return t;
    }

};

#endif // ParseDataUL_h
