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

  public:

    enum PMObjectAttribute {
      ATTR_SUMMARY = 0,
      ATTR_DESCRIPTION,
      ATTR_SIZE, // installed size

      PMOBJ_NUM_ATTRIBUTES
    };

  public:

    PMObject( const PkgName &    name_r,
	      const PkgEdition & edition_r );

    virtual ~PMObject();

  public:

    ///////////////////////////////////////////////////////////////////
    // Object attribute retrieval
    ///////////////////////////////////////////////////////////////////

    /** get attributes like Summary, Description, Group etc.
     *
     * @param attr Attribute number
     * @return Attribute value
     * */
    virtual std::string getAttributeValue(PMObjectAttribute attr) = 0;

    /** get the name of an attribute
     *
     * @param attr Attribute number
     * @return Attribute name
     * */
    virtual std::string getAttributeName(PMObjectAttribute attr);

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
