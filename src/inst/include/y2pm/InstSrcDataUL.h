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

  File:       InstSrcDataUL.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Concrete InstSrcData able to handle UnitedLinux style layout.

/-*/
#ifndef InstSrcDataUL_h
#define InstSrcDataUL_h

#include <iosfwd>
#include <fstream>
#include <y2util/CommonPkdParser.h>
#include <y2util/TagCacheRetrieval.h>

#include <y2pm/PMULPackageDataProviderPtr.h>
#include <y2pm/PMULSelectionDataProviderPtr.h>
#include <y2pm/InstSrcDataULPtr.h>
#include <y2pm/InstSrcData.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMSelection.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDataUL
/**
 * @short Concrete InstSrcData able to handle UnitedLinux style layout.
 **/
class InstSrcDataUL : virtual public Rep, public InstSrcData {
    REP_BODY(InstSrcDataUL);

    private:
	/**
	 * actual data for this InstSrc
	 */
	std::list<PMSelectionPtr> _selections;
	std::list<PMPackagePtr> _packages;
	std::list<PMYouPatchPtr> _patches;

	//--------------------------------------------------------------------
	// InstData interface  

    protected:
	//-----------------------------
	// cache file handling
	/**
	 * write media content data to cache file
	 * @param pathname of corresponding InstSrcDescr cache file
	 * @return pathname of written cache
	 * writes content cache data to an ascii file
	 */
	const Pathname writeCache (const Pathname &descrpathname);

    public:

	/**
	 * generate PMSelection objects for each selection on the source
	 * @return list of PMSelectionPtr on this source
	 */
	const std::list<PMSelectionPtr>& getSelections (void) const;

	/**
	 * generate PMPackage objects for each Item on the source/target
	 * @return list of PMPackagePtr on this source
	 * */
	const std::list<PMPackagePtr>& getPackages (void) const;

	/**
	 * generate PMSolvable objects for each patch on the source
	 * @return list of PMSolvablePtr on this source
	 */
	const std::list<PMYouPatchPtr>& getPatches (void) const;

	std::ostream & dumpOn( std::ostream & str ) const;

    private:

	//-----------------------------------------------------------
	// static functions for tryGetDescr and tryGetData
	//-----------------------------------------------------------

	static std::list<PMSelectionPtr> lookupSelections (const std::list<PMSelectionPtr> all_selections, const std::list<std::string>& selections);
	static std::list<PMPackagePtr> lookupPackages (const std::list<PMPackagePtr> all_packages, const std::list<std::string>& packages);

	/**
	 * fill tagset from packages to PMPackage
	 *
	 */
	static int Tag2PkgRelList (PMSolvable::PkgRelList_type& pkgrellist, const std::list<std::string>& relationlist);

	/**
	 * fill tagset from packages to PMPackage
	 *
	 */
	static PMPackagePtr PkgTag2Package( TagCacheRetrievalPtr pkgcache, CommonPkdParser::TagSet * tagset, const std::list<PMPackagePtr>& packags );

	/**
	 * fill tagset from packages.<lang> to PMPackage
	 *
	 */
	static void LangTag2Package( TagCacheRetrievalPtr langcache, const std::list<PMPackagePtr>& packages, CommonPkdParser::TagSet * tagset );

	/**
	 * fill tagset from <name>.sel to PMSelection
	 *
	 */
	static PMSelectionPtr Tag2Selection ( PMULSelectionDataProviderPtr dataprovider, CommonPkdParser::TagSet * tagset );

	/*
	 * parse the 'packages' file
	 */
	static PMError parsePackages (std::list<PMPackagePtr>& packages, MediaAccessPtr media_r, const Pathname & descr_dir_r );

	/*
	 * parse the 'packages.<lang>' file
	 */
	static PMError parsePackagesLang (std::list<PMPackagePtr>& packages, MediaAccessPtr media_r, const Pathname & descr_dir_r);

	/*
	 * parse the 'selections' and '*.sel* files
	 */
	static PMError parseSelections (std::list<PMSelectionPtr>& selections, MediaAccessPtr media_r, const Pathname & descr_dir_r );

	/*
	 * fill the PMSelectionPtr and PMPackagePtr lists
	 */
	static PMError fillSelections (std::list<PMSelectionPtr>& all_selections, std::list<PMPackagePtr>& all_packages);

	/**
	 * read media.X/media file
	 *
	 * @param product_dir base directory
	 * @param media_r MediaAccessPtr
	 * @param number the X in media.X
	 * @param vendor where to store vendor
	 * @param id where to store id
	 * @param count where to store count
	 * */
	static PMError readMediaFile(const Pathname& product_dir, MediaAccessPtr media_r, unsigned number, std::string& vendor, std::string& id, unsigned& count);

    public:

	/**
	 * default error class
	 **/
	typedef InstSrcError Error;

    public:

	InstSrcDataUL();
	~InstSrcDataUL();

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
	static PMError tryGetDescr( InstSrcDescrPtr & ndescr_r,
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
	static PMError tryGetData( InstSrcDataPtr & ndata_r,
				MediaAccessPtr media_r, const Pathname & descr_dir_r );

};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDataULPkgTags
/**
 * @short provides the tag set for the packages file
 * (to feed the tag parser)
 *
 **/
class InstSrcDataULPkgTags : public CommonPkdParser::TagSet
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
    InstSrcDataULPkgTags( )
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
//	CLASS NAME : InstSrcDataULLangTags
/**
 * @short provides the tag set for the packages.<lang> file
 * (to feed the tag parser)
 *
 **/
class InstSrcDataULLangTags : public CommonPkdParser::TagSet
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
    InstSrcDataULLangTags( )
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
//	CLASS NAME : InstSrcDataULSelTags
/**
 * @short provides the tag set for a <name>.sel file
 * (to feed the tag parser)
 *
 **/
class InstSrcDataULSelTags : public CommonPkdParser::TagSet
{

public:

    enum Tags {
	VERSION,	// general file format version
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

public:
    InstSrcDataULSelTags( )
	: TagSet()	{

	CommonPkdParser::Tag* t;
	createTag( "=Ver", VERSION);		// general file format version
	createTag( "=Sel", SELECTION);		// name version release arch
	createTag( "=Sum", SUMMARY);
	createTag( "=Cat", CATEGORY);
	createTag( "=Vis", VISIBLE);
	createTag( "=Ord", ORDER);
	t = createTag( "+Rec", RECOMMENDS);	// list of recommends tags
	t->setEndTag("-Rec");
	t = createTag( "+Sug", SUGGESTS);	// list of suggests tags
	t->setEndTag("-Sug");
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

#endif // InstSrcDataUL_h
