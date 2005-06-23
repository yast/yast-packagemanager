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

  File:       InstSrcDataYUM.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: YUM installation source

/-*/
#ifndef InstSrcDataYUM_h
#define InstSrcDataYUM_h

#include <iosfwd>
#include <list>

#include <y2pm/InstSrcDataYUMPtr.h>
#include <y2pm/InstSrcData.h>
#include <y2pm/PMPackage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDataYUM
/**
 *
 **/
class InstSrcDataYUM : public InstSrcData {
  REP_BODY(InstSrcDataYUM);

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;

  private:

    /**
     * List of Packages on this source
     **/
    std::list<PMPackagePtr> _packages;

  protected:

    /**
     * Constructor called from @ref tryGetData
     **/
    InstSrcDataYUM( const Pathname & repodataDir_r );

    PMError providePkgToInstall( const Pathname & pkgfile_r, Pathname & path_r ) const;

  public:

    virtual ~InstSrcDataYUM();

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

#endif // InstSrcDataYUM_h

