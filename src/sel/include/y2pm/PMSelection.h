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

  File:       PMSelection.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Defines the software Selection object.

/-*/
#ifndef PMSelection_h
#define PMSelection_h

#include <iosfwd>
#include <string>

#include <y2pm/PMSelectionDataProviderPtr.h>
#include <y2pm/PMSelectionPtr.h>

#include <y2pm/PMObject.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelection
/**
 * The software selection
 **/
class PMSelection : virtual public Rep, public PMObject {
  REP_BODY(PMSelection);

  public:

    /**
     * Attributes provided by PMSelection
     **/
    enum PMSelectionAttribute {
      SEL_ATTR_BEGIN = PMOBJ_NUM_ATTRIBUTES,
      // ATTR_WAHTEVER_IS_FIRST = SEL_ATTR_BEGIN,
      // last entry:
      SEL_NUM_ATTRIBUTES
    };

    /**
     * Get attribute name as string.
     **/
    std::string getAttributeName(PMSelectionAttribute attr) const;

    /**
     * Access to base class getAttributeName
     **/
    PMObject::getAttributeName;

    /**
     * Get attribute value
     **/
    PkgAttributeValue getAttributeValue(PMSelectionAttribute attr) const;

    /**
     * Access to base class getAttributeValue
     **/
    PMObject::getAttributeValue;

  protected:

    PMSelectionDataProviderPtr _dataProvider;

  protected:

    /**
     * Provide DataProvider access to the underlying Object
     **/
    virtual PMDataProviderPtr dataProvider() const { return _dataProvider; }

  public:

    PMSelection( const PkgName &    name_r,
		 const PkgEdition & edition_r );

    virtual ~PMSelection();

  public:

    /** assign a data provider
     * @param dataprovider the dataprovider
     * */
    void setDataProvider(PMSelectionDataProviderPtr dataprovider)
    { _dataProvider = dataprovider; }

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMSelection_h
