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
#include <list>

#include <y2util/Pathname.h>

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

    PkgSet* installed;
    PkgSet* available;
    PkgSet* toinstall;
    PkgSet* nowinstalled;

  private:

    typedef std::list<PMSelectable::SavedState> SavedList;

    SavedList _savedList;

  public:

    /**
     * Save current selection.
     **/
    void SaveState();

    /**
     * Restore previously saved selection.
     **/
    bool RestoreState();

    /**
     * Return true if current selection differs from previously saved selection.
     **/
    bool DiffState() const;

    /**
     * Forgett a previously saved selection.
     **/
    void ClearSaveState();

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
     * Pre poolSetInstalled hook
     **/
    virtual void prePSI() {;}
    /**
     * Post poolSetInstalled hook
     **/
    virtual void postPSI() {;}
    /**
     * Pre poolAddCandidates hook
     **/
    virtual void prePAC() {;}
    /**
     * Post poolAddCandidates hook
     **/
    virtual void postPAC() {;}
    /**
     * Pre poolRemoveCandidates hook
     **/
    virtual void prePRC() {;}
    /**
     * Post poolRemoveCandidates hook
     **/
    virtual void postPRC() {;}

  private:

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
     *
     * This clears any saved state!
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

    /**
     * General test whether fnc_r returns true for at least one Selectable.
     *
     * @param fnc_r pointer to a boolean PMSelectable const method
     * (e.g. PMSelectable::by_user).
     **/
    bool anyMatch( PMSelectable::Test_method fnc_r ) const;

    /**
     * General test whether fnc_r returns true for at least one Selectable.
     *
     * @param fnc_r pointer to boolean test function taking a constPMSelectablePtr
     * as argument.
     **/
    bool anyMatch( PMSelectable::Test_fnc fnc_r ) const;

    /**
     * are there currently any "by_user" selectables ?
     **/
    bool anythingByUser() const { return anyMatch( &PMSelectable::by_user ); }

    /**
     * are there currently any selectables for deletion ?
     **/
    bool anythingToDelete() const { return anyMatch( &PMSelectable::to_delete ); }

    /**
     * are there currently any selectables for installation ?
     **/
    bool anythingToInstall() const { return anyMatch( &PMSelectable::to_install ); }

  public:
    // solver Part

    /**
     * resolve dependencies for packages marked for installation
     *
     * @param good (output) list of PkgDep::Result that have been marked for installation/upgrade
     * @param bad (output) list of PkgDep::ErrorResult that have trouble
     * @param filter_conflicts_with_installed filter packages which are
     * selected for insallation by appl and conflict with an already installed
     * one
     *
     * @return true if no conflicts where found (i.e no bad)
     * */
    bool solveInstall(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad, bool filter_conflicts_with_installed = false);

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
     *
     * DO NOT USE ATM
     * */
    //bool solveUpgrade(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad, PkgDep::SolvableList &to_remove);


//    bool solveEverythingRight(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad, PkgDep::SolvableList &to_remove);

    /**
     * determine whether installed system is consistent
     *
     * @param bad broken packages
     * */
    bool solveConsistent(PkgDep::ErrorResultList& bad);

    /**
     * set maximum number of packages that will be automatically selected for
     * removal on upgrade
     * */
    //void setMaxRemoveThreshold(unsigned nr);

  private:

    void buildSets(PkgSet& installed, PkgSet& available, PkgSet& to_install);

  public:

    /**
      Write settings like Taboo states to disk.
    */
    virtual void writeSettings();

    /**
      Read setting slike Taboo states from disk.
    */
    virtual void readSettings();

  protected:

    /**
      Return file name used to read and write settings. Subclasses should
      override this function and provide an appropriate filename, if they need
      their settings to be stored.
    */
    virtual Pathname settingsFile() const { return Pathname(); }
};

///////////////////////////////////////////////////////////////////

#endif // PMManager_h
