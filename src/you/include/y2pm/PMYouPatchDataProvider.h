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

  File:       PMYouPatchDataProvider.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Common interface to be realized by all YouPatchDataProvider.

/-*/
#ifndef PMYouPatchDataProvider_h
#define PMYouPatchDataProvider_h

#include <iosfwd>
#include <string>

#include <y2pm/PMYouPatchDataProviderPtr.h>
#include <y2pm/PMDataProvider.h>

#include <y2pm/PMYouPatch.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchDataProvider
/**
 * @short Common interface to be realized by all YouPatchDataProvider.
 * @see DataProvider
 **/
class PMYouPatchDataProvider : virtual public Rep, public PMDataProvider  {
  REP_BODY(PMYouPatchDataProvider);

  protected:

    PMYouPatchDataProvider();

    virtual ~PMYouPatchDataProvider();

  public:

	/**
	 * provide PMObject attributes
	 */
	const std::string summary() const;
	const std::list<std::string> description() const;
	const std::list<std::string> insnotify() const;
	const std::list<std::string> delnotify() const;

    /**
     * YouPatch attribute retrieval.
     * @see PMYouPatch
     **/
    virtual PkgAttributeValue getAttributeValue( constPMYouPatchPtr sel_r,
						 PMYouPatch::PMYouPatchAttribute attr_r ) = 0;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchDataProvider_h

