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

  File:       PMYouPatch.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Defines the YouPatch object.

/-*/
#ifndef PMYouPatch_h
#define PMYouPatch_h

#include <iosfwd>
#include <string>

#include <y2pm/PMYouPatchDataProviderPtr.h>
#include <y2pm/PMYouPatchPtr.h>

#include <y2pm/PMObject.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatch
/**
 * The Package.
 **/
class PMYouPatch : virtual public Rep, public PMObject {
  REP_BODY(PMYouPatch);

  public:

    /**
     * Attributes provided by PMYouPatch
     **/
    enum PMYouPatchAttribute {
      YOU_ATTR_BEGIN = PMOBJ_NUM_ATTRIBUTES,
      // ATTR_WAHTEVER_IS_FIRST = YOU_ATTR_BEGIN,
      // last entry:
      YOU_NUM_ATTRIBUTES
    };

    /**
     * Get attribute name as string.
     **/
    std::string getAttributeName(PMYouPatchAttribute attr) const;

    /**
     * Access to base class getAttributeName
     **/
    PMObject::getAttributeName;

    /**
     * Get attribute value
     **/
    PkgAttributeValue getAttributeValue(PMYouPatchAttribute attr) const;

    /**
     * Access to base class getAttributeValue
     **/
    PMObject::getAttributeValue;

  protected:

    PMYouPatchDataProviderPtr _dataProvider;

  protected:

    /**
     * Provide DataProvider access to the underlying Object
     **/
    virtual PMDataProviderPtr dataProvider() const { return _dataProvider; }

  public:

    PMYouPatch( const PkgName &    name_r,
		const PkgEdition & edition_r );

    virtual ~PMYouPatch();

  public:

    /** assign a data provider
     * @param dataprovider the dataprovider
     * */
    void setDataProvider(PMYouPatchDataProviderPtr dataprovider)
    { _dataProvider = dataprovider; }

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatch_h
