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
#include <y2pm/PkgArch.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMObject
/**
 * @short Interface class for stuff common to all objects (Packages, Selections,..)
 **/
class REP_CLASS(PMObject), public PMSolvable {
  REP_BODY(PMObject)
  public:
    enum PMObjectAttribute {
      PMOBJ_SUMMARY = 0,
      PMOBJ_DESCRIPTION,
      PMOBJ_SIZE,

      PMOBJ_NUM_ATTRIBUTES
    };

  private:

      static const char* const PMObjectAttributeNames[PMOBJ_NUM_ATTRIBUTES];

  public:

    PMObject( const PkgName &    name_r,
	      const PkgEdition & edition_r );

    virtual ~PMObject();

  public:

//    virtual std::string Summary() const { return "--not available--"; }

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
    std::string getAttributeName(PMObjectAttribute attr);

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMObject_h
