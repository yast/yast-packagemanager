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

   File:       PMObject.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMObject_h
#define PMObject_h

#include <iosfwd>
#include <string>

#include <y2pm/PMObjectPtr.h>
#include <y2pm/PMSolvable.h>

#include <y2pm/PMSelectablePtr.h>
#include <y2pm/PMDataProviderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMObject
/**
 * @short Interface class for stuff common to all objects (Packages, Selections,..)
 **/
class PMObject : virtual public Rep, public PMSolvable {
  REP_BODY(PMObject);

  private:

    friend class PMManager;
    friend class PMSelectable;

    PMSelectablePtr _selectable;

  protected:

    /**
     * Access to the concrete Objects DataProvider for attribute retrieval.
     **/
    virtual PMDataProviderPtr dataProvider() const = 0;

  public:

    /**
     * Attributes provided by PMObject
     **/
    enum PMObjectAttribute {
	PMOBJ_ATTR_BEGIN = PMSLV_NUM_ATTRIBUTES,
	ATTR_SUMMARY = PMOBJ_ATTR_BEGIN,
	ATTR_DESCRIPTION,
	ATTR_SIZE, // installed size
	// last entry:
	PMOBJ_NUM_ATTRIBUTES
    };

    /**
     * Get attribute name as string.
     **/
    std::string getAttributeName(PMObjectAttribute attr) const;

    /**
     * Access to base class getAttributeName
     **/
    PMSolvable::getAttributeName;

    /**
     * Get attribute value
     **/
    PkgAttributeValue getAttributeValue(PMObjectAttribute attr) const;

    /**
     * Access to base class getAttributeValue
     **/
    PMSolvable::getAttributeValue;

  public:

    PMObject( const PkgName &    name_r,
	      const PkgEdition & edition_r );

    virtual ~PMObject();

  public:

    ///////////////////////////////////////////////////////////////////
    // Shortcuts that tell about the Objects status within
    // the managing Selectable.
    ///////////////////////////////////////////////////////////////////

    /**
     * Return true if the Object is liked to a Selectable. This is true
     * if the Object is under controll of a Manager.
     **/
    bool hasSelectable() const { return( _selectable != 0 ); }

    /**
     * Return the managing Selectable.
     **/
    PMSelectablePtr getSelectable() const { return _selectable; }

    /**
     * Return true if this is the Selectables InstalledObj (on InstTartget).
     * The one actually installed on the target system.
     **/
    bool isInstalledObj() const;

    /**
     * Return true if this is the Selectables CandidateObj (from an InstSrc).
     * Among all available Objects, this is considered to be the best choice
     * for an installation.
     **/
    bool isCandidateObj() const;

    /**
     * Short for neither isInstalledObj nor isCandidateObj.
     * Thus the Object is in the Selectables list of available Objects (from an InstSrc),
     * but not considered to be the best choice for an installation.
     **/
    bool isAvailableOnly() const;

  public:

    /**
     * Used by ostream::operator<< to print some debug lines
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMObject_h
