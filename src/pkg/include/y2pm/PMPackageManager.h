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

  File:       PMPackageManager.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMPackageManager_h
#define PMPackageManager_h

#include <iosfwd>
#include <list>

#include <y2util/YRpmGroupsTree.h>

#include <y2pm/PMError.h>
#include <y2pm/PMManager.h>
#include <y2pm/PMPackage.h>
#include <y2pm/PMPackageManager_update.h>
#include <y2pm/PkgDu.h>
#include <y2pm/InstSrcManager.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageManager
/**
 *
 **/
class PMPackageManager : public PMManager {

  friend std::ostream & operator<<( std::ostream & str, const PMPackageManager & obj );

  PMPackageManager & operator=( const PMPackageManager & );
  PMPackageManager            ( const PMPackageManager & );

  private:

    friend class Y2PM;
    PMPackageManager();
    virtual ~PMPackageManager();

  private:

    /**
     * Make sure the passed PMObjectPtr actually references a PMPackage. If not,
     * report error and return NULL.
     **/
    virtual PMObjectPtr assertObjectType( const PMObjectPtr & object_r ) const;

    /**
     * problematic update items, filled by doUpdate, used by UI
     */
    PMManager::PMSelectableVec _update_items;

  private:

    YRpmGroupsTree * _rpmGroupsTree;

  public:

    /**
     * Fill in the packages set to be deleted and to be installed.
     * instlist_r is sorted according to PreReqs.
     * srclist_r is the unordered (!) list of all source packages to install
     **/
    void getPackagesToInsDel( std::list<PMPackagePtr> & dellist_r,
			      std::list<PMPackagePtr> & instlist_r,
			      std::list<PMPackagePtr> & srclist_r,
			      InstSrcManager::ISrcIdList sourcerank_r = InstSrcManager::ISrcIdList() );

    /**
     * go through all installed packages and update them.
     *
     * Any options for update are passed down via opt_stats_r.
     * Remaining fields (statistics and returned lists) will be
     * filled by doUpdate.
     **/
    void doUpdate( PMUpdateStats & opt_stats_r );

    /**
     * The number of problematic update packges.
     **/
    unsigned updateSize() const { return _update_items.size(); }

    /**
     * True if update had problems
     **/
    bool updateEmpty() const { return _update_items.empty(); }

    /**
     * Iterator for problematic update items within this Manager.
     **/
    PMSelectableVec::const_iterator updateBegin() const { return _update_items.begin(); }

    /**
     * Iterator for problematic update items within this Manager.
     **/
    PMSelectableVec::const_iterator updateEnd() const { return _update_items.end(); }


    /**
     * Retrieve the internal RPM groups tree (for cloning tree items in the UI etc.).
     **/
    YRpmGroupsTree * rpmGroupsTree() const { return _rpmGroupsTree; }

    /**
     * Insert an RPM group into this tree if not already present.
     * Splits the RPM group string ("abc/def/ghi") and creates tree items for
     * each level as required.
     * Returns the tree entry for this RPM group.
     **/
    YStringTreeItem * addRpmGroup( const std::string & rpmGroup )
	{ return _rpmGroupsTree->addRpmGroup( rpmGroup ); }

    /**
     * Returns the complete (untranslated) RPM group tag string for 'node'.
     **/
    std::string rpmGroup( const YStringTreeItem * node )
	{ return _rpmGroupsTree->rpmGroup( node ); }

    /**
     * Returns the complete translated RPM group tag string for 'node'.
     **/
    std::string translatedRpmGroup( const YStringTreeItem * node )
	{ return _rpmGroupsTree->translatedRpmGroup( node ); }


    ///////////////////////////////////////////////////////////////////
    // Disk usage calcualtion
    ///////////////////////////////////////////////////////////////////
  private:

    PkgDuMaster _du_master;

  public:

    typedef PkgDuMaster::MountPoint MountPoint;

    /**
     * Provide 'df' info for disk usage calculation
     **/
    void setMountPoints( const std::set<MountPoint> & mountpoints_r ) {
      _du_master.setMountPoints( mountpoints_r );
    }

    /**
     * Returns PkgDuMaster, that contains not just the mountpoints set,
     * but overall disk usage info (not spillted to partitions).
     *
     * This does not update the statistics! It's the info of any previous
     * updateDu().
     *
     * <b>Note:</b> If the current mountpoint set is empty, we try to get
     * one from InstTarget.
     **/
    const PkgDuMaster & getDu();

    /**
     * Calculate disk usage based on previously provided 'df' info.
     *
     * Returns PkgDuMaster, that contains not just the mountpoints set,
     * but overall disk usage info (not spillted to partitions).
     *
     * <b>Note:</b> If the current mountpoint set is empty, we try to get
     * one from InstTarget.
     **/
    const PkgDuMaster & updateDu();


    /**
     * Debug: Sum up diskusage of all installed packages to dudata.
     **/
    PkgDuMaster & countDuInstalled( PkgDuMaster & dudata ) const;

    /**
     * Debug: Sum up diskusage of all candidate packages to dudata.
     **/
    PkgDuMaster & countDuCandidates( PkgDuMaster & dudata ) const;

    /**
     * Debug: Sum up diskusage of all candidate packages selected for install to dudata.
     **/
    PkgDuMaster & countDuSelected( PkgDuMaster & dudata ) const;

};

///////////////////////////////////////////////////////////////////

#endif // PMPackageManager_h
