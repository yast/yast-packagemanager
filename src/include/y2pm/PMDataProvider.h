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

   File:       PMDataProvider.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef PMDataProvider_h
#define PMDataProvider_h

#include <iosfwd>

#include <y2pm/PMDataProviderPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMDataProvider
/**
 * @short Interface class for stuff common to all DataProvider
 **/
class PMDataProvider : virtual public Rep {
  REP_BODY(PMDataProvider);

  public:

    PMDataProvider();

    virtual ~PMDataProvider();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMDataProvider_h
