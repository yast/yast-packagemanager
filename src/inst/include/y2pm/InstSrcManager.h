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

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;


  private:

    friend class Y2PM;
    InstSrcManager( const bool autoEnable_r = true );
    ~InstSrcManager();

  private:

    /**
     * Cachedir to use.
     **/
    static Pathname _cache_root_dir;

    /**
     * Subpath (below _cache_root_dir) for InstSrcManager downloads.
     **/
    static const Pathname _cache_tmp_dir;

    /**
     * Helper function to combine _cache_root_dir and subpaths.
     **/
    Pathname cachePath( const Pathname & sub_r ) const { return( _cache_root_dir + sub_r ); }

  private:

    /**
     * Full path of cache_root_dir
     **/
    Pathname cache_root_dir() const { return _cache_root_dir; }

    /**
     * Full path of cache_tmp_dir (for downloads, etc.)
     **/
    Pathname cache_tmp_dir() const { return cachePath( _cache_tmp_dir ); }

    /**
     * Return a new (not existing) directory name in cache_root_dir
     **/
    Pathname genSrcCacheName() const;

  private:

    struct ProductEntry {
      Pathname    _dir;
      std::string _name;
      bool operator<( const ProductEntry & rhs ) const {
	return( _dir.asString() < rhs._dir.asString() );
      }
    };

    typedef std::set<ProductEntry> ProductSet;

    PMError scanProductsFile( const Pathname & file_r, ProductSet & pset_r ) const;

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;

    typedef constInstSrcPtr   ISrcId;

    typedef std::list<ISrcId> ISrcIdList;

  private:

    typedef std::set<InstSrcPtr> ISrcPool;

    ISrcPool _knownSources;

    /**
     * Preload all cached InstSrces.
     **/
    PMError initSrcPool( const bool autoEnable_r );

    /**
     * Create InstSrc from cache and add it to ISrcPool.
     **/
    PMError scanSrcCache( const Pathname & srccache_r, const bool autoEnable_r );

    /**
     * Find InstSrcPtr in _knownSources by ISrcId.
     * Return NULL if not in _knownSources.
     **/
    InstSrcPtr lookupId( const ISrcId & isrc_r ) const;

    /**
     * Find InstSrcPtr in _knownSources by comparing
     * InstSrcDescr. Return NULL if not in _knownSources.
     **/
    InstSrcPtr lookupInstSrc( const InstSrcPtr & isrc_r ) const;

    /**
     * Add nsrc_r to _knownSources if same product is not yet present.
     * Return ISrcId for the added nsrc_r, or NULL if duplicate.
     **/
    ISrcId poolAdd( InstSrcPtr nsrc_r );

    /**
     * helper for cacheCopyTo fake
     **/
    PMError intern_cacheCopyTo( const Pathname & newRoot_r );

  public:

    /**
     * Access media. Detect kind of InstSrc(es) available on media, and
     * load their descriptions. (add them to ISrcPool)
     **/
    PMError scanMedia( ISrcIdList & idlist_r, const Url & mediaurl_r );

    /**
     * Enable InstSrc. Let it provide it's Objects to the Manager classes.
     **/
    PMError enableSource( const ISrcId & isrc_r );

    /**
     * Disable InstSrc. Provided Objects are withdrawn from Manager classes.
     **/
    PMError disableSource( const ISrcId & isrc_r );
#if 0
    /**
     * Delete InstSrc. Erase it together with all cached info.
     **/
    PMError deleteSource( const ISrcId & isrc_r );
#endif
    /**
     * Set whether this source should be automaticaly enabled on startup.
     *
     * Note that this is something different, than calling enableSource/disableSource.
     * setAutoenable determines the desired state of an InstSrc when the InstSrcManager
     * is started (auto enable it or not). enableSource/disableSource actually
     * enabale/disable an InstSrc (i.e. let it provide/withdraw Objects).
     **/
    PMError setAutoenable( const ISrcId isrc_r, const bool yesno );

  public:

    /**
     * Return the number of known InstSrc'es.
     **/
    unsigned knownSources() const { return _knownSources.size(); }

    /**
     * PRELIMINARILY: Return via idlist_r the ISrcId's of all known
     * InsrSrces. Or enabled InsrSrces only, if enabled_only == true.
     *
     * Should be replaced by some Iterator.
     **/
    void getSources( ISrcIdList & idlist_r, const bool enabled_only = false ) const;

    /**
     * Used during installation/Upadte (where caching is disabled)
     * to install the caches on the system. So InstSrc'es will be
     * available in the running system after boot.
     **/
    PMError cacheCopyTo( const Pathname & newRoot_r );
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcManager_h
