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

#include <y2pm/InstSrc.h>

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

  private:

    static std::string _cache_root_dir;

    static const Pa;

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;

    typedef constInstSrcPtr   ISrcId;

    typedef std::list<ISrcId> ISrcIdList;

  private:

    typedef std::set<InstSrcPtr>  ISrcPool;

    typedef std::list<InstSrcPtr> ISrcList;

    ISrcPool _knownSources;

    ISrcList _enabledSources;

  public:

    /**
     * Access media. Detect kind of InstSrc available on media.
     *
     **/
    PMError scanMedia( ISrcIdList & idlist_r, const Url & mediaurl_r );

    PMError enableSource( const ISrcId & isrc_r );

    PMError disableSource( const ISrcId & isrc_r );

    PMError deleteSource( const ISrcId & isrc_r );

    PMError setAutoenable( const ISrcId isrc_r, const bool yesno );

};

///////////////////////////////////////////////////////////////////

#endif // InstSrcManager_h
