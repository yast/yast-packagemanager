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

  Purpose: Common interface to be realized by all PackageDataProvider.

/-*/
#ifndef PMPackageDataProvider_h
#define PMPackageDataProvider_h

#include <iosfwd>
#include <string>

#include <y2util/FSize.h>
#include <y2util/Date.h>

#include <y2pm/PMPackageDataProviderPtr.h>
#include <y2pm/PMDataProvider.h>

#include <y2pm/PMPackage.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageDataProvider
/**
 * @short Common interface to be realized by all PackageDataProvider.
 * @see DataProvider
 **/
class PMPackageDataProvider : virtual public Rep, public PMDataProvider  {
  REP_BODY(PMPackageDataProvider);

  protected:

    PMPackageDataProvider();

    virtual ~PMPackageDataProvider();

  public:

	/**
	 * access functions for PMPackage attributes
	 */

	virtual const Date buildtime () const = 0;
	virtual const std::string buildhost () const = 0;
	virtual const Date installtime () const = 0;
	virtual const std::string distribution () const = 0;
	virtual const std::string vendor () const = 0;
	virtual const std::string license () const = 0;
	virtual const std::string packager () const = 0;
	virtual const std::string group () const = 0;
	virtual const std::list<std::string> changelog () const = 0;
	virtual const std::string url () const = 0;
	virtual const std::string os () const = 0;
	virtual const std::list<std::string> prein () const = 0;
	virtual const std::list<std::string> postin () const = 0;
	virtual const std::list<std::string> preun () const = 0;
	virtual const std::list<std::string> postun () const = 0;
	virtual const std::string sourcerpm () const = 0;
	virtual const FSize archivesize () const = 0;
	virtual const std::list<std::string> authors () const = 0;
	virtual const std::list<std::string> filenames () const = 0;
	// suse packages values
	virtual const std::list<std::string> recommends () const = 0;
	virtual const std::list<std::string> suggests () const = 0;
	virtual const std::string location () const = 0;
	virtual const std::list<std::string> keywords () const = 0;

};

///////////////////////////////////////////////////////////////////

#endif // PMPackageDataProvider_h

