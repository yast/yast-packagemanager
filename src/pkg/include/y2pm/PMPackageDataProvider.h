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
	 * access functions for PMObject attributes
	 */

	const std::string summary() const;
	const std::list<std::string> description() const;
	const std::list<std::string> insnotify() const;
	const std::list<std::string> delnotify() const;

	/**
	 * access functions for PMPackage attributes
	 */

	const long buildtime() const;
	const std::string buildhost() const;
	const long installtime() const;
	const std::string distribution() const;
	const std::string vendor() const;
	const std::string license() const;
	const std::string packager() const;
	const std::string group() const;
	const std::list<std::string> changelog() const;
	const std::string url() const;
	const std::string os() const;
	const std::list<std::string> prein() const;
	const std::list<std::string> postin() const;
	const std::list<std::string> preun() const;
	const std::list<std::string> postun() const;
	const std::string sourcerpm() const;
	const long archivesize() const;
	const std::list<std::string> authors() const;
	const std::list<std::string> filenames() const;
	// suse packages values
	const std::list<std::string> recommends() const;
	const std::list<std::string> suggests() const;
	const std::list<std::string> location() const;
	const std::list<std::string> keywords() const;

    /**
     * Package attribute retrieval.
     * @see PMPackage
     **/
    virtual PkgAttributeValue getAttributeValue( constPMPackagePtr pkg_r,
						 PMPackage::PMPackageAttribute attr_r ) = 0;

};

///////////////////////////////////////////////////////////////////

#endif // PMPackageDataProvider_h

