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

#include <y2util/BitField.h>

#include <y2pm/PMObject.h>
#include <y2pm/PMSelectable.h>
#include <y2pm/PMObjectContainerIter.h>

#include <y2pm/PkgDep.h>

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

    //void solveDeps();

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
     * resolve dependences for Packages marked for installation
     *
     * @param good (output) list of PkgDep::Result that can be installed
     * @param bad (output) list of PkgDep::ErrorResult that have trouble
     *
     * @return true if no conflicts where found (i.e no bad)
     * */
    bool solveInstall(PkgDep::ResultList& good, PkgDep::ErrorResultList& bad);

};

///////////////////////////////////////////////////////////////////

#endif // PMManager_h
