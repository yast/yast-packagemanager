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

  File:       PMSelectionManager.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMSelectionManager_h
#define PMSelectionManager_h

#include <iosfwd>
#include <list>

#include <y2pm/PMError.h>
#include <y2pm/PMManager.h>
#include <y2pm/PMSelection.h>
#include <y2pm/PMPackageManager.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectionManager
/**
 *
 **/
class PMSelectionManager : public PMManager {

  friend std::ostream & operator<<( std::ostream & str, const PMSelectionManager & obj );

  PMSelectionManager & operator=( const PMSelectionManager & );
  PMSelectionManager            ( const PMSelectionManager & );

  private:

    friend class Y2PM;
    PMSelectionManager();
    virtual ~PMSelectionManager();

    /**
     * list of currently active selections
     * used to determine which (packages of) selections to
     * de-select when activate() is called.
     *
     * see activate()
     */
    std::list<PMSelectablePtr> _currently_actives;
    LangCode _currently_preferred_locale;
    std::list<LangCode> _currently_requested_locales;

  private:

    /**
     * Make shure the passed PMObjectPtr actually references a PMSelection. If not,
     * rerport error and return NULL.
     **/
    virtual PMObjectPtr assertObjectType( const PMObjectPtr & object_r ) const;

    /**
     * get first matching alternative selection of given
     * "pack (alt1, alt2, ...)" string
     */
    PMSelectablePtr getAlternativeSelectable (std::string pkgstr, PMPackageManager & package_mgr);

    /**
     * set all packages of the given list to "auto"
     */
    void setSelectionPackages (const std::list<std::string> packages, bool these_are_delpacks, PMPackageManager & package_mgr);

    /**
     * set all packages of this selection to "don't install"
     * if their status is "auto" (i.e. not explicitly requested
     * by the user).
     */
    void resetSelectionPackages (const std::list<std::string> packages, bool these_are_inspacks, PMPackageManager & package_mgr);

    /**
     * activate given selection
     * set all packages of this selection to "auto"
     */
    void setSelection (PMSelectionPtr selection, PMPackageManager & package_mgr);

    /**
     * de-activate given selection
     * set all packages of this selection to "don't install"
     * if their status is "auto" (i.e. not explicitly requested
     * by the user).
     */
    void resetSelection (PMSelectionPtr selection, PMPackageManager & package_mgr);

  public:

    /**
     * activate the selection
     * (go through package list of selection and select/deselect
     * all packages -> needs PMPackageManager)
     */
    PMError activate( PMPackageManager & package_mgr );

    /**
     * Remember all activated selection in InstTargetSelDB
     **/
    PMError installOnTarget() const;

};

///////////////////////////////////////////////////////////////////

#endif // PMSelectionManager_h
