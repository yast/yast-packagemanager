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
#include <map>
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

    static const unsigned NO_RANK = unsigned(-1);

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
      ProductEntry( const Pathname & dir_r = "/", const std::string & name_r = std::string() ){
	_dir  = dir_r;
	_name = name_r;
      }
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

    typedef std::list<InstSrcPtr> ISrcPool;

    ISrcPool _knownSources;

    /**
     * Preload all cached InstSrces.
     **/
    PMError initSrcPool( const bool autoEnable_r );

    /**
     * Create InstSrc from cache and add it to ISrcPool.
     **/
    PMError scanSrcCache( const Pathname & srccache_r );

    /**
     * Rewrite rank values of all known sources. Used by initSrcPool.
     **/
    PMError writeNewRanks();

    /**
     * Return _knownSources iterator to ISrcId.
     * Return _knownSources.end() if not in _knownSources.
     **/
    ISrcPool::iterator poolHandle( const ISrcId & isrc_r );

    /**
     * Find InstSrcPtr in _knownSources by ISrcId.
     * Return NULL if not in _knownSources.
     **/
    InstSrcPtr lookupId( const ISrcId & isrc_r ) const;

    /**
     * Find InstSrcPtr in _knownSources by numeric srcID.
     * Return NULL if not in _knownSources.
     **/
    InstSrcPtr lookupSourceByID( InstSrc::UniqueID srcID_r ) const;

    /**
     * Find InstSrcPtr in _knownSources by comparing
     * InstSrcDescr. Return NULL if not in _knownSources.
     **/
    InstSrcPtr lookupInstSrc( const InstSrcPtr & isrc_r ) const;

    /**
     * Add nsrc_r to _knownSources if same product is not yet present.
     * Return ISrcId for the added nsrc_r, or NULL if duplicate.
     *
     * If nsrc has no rank assigned, it gets least priority.
     *
     * <b>Note:</> If norankcheck_r is true, ranks are not checked,
     * and added sources are not attached to the InstSrcManager.
     * This is to be used by initSrcPool/scanSrcCache at startup
     * only! They take care of ranking and attaching after all
     * cached sources were loaded.
     **/
    ISrcId poolAdd( InstSrcPtr nsrc_r, bool rankcheck_r = true );

    /**
     * Enable/disable isrc_r according to yesno_r.
     **/
    PMError activateSource( const InstSrcPtr & isrc_r, bool yesno_r );

    /**
     * Enable/disable isrc_r according to yesno_r.
     **/
    PMError activateSource( const ISrcId & isrc_r, bool yesno_r );

    /**
     * helper for cacheCopyTo fake
     **/
    PMError intern_cacheCopyTo();

  public:

    /**
     * Access media. Detect kind of InstSrc(es) available on media by scanning
     * /media.1/products if available. Otherwise assume the InstSrc is located
     * directly below mediaurl.
     *
     * Load the InstSrc(es) (add them to ISrcPool).
     **/
    PMError scanMedia( ISrcIdList & idlist_r, const Url & mediaurl_r );

    /**
     * Load a certain InstSrc located on mediaurl in product_dir.
     *
     * This is what the above scanMedia function calls, for each product
     * found in a /media.1/products file.
     **/
    PMError scanMedia( ISrcId & isrc_r, const Url & mediaurl_r, const Pathname & product_dir_r );

    /**
     * Enable InstSrc. Let it provide it's Objects to the Manager classes.
     **/
    PMError enableSource( const ISrcId & isrc_r ) { return activateSource( isrc_r, true ); }

    /**
     * Disable InstSrc. Provided Objects are withdrawn from Manager classes.
     **/
    PMError disableSource( const ISrcId & isrc_r ) { return activateSource( isrc_r, false ); }

  public:

    /**
     * Set whether this source should be automaticaly enabled on startup.
     *
     * Note that this is something different, than calling enableSource/disableSource.
     * setAutoenable determines the desired state of an InstSrc when the InstSrcManager
     * is started (auto enable it or not). enableSource/disableSource actually
     * enabale/disable an InstSrc (i.e. let it provide/withdraw Objects).
     **/
    PMError setAutoenable( const ISrcId isrc_r, const bool yesno );

    /**
     * Raise priority by one
     **/
    PMError rankUp( const ISrcId isrc_r );

    /**
     * Lower priority by one
     **/
    PMError rankDown( const ISrcId isrc_r );

    /**
     * Adjust isrc priority to be directly behind point. If point is NULL
     * it will get least priority.
     **/
    PMError rankBehind( const ISrcId isrc_r, const ISrcId point_r );

    /**
     * Adjust isrc priority to be directly before point. If point is NULL
     * it will get highest priority.
     **/
    PMError rankBefore( const ISrcId isrc_r, const ISrcId point_r );

    /**
     * To be called by InstSrc Editor to write the current ranking
     * to disk.
     **/
    PMError setNewRanks();

    /**
     * Delete InstSrc. Erase it together with all cached info.
     *
     * <B>Note:</B> The ISrcId passed will be set to NULL. It
     * should be the last ISrcId remembered somewhere. Otherwise
     * deletion is delayed untill all autstanding references were
     * cleared.
     **/
    PMError deleteSource( ISrcId & isrc_r );

    /**
     * Set new Url for InstSrc.
     **/
    PMError rewriteUrl( const ISrcId isrc_r, const Url & newUrl_r );


    typedef std::pair<InstSrc::UniqueID, bool> SrcState;
    typedef std::vector<SrcState>              SrcStateVector;
    typedef std::set<InstSrc::UniqueID>        SrcDelSet;

    /**
     * Rearange known InstSrces rank and default state according to keep_r
     * (highest priority first). InstSrces to delete are given by del_r.
     *
     * In order to perform each currently known InstSrc must occur in either
     * keep_r or del_r. The bool part of a SrcState tells whether the InstSrc
     * should be enabled by default.
     **/
    PMError adjustSources( const SrcStateVector & keep_r, const SrcDelSet del_r );

  public:

    /**
     * Return the number of known InstSrc'es.
     **/
    unsigned knownSources() const { return _knownSources.size(); }

    /**
     * Return the ISrcId's of all known InstSrces. Or enabled InsrSrces
     * only, if enabled_only == true.
     *
     * TODO: Should be replaced by some Iterator.
     **/
    ISrcIdList getSources( const bool enabled_only = false ) const;

    /**
     * Return the ISrcId's of all known InstSrces. Or enabled InsrSrces
     * only, if enabled_only == true.
     *
     * TODO: Should be replaced by some Iterator.
     **/
    void getSources( ISrcIdList & idlist_r, const bool enabled_only = false ) const {
      idlist_r = getSources( enabled_only );
    }

    /**
     * Convert the numeric srcID back to an ISrcId.
     *
     * As an ISrcId basically is a 'constInstSrcPtr', it may get reused by deleting and creating
     * InstSrc'es. Besides this an ISrcId keeps an open reference to the InstSrc. So they are not
     * intended to be stored longer than necessary.
     *
     * Rut each InstSrc owns a unique numeric ID, which may be retrieved by calling @ref InstSrc::srcID.
     * Providing this srcID, the coresponding ISrcId is returned, or NULL, if the InstSrc not found in
     * the list of known InstSrces (e.g. meanwhile deleted).
     **/
    ISrcId getSourceByID( InstSrc::UniqueID srcID_r ) const { return lookupSourceByID( srcID_r ); }

    /**
     * Disable all InstSrc'es.
     **/
    void disableAllSources();

    /**
     * Enable/disable all InstSrc'es according to their default state
     * (see @ref setAutoenable).
     **/
    void enableDefaultSources();

    /**
     * Let a certain InstSrc release it's media.
     * if_removable is true: release if CD/DVD only.
     * @return Error returned by @ref InstSrc::releaseMedia
     **/
    PMError releaseMedia( const ISrcId isrc_r, bool if_removable_r = false ) const;

    /**
     * Let all InstSrc'es release their media.
     * if_removable is true: release if CD/DVD only.
     **/
    PMError releaseAllMedia( bool if_removable_r = false ) const;

    /**
     * Used during installation/Upadte (where caching is disabled)
     * to install the caches on the system. So InstSrc'es will be
     * available in the running system after boot.
     **/
    PMError cacheCopyTo( const Pathname & newRoot_r );


    ///////////////////////////////////////////////////////////////////
    //
    // Temporary interface for handling install order (yast.order)
    // during installation/update.
    //
    ///////////////////////////////////////////////////////////////////

  public:

    typedef std::vector<InstSrc::UniqueID> InstOrder;

  private:

    InstOrder _instOrder;

  public:

    /**
     * Explicitly set an install order.
     *
     * orderedSrcID_r is expected to contain valid numeric InstSrc IDs
     * (see @ref getSourceByID). @ref instOrderSources will then put all
     * enabled InstSrces according to orderedSrcID_r in front of the list.
     * Enabled InstSrces not mentioned in orderedSrcID_r will be appended
     * in default order.
     **/
    void setInstOrder( const InstOrder & newOrder_r );

    /**
     * The default install order is sorted by rank, thus the same as
     * returned by @ref getSources(true).
     **/
    void setDefaultInstOrder();

    /**
     * Return the current InstOrder.
     **/
    const InstOrder & instOrder() const { return _instOrder; }

    /**
     * Return the ISrcId's of all enabled InstSrces in install order.
     * The default install order is sorted by rank, thus the same as
     * returned by @ref getSources(true). See @ref setInstOrder.
     **/
    ISrcIdList instOrderSources() const;

    /**
     * Return the index [0..n] of isrc_r in @ref instOrderSources,
     * or -1 if isrc_r is not enabled or NULL.
     **/
    int instOrderIndex( const ISrcId & isrc_r ) const;

};

///////////////////////////////////////////////////////////////////

#endif // InstSrcManager_h
