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
#include <map>

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


    typedef std::map<PMSelectablePtr,int> ActiveMap;

    /**
     * Remembers selections state at last call to @ref activate.
     * <ul>
     * <li><code>-1</code> was to delete
     * <li><code>0</code> was unmodified
     * <li><code>1</code> was to install
     * </ul>
     **/
    ActiveMap _last_active;

    /**
     * Remember selections state.
     **/
    void setLast( const PMSelectablePtr & sel_r, int val_r );

    /**
     * Return selections remembered state.
     **/
    int lastState( const PMSelectablePtr & sel_r ) const;

    /**
     * Return selections current state.
     **/
    int getState( const PMSelectablePtr & sel_r ) const;

  private:

    /**
     * Make shure the passed PMObjectPtr actually references a PMSelection. If not,
     * rerport error and return NULL.
     **/
    virtual PMObjectPtr assertObjectType( const PMObjectPtr & object_r ) const;

  private:

    /**
     * Set all packages in this selection to unmodified.
     **/
    void resetSelectionPackages( const PMSelectionPtr & sel_r, PMPackageManager & package_mgr );
    /**
     * Install selection (install inspacks, delete delpacks).
     **/
    void setSelectionPackages( const PMSelectionPtr & sel_r, PMPackageManager & package_mgr );
    /**
     * Delete selection (delete inspacks, unmodify delpacks).
     **/
    void removeSelectionPackages( const PMSelectionPtr & sel_r, PMPackageManager & package_mgr );

  public:

    /**
     * activate the selection
     * (go through package list of selection and select/deselect
     * all packages -> needs PMPackageManager)
     */
    PMError activate( PMPackageManager & package_mgr );

    /**
     * Remember all activated selections in InstTargetSelDB. Reset all
     * selections (not the packages!) and reflect the new InstTargetSelDB
     * content.
     **/
    PMError installOnTarget();

};

///////////////////////////////////////////////////////////////////

#endif // PMSelectionManager_h
