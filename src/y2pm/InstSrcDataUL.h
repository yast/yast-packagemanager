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
#include <y2pm/InstSrcPtr.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMSelection.h>

#include <y2pm/ULParsePackagesLang.h>
class ULParsePackagesLang;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDataUL
/**
 * @short Concrete InstSrcData able to handle UnitedLinux style layout.
 **/
class InstSrcDataUL : public InstSrcData {
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
	const std::vector<PMYouPatchPtr>& getPatches (void) const { return InstData::getPatches(); }

	/**
	 * Triggered on change of preferredLocale
	 **/
	virtual void preferredLocaleChanged() const;

	/**
	 * On change of preferredLocale reparse new packages.lang
	 **/
	void reparsePackagesLang( ULParsePackagesLang & parser_r ) const;

	/**
	 * On reparse new packages.lang adjust dataprovider
	 **/
	void adjustDpLangData( PMPackagePtr pkg_r, const ULParsePackagesLang::Entry & entry_r ) const;

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;

  public:

    InstSrcDataUL();
    ~InstSrcDataUL();

    /**
     * Write data to cache.
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
				MediaAccessPtr media_r, const Pathname & product_dir_r );

    /**
     * Any concrete InstSrcData must realize this, as it knows the expected
     * layout on the media. Expect MediaAccessPtr to be open and attached.
     *
     * try to find selection/package/patch information on the media
     *
     * Return the InstSrcData retrieved from the media via ndata_r,
     * or NULL and PMError set.
     **/
    static PMError tryGetData( const InstSrcPtr source, InstSrcDataPtr & ndata_r,
			       MediaAccessPtr media_r, Pathname descr_dir_r,
			       const std::list<PkgArch>& allowed_archs, const LangCode& locale);

  private:

    /**
     * If cache is not disabled, assert that it actualy contains data. If still empty,
     * initialize cache from InstSrc's media. Called from tryGetData().
     **/
    static PMError initDataCache( const Pathname & cache_dir_r, const InstSrcPtr source_r );

};

///////////////////////////////////////////////////////////////////

#endif // InstSrcDataUL_h
