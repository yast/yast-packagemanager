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

  File:       RpmLibHeaderCache.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef RpmLibHeaderCache_h
#define RpmLibHeaderCache_h

#include <iosfwd>

#include <y2util/Pathname.h>

#include <y2pm/PkgName.h>
#include <y2pm/binHeaderCache.h>
#include <y2pm/RpmLibHeaderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : RpmLibHeaderCache
/**
 *
 **/
class RpmLibHeaderCache : public binHeaderCache {

  friend std::ostream & operator<<( std::ostream & str, const RpmLibHeaderCache & obj );

  RpmLibHeaderCache & operator=( const RpmLibHeaderCache & );
  RpmLibHeaderCache            ( const RpmLibHeaderCache & );

  private:

    static const PkgNameEd & def_magic();

  protected:

    virtual bool magicOk();

  public:

    RpmLibHeaderCache( const Pathname & cache_r );
    virtual ~RpmLibHeaderCache();

    constRpmLibHeaderPtr getFirst( Pathname & citem_r, int & isSource_r, pos & at_r );
    constRpmLibHeaderPtr getNext( Pathname & citem_r, int & isSource_r, pos & at_r );

    constRpmLibHeaderPtr getAt( pos at_r );

  public:

    struct buildOpts {
      bool recurse;
      buildOpts()
	: recurse( false )
      {}
    };

    static int buildHeaderCache( const Pathname & cache_r, const Pathname & pkgroot_r,
				 const buildOpts & options_r = buildOpts() );
};

///////////////////////////////////////////////////////////////////

#endif // RpmLibHeaderCache_h
