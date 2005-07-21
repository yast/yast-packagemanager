/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                         (C) SuSE Linux Products GmbH |
\----------------------------------------------------------------------/

  File:       YUMImpl.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: YUM installation source implementation

/-*/
#ifndef YUMImpl_h
#define YUMImpl_h

#include <iosfwd>

#include "y2pm/InstSrcDataYUM.h"
#include "y2pm/YUMParser.h"
#include "YUMRepodata.h"

///////////////////////////////////////////////////////////////////
namespace YUM
{ /////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  //
  //	CLASS NAME : Impl
  /**
   *
   **/
  class Impl : public Rep
  {
    Impl & operator=( const Impl & );
    Impl            ( const Impl & );

  public:
    /**
     * Ctor
     **/
    Impl( InstSrcDataYUM & parent, const Pathname & repoDir_r );

    /**
     * Dtor
     **/
    virtual
    ~Impl()
    {}

    /**
     * Dump debug data
     **/
    virtual std::ostream &
    dumpOn( std::ostream & str ) const;

    /**
     * Return list of Packages on this source
     **/
    const std::list<PMPackagePtr> &
    getPackages() const
    { return _packages; }

    const Repodata &
    repodata() const
    { return _repodata; }

  private:

    bool
    scanPrimary( YUMPrimaryParser & iter_r );

  private:
    /**
     * Back link to interface
     **/
    InstSrcDataYUM & _parent;

    /**
     * Metadata
     **/
    Repodata _repodata;

    /**
     * The packages
     **/
    std::list<PMPackagePtr> _packages;
};

///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////
} // namespace YUM
///////////////////////////////////////////////////////////////////
#endif // YUMImpl_h
