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

   File:       PMPackageDataProvider.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMPackageDataProvider_h
#define PMPackageDataProvider_h

#include <iosfwd>

#include <y2pm/PMPackageDataProviderPtr.h>

#include <y2pm/PMDataProvider.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProvider
/**
 *
 **/
class REP_CLASS(PMPackageDataProvider), public PMDataProvider  {
  REP_BODY(PMPackageDataProvider)

  public:

    PMPackageDataProvider();

    virtual ~PMPackageDataProvider();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMPackageDataProvider_h

