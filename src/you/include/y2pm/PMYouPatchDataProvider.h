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

    PMYouPatchPtr _patch;

  public:

    PMYouPatchDataProvider();

    virtual ~PMYouPatchDataProvider();

  public:

    void setPatch( const PMYouPatchPtr &p ) { _patch = p; }

	/**
	 * hint before accessing multiple attributes
	 */
	void startRetrieval() const { return PMDataProvider::startRetrieval(); }

	/**
	 * hint after accessing multiple attributes
	 */
	void stopRetrieval() const { return PMDataProvider::stopRetrieval(); }

	/**
	 * provide PMObject attributes
	 */
	const std::string summary() const;
	const std::list<std::string> description() const;
	const std::list<std::string> insnotify() const;
	const std::list<std::string> delnotify() const;
	const FSize size() const;

};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchDataProvider_h

