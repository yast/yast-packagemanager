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

   File:       PMManager.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMManager_h
#define PMManager_h

#include <iosfwd>
#include <set>
#include <map>

#include <y2pm/PMObject.h>
#include <y2pm/PMSelectable.h>
#include <y2pm/PMObjectContainerIter.h>

#include <y2pm/PkgDep.h>
#include <y2pm/PkgSet.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMManager
/**
 *
 **/
class PMManager {

  friend std::ostream & operator<<( std::ostream & str, const PMManager & obj );

  PMManager & operator=( const PMManager & );
  PMManager            ( const PMManager & );

  public:

    typedef std::set<PMSelectablePtr>  PMSelectableVec;

  private:

    typedef std::map<std::string,PMSelectablePtr> PMSelectablePool;

    PMSelectablePool _itemPool;
    PMSelectableVec _items;

  private:

    /**
     * Concrete Manager has to assert that the passed ObjectPtr actually references the
     * correct type of Object (PackageManager e.g. will handle nothing else but Packages).
     **/
    virtual PMObjectPtr assertObjectType( const PMObjectPtr & object_r ) const = 0;

    /**
     * Currently we don't need concrete Selectables (e.g. PackageSelectable), thus new
     * Selectables are created here. Otherwise make it pure virtual, and let the concrete
     * Manager create the appropriate type of Selectable.
     **/
    virtual PMSelectablePtr newSelectable( const PkgName & name_r ) const;

    /**
     * Lookup Selectable by name.
     **/
    PMSelectablePtr poolLookup( const std::string & name_r ) const;

    /**
     * Make shure the pool contains a Selectable named name_r, and return it.
     **/
    PMSelectablePtr poolProvide( const std::string & name_r );

    /**
     * Remove superfluous empty Selecatables. To be called after Objects
     * were removed from Selectables.
     **/
    void poolAdjust();


    void clearAll();

    void checkPool() const;

  public:

    PMManager();
    virtual ~PMManager();

  public:

    /**
     * Called from TargetSystem providing all(!) installed objects
     **/
    void poolSetInstalled( PMObjectContainerIter iter_r );

    /**
     * Called from InstSrc to add provided Objects
     **/
    void poolAddCandidates( PMObjectContainerIter iter_r );

    /**
     * Called from InstSrc to remove the formerly added objects.
     **/
    void poolRemoveCandidates( PMObjectContainerIter iter_r );

  public:
    /**
     * update all installed objects within this manager
     * return number of objects affected
     *
     * goes through all managed objects and marks every object
     * as 'to_install' if an installed and a (newer) candidate exists.
     */
    unsigned int updateAllInstalled (bool only_newer = true);

  public:

    /**
     * The number of Selectables within this Manager.
     **/
    unsigned size() const { return _items.size(); }

    /**
     * True if Manager does not contain any Selectable
     **/
    bool empty() const { return _items.empty(); }

    /**
     * Iterator for Selectables within this Manager.
     **/
    PMSelectableVec::const_iterator begin() const { return _items.begin(); }

    /**
     * Iterator for Selectables within this Manager.
     **/
    PMSelectableVec::const_iterator end() const { return _items.end(); }

    /**
     * Iterator for Selectables within this Manager.
     **/
    PMSelectableVec::const_reverse_iterator rbegin() const { return _items.rbegin(); }

    /**
     * Iterator for Selectables within this Manager.
     **/
    PMSelectableVec::const_reverse_iterator rend() const { return _items.rend(); }

    /**
     * PMSelectablePtr to the Selectable with the given name, or NULL if there is none.
     **/
    PMSelectablePtr getItem( const std::string & name_t ) const { return poolLookup( name_t ); }

    /**
     * PMSelectablePtr to the Selectable with the given name, or NULL if there is none.
     **/
    PMSelectablePtr operator [] ( const std::string & name_r ) const { return getItem( name_r ); }

  public:

    /**
     * Set all Selectables to NothingSelected ( neither install nor delete )
     **/
    void setNothingSelected();

  public:
    // solver Part

    /**
     * resolve dependencies for packages marked for installation
     *
     * @param good (output) list of PkgDep::Result that have been marked for installation/upgrade
     * @param bad (output) list of PkgDep::ErrorResult that have trouble
     *
     * @return true if no conflicts where found (i.e no bad)
     * */
    bool solveInstall(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad);
    
    /**
     * resolve dependencies for all packages that are marked for installation
     * or are available with newer version
     *
     * @param good (output) list of PkgDep::Result that have been marked for installation/upgrade
     * @param bad (output) list of PkgDep::ErrorResult that have trouble
     * @param to_remove (output) list of PMSolvablePtr that have been marked for removal
     *
     * @return false if upgrade failed (probably due to max_remove). Results
     * may not be reliable and manual intervention is required
     * */
    bool solveUpgrade(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad, PkgDep::SolvableList &to_remove);

    /**
     * set maximum number of packages that will be automatically selected for
     * removal on upgrade
     * */
    void setMaxRemoveThreshold(unsigned nr);

  private:

    void buildSets(PkgSet& installed, PkgSet& available, PkgSet& to_install);

};

///////////////////////////////////////////////////////////////////

#endif // PMManager_h
