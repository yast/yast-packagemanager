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

  File:       InstSrc.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:	Class for installation sources
		Defines (provides access to)
		- media access (a pointer to MediaAccess)
		- source description (product, version, vendor, ...)
		- contents (list of package, list of selections, ...)
/-*/
#ifndef InstSrc_h
#define InstSrc_h

#include <iosfwd>
#include <list>
#include <string>

#include <y2util/Pathname.h>
#include <y2util/Url.h>

#include <y2pm/InstSrcPtr.h>
#include <y2pm/InstSrcError.h>

#include <y2pm/MediaAccessPtr.h>
#include <y2pm/InstSrcDescrPtr.h>
#include <y2pm/InstSrcDataPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrc
/**
 * @short An InstSrc
 **/
class InstSrc: virtual public Rep {
  REP_BODY(InstSrc);

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;

    /**
     * Known types of InstSrc.
     *
     * For each type there must be a concrete InstSrcData able to handle
     * that kind of InstSrc. Autodetect switch in _init_newMedia must be
     * adjusted, if types are added/removed.
     * @see InstSrcData
     **/
    enum Type {
      T_UNKNOWN,
      // :first entry
      T_UnitedLinux,
      T_TEST_DIST,
      // last entry:
      T_AUTODETECT
    };

    /**
     * Convert Type to string (on save to file).
     * <b>Keep it compatible with fromString.</b>
     **/
    static std::string toString( const Type t );

    /**
     * Restore Type from string (on restore from file).
     * <b>Keep it compatible with toString.</b>
     **/
    static Type fromString( std::string s );

    /**
     * Output Type as string
     **/
    friend std::ostream & operator<<( std::ostream & str, const Type obj );

  private:

    /**
     * Cachedir to use.
     **/
    Pathname _cache;

    /**
     * If true, delete cachedir in destructor.
     **/
    bool _cache_deleteOnExit;

    /**
     * Subpath (below _cache) to cached InstSrcDescr.
     **/
    static const Pathname _c_descr_dir;

    /**
     * Subpath (below _cache) to madia attach point.
     **/
    static const Pathname _c_media_dir;


    /**
     * Helper function to combine _cache and subpaths.
     **/
    Pathname cachePath( const Pathname & sub_r ) const { return( _cache + sub_r ); }

  private:

    /**
     * Used by InstSrc::vconstruct to set up an InstSrc from
     * an existing cachedir.
     *
     * Setup InstSrc from an (existing!) cachedir. cache_media_dir
     * is created on the fly. InstSrcDescr is created from cache_descr_dir.
     * MediaAccess is created, but media remains closed.
     **/
    PMError _init_openCache( const Pathname & cachedir_r );

    /**
     * Used by InstSrc::vconstruct to set up an InstSrc from
     * URL.
     *
     * A new cachedir is created and initialized: cachedir must
     * not exist, but parent dir must. cache_media_dir is created.
     * _cache_deleteOnExit is set, so cachedir will be deleted if
     * further initialisation fails. InstSrc::vconstruct will clear
     * the flag, if initialisation succeeded.
     **/
    PMError _init_newCache( const Pathname & cachedir_r );

    /**
     * Used by InstSrc::vconstruct to set up an InstSrc from
     * URL.
     *
     * Create MediaAcess and open media. Try to get InstSrcDescr
     * from media (if type_r == T_UNKNOWN autodetect type). Media
     * remains open.
     **/
    PMError _init_newMedia( const Url & mediaurl_r, const Pathname & produduct_dir_r,
			    Type type_r );

  protected:

    /**
     * constructor
     *
     * Protected, because InstSrc is to be constructed via
     * InstSrc::vconstruct only.
     **/
    InstSrc();

    /**
     * destructor
     **/
    ~InstSrc();

  protected:

    /**
     * Full path of cache_dir.
     **/
    Pathname cache_dir() const { return _cache; }

    /**
     * Full path of InstSrcDescr cache file.
     **/
    Pathname cache_descr_dir() const { return cachePath( _c_descr_dir ); }

    /**
     * Full path of madia attach point.
     **/
    Pathname cache_media_dir() const { return cachePath( _c_media_dir ); }

  protected:

    /**
     * direct media access
     **/
    MediaAccessPtr _media;

    /**
     * description of media
     **/
    InstSrcDescrPtr _descr;

    /**
     * content of media
     **/
    InstSrcDataPtr _data;

  public:

    /**
     * Access to installation source description
     **/
    constInstSrcDescrPtr descr() const { return _descr; }

    /**
     * Access to installation source data
     **/
    constInstSrcDataPtr  data() const { return _data; }

#if 0
    bool initialized() const { return( _descr != 0 ); }

    PMError may_scan( const Pathname & cachedir_r );

    //-----------------------------
    // general functions

    /**
     * clean up, e.g. remove all caches
     */
    bool Erase();

    /**
     * @return description of Installation source
     * This is needed by the InstSrcMgr
     */
    const InstSrcDescr *getDescription() const;

    /**
     * register this source (store cache files etc)
     * return pathname of saved content file
     */
    const Pathname registerSource (void) const;

    //-----------------------------
    // activation status

    /**
     * return activation status
     */
    bool getActivation() const;

    /**
     * temporary (de)activate source
     */
    void setActivation (bool yesno);

    //-----------------------------
    // source content access

    /**
     * return the number of selections on this source
     */
    int numSelections() const;

    /**
     * return the number of packages on this source
     */
    int numPackages() const;

    /**
     * return the number of patches on this source
     */
    int numPatches() const;

    /**
     * generate PMSolvable objects for each selection on the source
     * @return list of PMSolvablePtr on this source
     */
    const std::list<PMSolvablePtr> *getSelections() const;

    /**
     * generate PMPackage objects for each Item on the source
     * @return list of PMPackagePtr on this source
     * */
    const std::list<PMPackagePtr> *getPackages() const;

    /**
     * generate PMSolvable objects for each patch on the source
     * @return list of PMSolvablePtr on this source
     */
    const std::list<PMSolvablePtr> *getPatches() const;
#endif

  public:

    std::ostream & dumpOn( std::ostream & str ) const;

  public:

    /**
     * Construct an InstSrc from an (existing!) cachedir. The new InstSrc
     * provides a valid description. Media is closed.
     *
     * Return new InstSrc via nsrc_r, or NULL if error.
     *
     * @see _init_openCache
     **/
    static PMError vconstruct( InstSrcPtr & nsrc_r, const Pathname & cachedir_r );

    /**
     * Try to construct an InstSrc from an URL. type_r == T_UNKNOWN means
     * autodetect type of InstSrc located at URL. The new InstSrc
     * provides a valid description. Media is open.
     *
     * Return new InstSrc via nsrc_r, or NULL if error.
     *
     * @see _init_newCache
     * @see _init_newMedia
     **/
    static PMError vconstruct( InstSrcPtr & nsrc_r, const Pathname & cachedir_r,
			       const Url & mediaurl_r, const Pathname & produduct_dir_r,
			       Type type_r = T_AUTODETECT );

};

///////////////////////////////////////////////////////////////////

#endif // InstSrc_h

