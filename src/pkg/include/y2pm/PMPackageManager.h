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
     * Fill in the packages set to be deleted and to be installed.
     * instlist_r is sorted according to PreReqs.
     **/
    void getPackagesToInsDel( std::list<PMPackagePtr> & dellist_r,
			      std::list<PMPackagePtr> & instlist_r );

    /**
     * prepare for update
     * go through all installed packages and update them
     *
     * return number of packages affected
     * return non-suse packages for which an update candidate exists in noinstall_r
     * return non-suse packages for which an obsolete exists in nodelete_r
     *
     **/
    int doUpdate (std::list<PMPackagePtr>& noinstall_r, std::list<PMPackagePtr>& nodelete_r);

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
