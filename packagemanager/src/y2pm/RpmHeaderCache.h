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

  File:       RpmHeaderCache.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef RpmHeaderCache_h
#define RpmHeaderCache_h

#include <iosfwd>

#include <y2util/Pathname.h>

#include <y2pm/PkgName.h>
#include <y2pm/binHeaderCache.h>
#include <y2pm/RpmHeaderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : RpmHeaderCache
/**
 *
 **/
class RpmHeaderCache : public binHeaderCache {

  friend std::ostream & operator<<( std::ostream & str, const RpmHeaderCache & obj );

  RpmHeaderCache & operator=( const RpmHeaderCache & );
  RpmHeaderCache            ( const RpmHeaderCache & );

  private:

    static const PkgNameEd & def_magic();

  protected:

    virtual bool magicOk();

  public:

    RpmHeaderCache( const Pathname & cache_r );
    virtual ~RpmHeaderCache();

    constRpmHeaderPtr getFirst( Pathname & citem_r, int & isSource_r, pos & at_r );
    constRpmHeaderPtr getNext( Pathname & citem_r, int & isSource_r, pos & at_r );

    constRpmHeaderPtr getAt( pos at_r );

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

#endif // RpmHeaderCache_h
