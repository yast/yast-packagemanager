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

  Purpose: Common interface to be realized by all DataProvider.

/-*/
#ifndef PMDataProvider_h
#define PMDataProvider_h

#include <iosfwd>

#include <y2pm/PMDataProviderPtr.h>

#include <y2pm/PMObject.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMDataProvider
/**
 * @short Common interface to be realized by all DataProvider.
 **/
class PMDataProvider : virtual public Rep {
  REP_BODY(PMDataProvider);

  protected:

    PMDataProvider();

    virtual ~PMDataProvider();

  public:

    /**
     * Object attribute retrieval.
     * @see PMObject
     **/
    virtual PkgAttributeValue getAttributeValue( constPMObjectPtr obj_r,
						 PMObject::PMObjectAttribute attr_r ) = 0;
};

///////////////////////////////////////////////////////////////////

#endif // PMDataProvider_h
