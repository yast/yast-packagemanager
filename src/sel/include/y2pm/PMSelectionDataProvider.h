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

  File:       PMSelectionDataProvider.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Common interface to be realized by all SelectionDataProvider.

/-*/
#ifndef PMSelectionDataProvider_h
#define PMSelectionDataProvider_h

#include <iosfwd>
#include <string>

#include <y2pm/PMSelectionDataProviderPtr.h>
#include <y2pm/PMDataProvider.h>

#include <y2pm/PMSelection.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMSelectionDataProvider
/**
 * @short Common interface to be realized by all SelectionDataProvider.
 * @see DataProvider
 **/
class PMSelectionDataProvider : virtual public Rep, public PMDataProvider  {
  REP_BODY(PMSelectionDataProvider);

  protected:

    PMSelectionDataProvider();

    virtual ~PMSelectionDataProvider();

  public:

    /**
     * Selection attribute retrieval.
     * @see PMSelection
     **/
    virtual PkgAttributeValue getAttributeValue( constPMSelectionPtr sel_r,
						 PMSelection::PMSelectionAttribute attr_r ) = 0;
};

///////////////////////////////////////////////////////////////////

#endif // PMSelectionDataProvider_h

