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
#include <y2util/LangCode.h>
#include <y2util/TaggedFile.h>
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
	std::list<PMPackagePtr>   _packages;

    public:

	/**
	 * generate PMSelection objects for each selection on the source
	 * @return list of PMSelectionPtr on this source
	 */
	const std::list<PMSelectionPtr>& getSelections() const { return _selections; }

	/**
	 * generate PMPackage objects for each Item on the source/target
	 * @return list of PMPackagePtr on this source
	 * */
	const std::list<PMPackagePtr>& getPackages() const { return _packages; }

	/**
	 * generate PMYouPatch objects for each patch on the target
	 * @return list of PMYouPatchPtr on this target
	 * Return empty list as we do not hold Patches
	 */
	const std::list<PMYouPatchPtr>& getPatches (void) const { return InstData::getPatches(); }
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
	static PMPackagePtr PkgTag2Package( TagCacheRetrievalPtr pkgcache, TaggedFile::TagSet& tagset, const std::list<PMPackagePtr>& packags );

	/**
	 * fill tagset from packages.<lang> to PMPackage
	 *
	 */
	static PMError LangTag2Package( TagCacheRetrievalPtr langcache, const std::list<PMPackagePtr>& packages, TaggedFile::TagSet& tagset );

	/**
	 * fill tagset from <name>.sel to PMSelection
	 *
	 */
	static PMSelectionPtr Tag2Selection ( PMULSelectionDataProviderPtr dataprovider, TaggedFile::TagSet& tagset );

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

  private:

    Pathname _descr_dir; // provided by media or cache
    Pathname _data_dir;

    static const LangCode _fallback_langcode; // fix en
    LangCode              _default_langcode;  // the one we're encouraged to use
    LangCode              _langcode;          // the one we actually use

    std::list<std::string> _selection_files;

    Pathname packagesFile()     const { return _descr_dir + "packages"; }
    Pathname packagesLangFile() const { return (_descr_dir + "packages.").extend(_langcode); }
    Pathname packagesDuFile()   const { return _descr_dir + "packages.DU"; };

    Pathname selectionsFile()   const { return _descr_dir + "selections"; }
    Pathname selectionFile( const std::string & name_r ) const { return _descr_dir + name_r; }

  protected:

    /**
     * @see InstSrcData#loadObjects
     **/
    virtual PMError loadObjects();

  public:

    /**
     * Write data to cache, if necessary
     **/
    virtual PMError writeCache( const Pathname & cache_dir_r ) const;

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

#endif // InstSrcDataUL_h
