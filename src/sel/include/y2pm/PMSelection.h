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
#include <y2pm/PkgArch.h>

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
      ATTR_CATEGORY = SEL_ATTR_BEGIN,
      ATTR_VISIBLE,
      ATTR_INSTALL,
      ATTR_DELETE,
      ATTR_ARCHIVESIZE,
      // last entry:
      PMSEL_NUM_ATTRIBUTES
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
		 const PkgEdition & edition_r,
		 const PkgArch &    arch_r,
		 PMSelectionDataProviderPtr dataProvider_r );

    virtual ~PMSelection();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMSelection_h
