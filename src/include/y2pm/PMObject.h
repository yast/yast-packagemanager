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
class REP_CLASS(PMObject), public PMSolvable {
  REP_BODY(PMObject)

  private:

    friend class PMManager;

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

    /**
     * Return wheter the Object is managed.
     **/
    bool hasSelectable() const { return( _selectable != 0 ); }

    /**
     * Return the managing Selectable.
     **/
    PMSelectablePtr getSelectable() const { return _selectable; }

#if 0
    // TBD: first test whether comparison of PMObjectPtr <-> PMObject* works.
    bool isInstalledObj() const {}
    bool isCandidateObj() const {}
    bool isInCandidateList() const {}
#endif

  public:

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

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMObject_h
