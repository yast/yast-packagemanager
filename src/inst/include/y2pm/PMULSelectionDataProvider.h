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

  File:       PMULSelectionDataProvider.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Realized PackageDataProvider for UnitedLinux packages format

/-*/
#ifndef PMULSelectionDataProvider_h
#define PMULSelectionDataProvider_h

#include <iosfwd>
#include <string>
#include <fstream>

#include <y2util/Pathname.h>
#include <y2util/TagParser.h>
#include <y2util/TagCacheRetrieval.h>
#include <y2pm/PMULSelectionDataProviderPtr.h>
#include <y2pm/PMSelectionDataProvider.h>

#include <y2pm/PMSelection.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMULSelectionDataProvider
/**
 * @short Realizes SelectionDataProvider for UnitedLinux packages format
 * @see DataProvider
 **/
class PMULSelectionDataProvider : public PMSelectionDataProvider  {
    REP_BODY(PMULSelectionDataProvider);
    private:

	// save file position and size data for multi line attributes
	TagCacheRetrieval::retrieval_t attr_REQUIRES;
	TagCacheRetrieval::retrieval_t attr_PROVIDES;
	TagCacheRetrieval::retrieval_t attr_CONFLICTS;
	TagCacheRetrieval::retrieval_t attr_OBSOLETES;

	TagCacheRetrieval::retrieval_t attr_INSTALL;
	TagCacheRetrieval::retrieval_t attr_DELETE;

	// save PkgAttributeValue for single line attributes

	PkgAttributeValue attr_NAME;
	PkgAttributeValue attr_VERSION;
	PkgAttributeValue attr_RELEASE;
	PkgAttributeValue attr_ARCH;

	PkgAttributeValue attr_SUMMARY;

	PkgAttributeValue attr_ARCHIVESIZE;
	PkgAttributeValue attr_SIZE;
	PkgAttributeValue attr_CATEGORY;
	PkgAttributeValue attr_VISIBLE;

	// retrieval pointer for *.sel data
	TagCacheRetrieval *_selection_retrieval;

    private:
	// internal attribute value provider
	PkgAttributeValue getValue( constPMObjectPtr obj_r,
				    PMSelection::PMSelectionAttribute attr_r );

    public:

	PMULSelectionDataProvider (TagCacheRetrieval *selection_retrieval = 0);
	virtual ~PMULSelectionDataProvider();

	/**
	 * Object attribute retrieval. (DataProvider interface)
	 * @see PMDataProvider
	 * @see PMObject
	 **/
	PkgAttributeValue getAttributeValue( constPMObjectPtr obj_r,
					     PMObject::PMObjectAttribute attr_r );

	/**
	 * Package attribute retrieval. (SelectionDataProvider interface)
	 * @see PMSelectionDataProvider
	 * @see PMSelection
	 **/
	PkgAttributeValue getAttributeValue( constPMSelectionPtr pkg_r,
					     PMSelection::PMSelectionAttribute attr_r );

	/** inject attribute to cache */
	virtual void setAttributeValue(
	    PMSelectionPtr pkg, PMSelection::PMSelectionAttribute attr,
	    const PkgAttributeValue& value);

	/** inject offset/size to cache */
	virtual void setAttributeValue(
	    PMSelectionPtr pkg, PMSelection::PMSelectionAttribute attr,
	    std::streampos begin, std::streampos end);

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMULSelectionDataProvider_h
