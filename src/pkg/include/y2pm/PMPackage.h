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

   File:       PMPackage.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMPackage_h
#define PMPackage_h

#include <iosfwd>

#include <y2pm/PMPackageDataProviderPtr.h>
#include <y2pm/PMPackagePtr.h>

#include <y2pm/PMObject.h>
#include <y2pm/PkgArch.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackage
/**
 * The Package.
 **/
class REP_CLASS(PMPackage), public PMObject {
  REP_BODY(PMPackage)

  protected:

    PkgArch _arch;

    PMPackageDataProviderPtr _dataProvider;

  public:

    PMPackage( const PkgName &    name_r,
	       const PkgEdition & edition_r,
	       const PkgArch &    arch_r );

    virtual ~PMPackage();

  public:

    const PkgArch& arch() const { return _arch; }

  public:

    virtual std::string Summary() const;

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMPackage_h
