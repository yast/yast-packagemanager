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

#include <y2util/FSize.h>
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
	 * access functions for PMObject/PMSelection attributes
	 */

	virtual const std::string summary(const std::string& lang) const = 0;
	virtual const std::list<std::string> description(const std::string& lang) const = 0;
	virtual const std::list<std::string> insnotify(const std::string& lang) const = 0;
	virtual const std::list<std::string> delnotify(const std::string& lang) const = 0;

	/**
	 * access functions for PMSelection attributes
	 */

	virtual const std::string category () const = 0;
	virtual const bool visible () const = 0;
	virtual const std::list<std::string> suggests() const = 0;
	virtual const std::list<std::string> inspacks(const std::string& lang = "") const = 0;
	virtual const std::list<std::string> delpacks(const std::string& lang = "") const = 0;
	virtual const FSize archivesize() const = 0;
	virtual const std::string order() const = 0;

};

///////////////////////////////////////////////////////////////////

#endif // PMSelectionDataProvider_h

