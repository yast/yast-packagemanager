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

  File:       InstSrcDataPLAIN.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef InstSrcDataPLAIN_h
#define InstSrcDataPLAIN_h

#include <iosfwd>
#include <list>

#include <y2pm/InstSrcDataPLAINPtr.h>
#include <y2pm/InstSrcData.h>
#include <y2pm/PMPackage.h>
#include <y2pm/RpmHeaderPtr.h>

class RpmHeaderCache;

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDataPLAIN
/**
 *
 **/
class InstSrcDataPLAIN : virtual public Rep, public InstSrcData {
  REP_BODY(InstSrcDataPLAIN);

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;

  private:

    RpmHeaderCache & _cache;

    /**
     * List of Packages on this source
     **/
    std::list<PMPackagePtr> _packages;

  protected:

    /**
     * Constructor called from @ref tryGetData
     **/
    InstSrcDataPLAIN( const Pathname & cachefile_r );


    friend class PMPLAINPackageDataProvider;

    /**
     * Called from PMPLAINPackageDataProvider to fill its header cache
     **/
    constRpmHeaderPtr getHeaderAt( unsigned _cachepos ) const;

    PMError providePkgToInstall( const Pathname & pkgfile_r, Pathname & path_r ) const;

  public:

    virtual ~InstSrcDataPLAIN();

    /**
     * Return list of Packages on this source
     **/
    const std::list<PMPackagePtr> & getPackages() const { return _packages; }

  public:

    /**
     * Any concrete InstSrcData must realize this, as it knows the expected
     * layout on the media. Expect MediaAccessPtr to be open and attached.
     *
     * try to find product/content information on the media
     *
     * Return the InstSrcDescr retrieved from the media via ndescr_r,
     * or NULL and PMError set.
     **/
    static PMError tryGetDescr( InstSrcDescrPtr & ndescr_r,
				const InstSrcPtr source_r,
				MediaAccessPtr media_r,
				const Pathname & product_dir_r,
				const Url & mediaurl_r );

    /**
     * Any concrete InstSrcData must realize this, as it knows the expected
     * layout on the media. Expect MediaAccessPtr to be open and attached.
     *
     * try to find selection/package/patch information on the media
     *
     * Return the InstSrcData retrieved from the media via ndata_r,
     * or NULL and PMError set.
     **/
    static PMError tryGetData( InstSrcDataPtr & ndata_r, const InstSrcPtr source_r );

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcDataPLAIN_h

