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

#include <y2util/FSize.h>

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
	ATTR_INSNOTIFY,			// notification on install
	ATTR_DELNOTIFY,			// notification on delete
	ATTR_SIZE, // installed size
	// last entry:
	PMOBJ_NUM_ATTRIBUTES
    };

    /**
     * access functions for attributes
     */
    virtual const std::string summary() const = 0;
    virtual const std::list<std::string> description() const = 0;
    virtual const std::list<std::string> insnotify() const = 0;
    virtual const std::list<std::string> delnotify() const = 0;
    virtual const FSize size() const = 0;

    /**
     * Get PMObject attribute name as string.
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
	      const PkgEdition & edition_r,
	      const PkgArch &    arch_r );

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
     * Not to be confused with hasInstalledObj() !
     **/
    bool isInstalledObj() const;

    /**
     * Return true if this is the Selectables CandidateObj (from an InstSrc).
     * Among all available Objects, this is considered to be the best choice
     * for an installation.
     * Not to be confused with hasCandidateObj() !
     **/
    bool isCandidateObj() const;

    /**
     * Short for neither isInstalledObj nor isCandidateObj.
     * Thus the Object is in the Selectables list of available Objects (from an InstSrc),
     * but not considered to be the best choice for an installation.
     **/
    bool isAvailableOnly() const;

    /**
     * Convenience method: Retrieve the installed instance of this
     * selectable. This may be a brother of this object or this object itself
     * or 0 (if there is no installed instance or if this object doesn't have a
     * selectable - in which case something has gone wrong badly anyway).
     **/
    PMObjectPtr getInstalledObj() const;
    
    /**
     * Convenience method: Retrieve the candidate instance of this
     * selectable. This may be a brother of this object or this object itself
     * or 0 (if there is no candidate instance or if this object doesn't have a
     * selectable - in which case something has gone wrong badly anyway).
     **/
    PMObjectPtr getCandidateObj() const;

    /**
     * Convenience method: Check if there is any installed instance of this
     * selectable - this instance or any of its brothers.
     * Not to be confused with isInstalledObj() !
     **/
    bool hasInstalledObj() const;
    
    /**
     * Convenience method: Check if there is any candidate instance of this
     * selectable - this instance or any of its brothers.
     * Not to be confused with isInstalledObj() !
     **/
    bool hasCandidateObj() const;
    

  public:

    /**
     * Used by ostream::operator<< to print some debug lines
     **/
    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMObject_h
