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
#include <y2pm/PMSelectable.h>
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

    typedef std::map<PMSelectablePtr,PMSelectable::Fate> ActiveMap;

    /**
     * Remembers selections fate at last call to @ref activate.
     **/
    ActiveMap _last_active;

    /**
     * Remember selections state.
     **/
    void setLast( const PMSelectablePtr & sel_r, PMSelectable::Fate val_r );

    /**
     * Return selections remembered state.
     **/
    PMSelectable::Fate lastState( const PMSelectablePtr & sel_r ) const;

    /**
     * Return selections current state.
     **/
    PMSelectable::Fate getState( const PMSelectablePtr & sel_r ) const;

  private:

    ActiveMap saved_last_active;

  public:

    /**
     * Save current selection.
     **/
    virtual void SaveState() {
      saved_last_active = _last_active;
      PMManager::SaveState();
    }

    /**
     * Restore previously saved selection.
     **/
    virtual bool RestoreState() {
      if ( !PMManager::RestoreState() ) {
	saved_last_active.clear();
	return false;
      }
      _last_active = saved_last_active;
      return true;
    }

    /**
     * Forgett a previously saved selection.
     **/
    virtual void ClearSaveState() {
      saved_last_active.clear();
      PMManager::ClearSaveState();
    }


  private:

    /**
     * Make shure the passed PMObjectPtr actually references a PMSelection. If not,
     * rerport error and return NULL.
     **/
    virtual PMObjectPtr assertObjectType( const PMObjectPtr & object_r ) const;

  public:

    /**
     * activate the selection
     * (go through package list of selection and select/deselect
     * all packages in PMPackageManager)
     **/
    PMError activate( PMPackageManager & package_mgr );

    /**
     * Same as above using Y2PM::packageManager().
     **/
    PMError activate();

    /**
     * Remember all activated selections in InstTargetSelDB. Reset all
     * selections (not the packages!) and reflect the new InstTargetSelDB
     * content.
     **/
    PMError installOnTarget();

};

///////////////////////////////////////////////////////////////////

#endif // PMSelectionManager_h
