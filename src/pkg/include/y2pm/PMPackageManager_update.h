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

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMUpdateStats
/**
 *
 **/
class PMUpdateStats {

  friend std::ostream & operator<<( std::ostream & str, const PMUpdateStats & obj );

  PMUpdateStats & operator=( const PMUpdateStats & );
  PMUpdateStats            ( const PMUpdateStats & );

  public:

    // initial status:      either:
    unsigned pre_todel;  // packages tagged to delete
    unsigned pre_nocand; // packages without candidate (foreign, renamed or droped)
    unsigned pre_avcand; // packages available for update (could be installed)

    // update checks
    unsigned chk_installed_total; // total installed packages checked
    unsigned chk_already_todel;
    unsigned chk_already_toins;
    // packages with candidate
    unsigned chk_to_update;
    unsigned chk_to_downgrade;
    unsigned chk_to_keep_old;
    // packages without candidate
    unsigned chk_keep_foreign;
    unsigned chk_dropped;
    unsigned chk_renamed;
    unsigned chk_renamed_guessed;
    unsigned chk_add_split;

  public:

    PMUpdateStats() {
      // initial status
      pre_todel  = 0;
      pre_nocand = 0;
      pre_avcand = 0;
      // update checks
      chk_installed_total = 0;
      chk_already_todel   = 0;
      chk_already_toins   = 0;
      // packages with candidate
      chk_to_update    = 0;
      chk_to_downgrade = 0;
      chk_to_keep_old  = 0;
      // packages without candidate
      chk_keep_foreign    = 0;
      chk_dropped         = 0;
      chk_renamed         = 0;
      chk_renamed_guessed = 0;
      chk_add_split       = 0;
    }
    ~PMUpdateStats() {}
};

///////////////////////////////////////////////////////////////////

#endif // PMPackageManager_update_h
