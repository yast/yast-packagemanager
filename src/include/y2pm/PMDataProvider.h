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
	 * hint before accessing multiple attributes
	 */
	virtual void startRetrieval() const = 0;

	/**
	 * hint after accessing multiple attributes
	 */
	virtual void stopRetrieval() const = 0;

	/**
	 * access functions for PMObject attributes
	 */

	virtual const std::string summary () const = 0;
	virtual const std::list<std::string> description () const = 0;
	virtual const std::list<std::string> insnotify () const = 0;
	virtual const std::list<std::string> delnotify () const = 0;
	virtual const FSize size () const = 0;

};

///////////////////////////////////////////////////////////////////

#endif // PMDataProvider_h
