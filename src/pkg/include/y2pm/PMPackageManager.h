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

#include <y2pm/PMError.h>
#include <y2pm/PMManager.h>
#include <y2pm/PMPackage.h>
#include <y2util/YRpmGroupsTree.h>
#include <y2pm/PMPackageManager_update.h>

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

  private:

    YRpmGroupsTree * _rpmGroupsTree;

  public:

    /**
     * are there currently any "by_user" selectables ?
     *
     */
    bool anythingByUser(void);

    /**
     * are there currently any selectables for deletion ?
     *
     */
    bool anythingToDelete (void);

    /**
     * are there currently any selectables for installation ?
     *
     */
    bool anythingToInstall (void);

    /**
     * Fill in the packages set to be deleted and to be installed.
     * instlist_r is sorted according to PreReqs.
     **/
    void getPackagesToInsDel( std::list<PMPackagePtr> & dellist_r,
			      std::list<PMPackagePtr> & instlist_r );

    /**
     * go through all installed packages and update them.
     *
     * Any options for update are passed down via opt_stats_r.
     * Remaining fields (statistics and returned lists) will be
     * filled by doUpdate.
     **/
    void doUpdate( PMUpdateStats & opt_stats_r );

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

};

///////////////////////////////////////////////////////////////////

#endif // PMPackageManager_h
