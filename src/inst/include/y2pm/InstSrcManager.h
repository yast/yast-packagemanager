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

  File:       InstSrcManager.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef InstSrcManager_h
#define InstSrcManager_h

#include <iosfwd>
#include <set>
#include <list>

#include <y2pm/InstSrcPtr.h>
#include <y2pm/MediaInfoPtr.h>

#include <y2pm/PMError.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcManager
/**
 *
 **/
class InstSrcManager {

  friend std::ostream & operator<<( std::ostream & str, const InstSrcManager & obj );

  InstSrcManager & operator=( const InstSrcManager & );
  InstSrcManager            ( const InstSrcManager & );

  private:

    friend class Y2PM;
    InstSrcManager();
    ~InstSrcManager();

  public:

    enum Error {
      E_OK    = PMError::E_OK,
      E_Error = PMError::E_INST_SRC_MGR,
      E_NO_MEDIA
    };

  public:

    typedef unsigned               ISrcId;
    typedef std::set<InstSrcPtr>   ISrcPool;
    typedef std::list<InstSrcPtr>  ISrcList;

    enum ISrcType {
      T_UNKNOWN,
      T_SUSE_DIST
    };

  private:

    ISrcPool _knownSources;

    ISrcList _enabledSources;

  public:

    /**
     * Access media. Detect kind of InstSrc available on media( if type == T_UKNOWN ).
     * Add InstSrc to the list of known sources and return the InstSrcPtr.
     **/
    PMError scanMedia( constInstSrcPtr & isrc_r, MediaInfoPtr media_r, const ISrcType type_r = T_UNKNOWN );

    PMError enableSource( InstSrcPtr & isrc_r );

    PMError disableSource( InstSrcPtr & isrc_r );

    PMError deleteSource( InstSrcPtr isrc_r );

    PMError setAutoenable( InstSrcPtr isrc_r, const bool yesno );

};

///////////////////////////////////////////////////////////////////

#endif // InstSrcManager_h
