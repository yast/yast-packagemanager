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

#include <y2pm/PMTypes.h>
#include <y2pm/PMManager.h>
#include <y2pm/PMSelection.h>
#include <y2pm/PMSelectable.h>
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

  private:

    ///////////////////////////////////////////////////////////////////
    // Remember selection packages fate.
    ///////////////////////////////////////////////////////////////////
    struct PkgFates {
      void add( PMSelectableVec & lhs, const PMSelectableVec & rhs ) {
	lhs.insert( rhs.begin(), rhs.end() );
      }
      public:
	// selections to install:
	PMSelectableVec pkgToDelete;   // hard request: get rid of it (delpack)
	PMSelectableVec pkgToInstall;  // hard request: (re)install it
	// unmodified but installed selections: protect inspacks from deletion (except by a delpack)
	PMSelectableVec pkgOnSystem;   // soft request: Keep it OnSystem.
	// selections to delete:
	PMSelectableVec pkgOffSystem;  // soft request: if no one minds, bring it OffSystem
	// EVERYTHING ELSE:
	PMSelectableVec pkgUnmodified; // is set to unmodified

	void addToDelete  ( const PMSelectableVec & rhs ) { add( pkgToDelete, rhs ); }
	void addToInstall ( const PMSelectableVec & rhs ) { add( pkgToInstall, rhs ); }
	void addOnSystem  ( const PMSelectableVec & rhs ) { add( pkgOnSystem, rhs ); }
	void addOffSystem ( const PMSelectableVec & rhs ) { add( pkgOffSystem, rhs ); }
	void addUnmodified( const PMSelectableVec & rhs ) { add( pkgUnmodified, rhs ); }
      public:
	PMSelectableVec pkgProcessed;

	void setNothingProcessed() { pkgProcessed.clear(); }

	void setProcessed( PMSelectableVec rhs ) {
	  add( pkgProcessed, rhs );
	  rhs.clear();
	}
	void processedToDelete()   { setProcessed( pkgToDelete ); }
	void processedToInstall()  { setProcessed( pkgToInstall ); }
	void processedOnSystem()   { setProcessed( pkgOnSystem ); }
	void processedOffSystem()  { setProcessed( pkgOffSystem ); }
	void processedUnmodified() { setProcessed( pkgUnmodified ); }

	bool didProcess( PMSelectablePtr pkg_r ) const {
	  return( pkgProcessed.find( pkg_r ) != pkgProcessed.end() );
	}
    };

    /**
     * Go through package fates lists and accordingly select/deselect
     * the packages in PMPackageManager. Used by @ref activate.
     **/
    PMError syncToPkgMgr( PkgFates & fates_r );

  public:

    /**
     * Activate the selection. I.e. translate the selections state changes into
     * state changes of individual packages and sync this to the PMPackageManager.
     **/
    PMError activate( PMPackageManager & package_mgr );

    /**
     * Same as above using Y2PM::packageManager().
     **/
    PMError activate();

    /**
     * Adjust language specific packages of the currently activated
     * selection set. Hast to be done whenever the set of requested
     * locales changes.
     **/
    PMError requestedLocalesChanged( const PM::LocaleSet & addLocales_r,
				     const PM::LocaleSet & delLocales_r );

    /**
     * Remember all activated selections in InstTargetSelDB. Reset all
     * selections (not the packages!) and reflect the new InstTargetSelDB
     * content.
     **/
    PMError installOnTarget();

};

///////////////////////////////////////////////////////////////////

#endif // PMSelectionManager_h
