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

#include <y2pm/PMSelectionPtr.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMYouPatchPtr.h>

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

  private:

    friend class InstSrcManager;

    /**
     * Called from InstSrcManager on add to it's known sources pool.
     * Unsets _cache_deleteOnExit, and writes description cache.
     **/
    void _mgr_attach();

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
     * Subpath (below _cache) to cached InstSrcData.
     **/
    static const Pathname _c_data_dir;

    /**
     * Subpath (below _cache) to madia attach point.
     **/
    static const Pathname _c_media_dir;

    /**
     * Helper function to combine _cache and subpaths.
     **/
    Pathname cachePath( const Pathname & sub_r ) const { return( _cache + sub_r ); }

  public:

    /**
     * Full path of cache_dir.
     **/
    Pathname cache_dir() const { return _cache; }

    /**
     * Full path of InstSrcDescr cache dir. If it does not exist, no chache write
     * is desired.
     **/
    Pathname cache_descr_dir() const { return cachePath( _c_descr_dir ); }

    /**
     * Full path of InstSrcData cache dir. If it does not exist, no chache write
     * is desired.
     **/
    Pathname cache_data_dir() const { return cachePath( _c_data_dir ); }

    /**
     * Full path of (prefered) madia attach point.
     **/
    Pathname cache_media_dir() const { return cachePath( _c_media_dir ); }

  private:

    /**
     * If true, data and description cache directories should be created
     * and used. (false during installation, when no system is present)
     **/
    bool mayUseCache() const;

    /**
     * Used by InstSrc::vconstruct to set up an InstSrc from
     * an existing cachedir.
     *
     * Setup InstSrc from an (existing!) cachedir. cache_descr_dir
     * must exist. InstSrcDescr is created from cache_descr_dir.
     * MediaAccess is created, but media remains closed.
     **/
    PMError _init_openCache( const Pathname & cachedir_r );

    /**
     * Used by InstSrc::vconstruct to set up an InstSrc from
     * URL.
     *
     * A new cachedir is created and initialized: cachedir must
     * not exist, but parent dir must.
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

  private:

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
     * Const access to installation source description
     **/
    constInstSrcDescrPtr descr() const { return _descr; }

    /**
     * Access to installation source description
     **/
    InstSrcDescrPtr descr() { return _descr; }

    /**
     * Const access to installation source data
     **/
    constInstSrcDataPtr data() const { return _data; }

    /**
     * Access to installation source data
     **/
    InstSrcDataPtr data() { return _data; }

    /**
     * Const access to media
     **/
    constMediaAccessPtr media() const { return _media; }

    /**
     * Access to media
     **/
    MediaAccessPtr media() { return _media; }

  public:

    /**
     * From InstSrcManager: Provide concrete InstSrcData according to Type
     * info stored in _descr. InstSrcData must provide Objects located on
     * the media to the appropriate Manager classes.
     **/
    PMError enableSource();

    /**
     * From InstSrcManager: Release concrete InstSrcData. InstSrcData must
     * withdraw provided Objects from Manager classes.
     **/
    PMError disableSource();

    /**
     * True if enabled (i.e. providing data to Manager classes ).
     **/
    bool enabled() const { return _data; }

    /**
     * Sync InstSrcDescr and InstSrcData to cache
     **/
    PMError writeCache();

  private:

	/**
	 * list of allowed subdirs below _descr->datadir() for package installation
	 *
	 * set when enabling source
	 * used in provideLocation
	 *
	 * (see ARCH.x line in content file for further information)
	 */
	std::list<Pathname> _datasubdirs;

	/** media change callback
	    - error type
		1 = no media found
		2 = wrong media number
		3 = wrong product
		4 = wrong release
	    - expected media #
	    - found media # (0 == none)
	    return 0: ok, 1: skip, -1 cancel
	 */
	int (*_mediachangefunc)(int, int, int, void*);
	int (*_mediaerrorfunc)(const std::string&, void*);

	/** arbitrary data to pass back for progress callback */
	void* _mediachangedata;
	void* _mediaerrordata;

	int _medianr;

  public:
	/** set callback function for media change
	 *
	 * @param func callback function
	 * @param data arbitrary data to pass when function is called
	 * */
	void setMediaChangeCallback(int (*func)(int,int,int,void*), void* data)
	{
	    _mediachangefunc = func;
	    _mediachangedata = data;
	}

	/** set callback function for media error
	 *
	 * @param func callback function
	 * @param data arbitrary data to pass when function is called
	 * */
	void setMediaErrorCallback(int (*func)(const std::string&,void*), void* data)
	{
	    _mediaerrorfunc = func;
	    _mediaerrordata = data;
	}

	/**
	 * provide media by number
	 *
	 */
	PMError provideMedia (int medianr);

	/**
	 * provide package via medianr, name, and sub-dir
	 *
	 * name is the rpm file name
	 * dir is the directory below <DATADIR>
	 *
	 * returns local path or empty on error
	 * uses media change callback
	 */
	Pathname providePackage (int medianr, const Pathname& name, const Pathname& dir);

	/**
	 * provide file via medianr and path
	 *
	 * path is relavite to the media root
	 *
	 * returns local path or empty on error
	 * uses media change callback
	 */
	Pathname provideFile (int medianr, const Pathname& path);

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

