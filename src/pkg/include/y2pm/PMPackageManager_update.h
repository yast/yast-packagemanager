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

  File:       PMPackageManager_update.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef PMPackageManager_update_h
#define PMPackageManager_update_h

#include <iosfwd>
#include <set>

#include <y2pm/PMSelectablePtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMUpdateStats
/**
 * @short Struct for update options, statistics, and result lists.
 *
 *
 **/
class PMUpdateStats {

  friend std::ostream & operator<<( std::ostream & str, const PMUpdateStats & obj );

  PMUpdateStats & operator=( const PMUpdateStats & );
  PMUpdateStats            ( const PMUpdateStats & );

  public:

    ///////////////////////////////////////////////////////////////////
    // OPTIONS
    ///////////////////////////////////////////////////////////////////

    /**
     * If true, dropped SuSE Packages will be preselected to delete
     **/
    bool delete_unmaintained;

  public:

    ///////////////////////////////////////////////////////////////////
    // STATISTICS
    ///////////////////////////////////////////////////////////////////

    /**
     * initial status: packages already tagged to delete
     **/
    unsigned pre_todel;
    /**
     * initial status: packages without candidate (foreign, renamed or droped)
     **/
    unsigned pre_nocand;
    /**
     * initial status: packages with candidate (available for update)
     **/
    unsigned pre_avcand;


    /**
     * update checks: total number of packages checked (should be number of installed packages)
     **/
    unsigned chk_installed_total;

    /**
     * update checks: packages already tagged to delete (should be pre_todel)
     **/
    unsigned chk_already_todel;

    /**
     * update checks: with candidate: packages already tagged to install
     **/
    unsigned chk_already_toins;
    /**
     * update checks: with candidate: packages updated to new version
     **/
    unsigned chk_to_update;
    /**
     * update checks: with candidate: packages downgraded
     *
     * Installed and candidate package have vendor SuSE and candidates buildtime
     * is newer.
     **/
    unsigned chk_to_downgrade;
    /**
     * update checks: with candidate: packages untouched
     **/
    unsigned chk_to_keep_old;

    /**
     * update checks: without candidate: installed package has not vendor SuSE
     *
     * Remains untouched.
     **/
    unsigned chk_keep_foreign;
    /**
     * update checks: without candidate: packages dropped (or SuSE internal)
     *
     * There's no available candidate providing it.
     **/
    unsigned chk_dropped;
    /**
     * update checks: without candidate: packages renamed
     *
     * There's exactly one available candidate providing it.
     **/
    unsigned chk_renamed;
    /**
     * update checks: without candidate: packages added by splitprovides
     *
     * Only packages not selected by one of the above checks are counted.
     **/
    unsigned chk_add_split;
    /**
     * update checks: without candidate: package renamed (but not uniqe, thus guessed)
     *
     * There are multiple available candidates providing it. If at the end
     * at least one out of these candidates was set to install by one of the
     * above checks, it's ok. Otherwise we have to guess one.
     **/
    unsigned chk_renamed_guessed;

  public:

    ///////////////////////////////////////////////////////////////////
    // RESULTLISTS
    ///////////////////////////////////////////////////////////////////

    /**
     * chk_to_keep_old / chk_keep_foreign / chk_dropped
     **/
    std::set<PMSelectablePtr> foreign_and_drop_set;

  public:

    PMUpdateStats() {
      // initial options
      delete_unmaintained       = true;
      // initial status
      pre_todel			= 0;
      pre_nocand		= 0;
      pre_avcand		= 0;
      // update checks
      chk_installed_total	= 0;
      chk_already_todel		= 0;
      // packages with candidate
      chk_already_toins		= 0;
      chk_to_update		= 0;
      chk_to_downgrade		= 0;
      chk_to_keep_old		= 0;
      // packages without candidate
      chk_keep_foreign		= 0;
      chk_dropped		= 0;
      chk_renamed		= 0;
      chk_renamed_guessed	= 0;
      chk_add_split		= 0;
    }
    ~PMUpdateStats() {}

    /**
     * total number of packages that will be installed
     **/
    unsigned totalToInstall() const
    {
      return chk_already_toins
	+ chk_to_update + chk_to_downgrade
	+ chk_renamed + chk_renamed_guessed + chk_add_split;
    }

    /**
     * total number of packages that will be finaly deleted
     * (does not count the renamed packages)
     **/
    unsigned totalToDelete() const
    {
      unsigned ret = chk_already_todel;
      if ( delete_unmaintained )
	ret += chk_dropped;
      return ret;
    }
};

///////////////////////////////////////////////////////////////////

#endif // PMPackageManager_update_h
