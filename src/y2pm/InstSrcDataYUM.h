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

  Purpose: YUM installation source interface

/-*/
#ifndef InstSrcDataYUM_h
#define InstSrcDataYUM_h

#include <iosfwd>
#include <list>

#include "y2pm/InstSrcDataYUMPtr.h"
#include "y2pm/InstSrcData.h"

///////////////////////////////////////////////////////////////////
namespace YUM
{ /////////////////////////////////////////////////////////////////

  class Impl; // InstSrcDataYUM implementation
  typedef VarPtr<Impl> ImplPtr;

  /////////////////////////////////////////////////////////////////
} // namespace YUM
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDataYUM
/**
 *
 **/
class InstSrcDataYUM : public InstSrcData
{
  REP_BODY(InstSrcDataYUM);

  public:
    /**
     * Default error class
     **/
    typedef InstSrcError Error;

    /**
     * Dtor
     **/
    virtual
    ~InstSrcDataYUM();

    /**
     * Dump debug data
     **/
    virtual std::ostream &
    dumpOn( std::ostream & str ) const;

    /**
     * Return list of Packages on this source
     **/
    virtual const std::list<PMPackagePtr> &
    getPackages() const;

  private:
    /**
     * Ctor called from @ref tryGetData
     **/
    InstSrcDataYUM( const Pathname & repodataDir_r );

    /**
     * InstSrcDataYUM implementation
     **/
    friend class YUM::Impl;
    YUM::ImplPtr _impl;

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
    static PMError
    tryGetDescr( InstSrcDescrPtr & ndescr_r,
                 const InstSrcPtr  source_r,
                 MediaAccessPtr    media_r,
                 const Pathname &  product_dir_r,
                 const Url &       mediaurl_r );

    /**
     * Any concrete InstSrcData must realize this, as it knows the expected
     * layout on the media. Expect MediaAccessPtr to be open and attached.
     *
     * try to find selection/package/patch information on the media
     *
     * Return the InstSrcData retrieved from the media via ndata_r,
     * or NULL and PMError set.
     **/
    static PMError
    tryGetData( InstSrcDataPtr & ndata_r,
                const InstSrcPtr source_r );
};
///////////////////////////////////////////////////////////////////

#endif // InstSrcDataYUM_h

