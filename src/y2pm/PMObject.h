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
#include <y2util/Vendor.h>

#include <y2pm/PMObjectPtr.h>
#include <y2pm/PMSolvable.h>

#include <y2pm/PMSelectablePtr.h>

#include <y2pm/InstSrcPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMObject
/**
 * @short Interface class for stuff common to all objects (Packages, Selections,..)
 **/
class PMObject : public PMSolvable {
  REP_BODY(PMObject);

  private:

    friend class PMManager;
    friend class PMSelectable;

    // back pointer to selectable if this object is managed
    PMSelectablePtr _selectable;

  private:

    /**
     * Internally used by PMSelectable. InstSrc may overwrite
     * candidate selection rules under certain conditions.
     **/
    virtual bool prefererCandidate() const { return false; }

  public:

    /**
     * PMObject attributes that should be realized by each concrete Object.
     * Mostly because the UI likes to have some data to show. If there are
     * no data a concrete Object could provide, call PMObjects default
     * implementation.
     **/
    virtual std::string            summary()         const = 0;
    virtual std::list<std::string> description()     const = 0;
    virtual std::list<std::string> insnotify()       const = 0;
    virtual std::list<std::string> delnotify()       const = 0;
    virtual FSize                  size()            const = 0;
    virtual bool                   providesSources() const = 0;

    virtual std::string            instSrcLabel()    const = 0;
    virtual Vendor                 instSrcVendor()   const = 0;
    virtual unsigned               instSrcRank()     const = 0; // rank == 0 is highest!

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
     * Return true if the Object is linked to a Selectable. This is true
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
     * Not to be confused with isCandidateObj() !
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
