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
	 * access functions for PMObject attributes
	 */

	virtual const std::string summary(const std::string& lang = "") const;
	virtual const std::list<std::string> description(const std::string& lang = "") const;
	virtual const std::list<std::string> insnotify(const std::string& lang = "") const;
	virtual const std::list<std::string> delnotify(const std::string& lang = "") const;
	virtual const FSize size() const;

	/**
	 * access functions for PMSelection attributes
	 */

	virtual const std::string category () const;
	virtual const bool visible () const;
	virtual const std::list<std::string> suggests() const;
	virtual const std::list<std::string> inspacks(const std::string& lang = "") const;
	virtual const std::list<std::string> delpacks(const std::string& lang = "") const;
	virtual const FSize archivesize() const;
	virtual const std::string order() const;

    /**
     * Selection attribute retrieval.
     * @see PMSelection
     **/
    virtual PkgAttributeValue getAttributeValue( constPMSelectionPtr sel_r,
						 PMSelection::PMSelectionAttribute attr_r ) = 0;
};

///////////////////////////////////////////////////////////////////

#endif // PMSelectionDataProvider_h

