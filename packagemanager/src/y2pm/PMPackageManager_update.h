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
//	CLASS NAME : PMUpdateOpts
/**
 * @short Struct for update options
 **/
class PMUpdateOpts {

  public:

    ///////////////////////////////////////////////////////////////////
    // OPTIONS
    ///////////////////////////////////////////////////////////////////

    /**
     * If true, dropped SuSE Packages will be preselected to delete
     **/
    bool delete_unmaintained;

  public:

    PMUpdateOpts() {
      delete_unmaintained = true;
    }

    ~PMUpdateOpts() {}
};

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMUpdateStats
/**
 * @short Struct for update options, statistics, and result lists.
 **/
class PMUpdateStats : public PMUpdateOpts {

  friend std::ostream & operator<<( std::ostream & str, const PMUpdateStats & obj );

  public:

    ///////////////////////////////////////////////////////////////////
    // STATISTICS
    ///////////////////////////////////////////////////////////////////

    /**
     * initial status: packages already tagged to delete
     **/
    unsigned pre_todel;
    /**
     * initial status: packages without candidate (foreign, replaced or dropped)
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
     * update checks: packages skipped due to taboo (foreign)
     **/
    unsigned chk_is_taboo;

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
     * update checks: with candidate: packages untouched (foreign package downgrade)
     **/
    unsigned chk_to_keep_downgrade;
    /**
     * update checks: with candidate: packages untouched (uptodate)
     **/
    unsigned chk_to_keep_installed;

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
     * update checks: without candidate: packages replaced
     *
     * There's exactly one available candidate providing it.
     **/
    unsigned chk_replaced;
    /**
     * update checks: without candidate: packages added by splitprovides
     *
     * Only packages not selected by one of the above checks are counted.
     **/
    unsigned chk_add_split;
    /**
     * update checks: without candidate: package replaced (but not uniqe, thus guessed)
     *
     * There are multiple available candidates providing it. If at the end
     * at least one out of these candidates was set to install by one of the
     * above checks, it's ok. Otherwise we have to guess one.
     **/
    unsigned chk_replaced_guessed;

  public:

    ///////////////////////////////////////////////////////////////////
    // RESULTLISTS
    ///////////////////////////////////////////////////////////////////

    /**
     * chk_to_keep_old / chk_keep_foreign / chk_dropped
     **/
    //std::set<PMSelectablePtr> foreign_and_drop_set;

  public:

    PMUpdateStats() {
      // initial status
      pre_todel			= 0;
      pre_nocand		= 0;
      pre_avcand		= 0;
      // update checks
      chk_installed_total	= 0;
      chk_already_todel		= 0;
      chk_is_taboo		= 0;
      // packages with candidate
      chk_already_toins		= 0;
      chk_to_update		= 0;
      chk_to_downgrade		= 0;
      chk_to_keep_downgrade	= 0;
      chk_to_keep_installed	= 0;
      // packages without candidate
      chk_keep_foreign		= 0;
      chk_dropped		= 0;
      chk_replaced		= 0;
      chk_replaced_guessed	= 0;
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
	+ chk_replaced + chk_replaced_guessed + chk_add_split;
    }

    /**
     * total number of packages that will be finaly deleted
     * (does not count the replaced packages)
     **/
    unsigned totalToDelete() const
    {
      unsigned ret = chk_already_todel;
      if ( delete_unmaintained )
	ret += chk_dropped;
      return ret;
    }

    /**
     * total number of packages that remain untouched
     **/
    unsigned totalToKeep() const
    {
      unsigned ret = chk_is_taboo + chk_to_keep_downgrade + chk_to_keep_installed + chk_keep_foreign;
      if ( !delete_unmaintained )
	ret += chk_dropped;
      return ret;
    }
};

///////////////////////////////////////////////////////////////////

#endif // PMPackageManager_update_h
