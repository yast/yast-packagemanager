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
	 * access functions for PMObject attributes
	 */

	virtual const std::string summary () const;
	virtual const std::list<std::string> description () const;
	virtual const std::list<std::string> insnotify () const;
	virtual const std::list<std::string> delnotify () const;
	virtual const FSize size () const;

	/**
	 * access functions for PMPackage attributes
	 */

	virtual const Date buildtime () const;
	virtual const std::string buildhost () const;
	virtual const Date installtime () const;
	virtual const std::string distribution () const;
	virtual const std::string vendor () const;
	virtual const std::string license () const;
	virtual const std::string packager () const;
	virtual const std::string group () const;
	virtual const std::list<std::string> changelog () const;
	virtual const std::string url () const;
	virtual const std::string os () const;
	virtual const std::list<std::string> prein () const;
	virtual const std::list<std::string> postin () const;
	virtual const std::list<std::string> preun () const;
	virtual const std::list<std::string> postun () const;
	virtual const std::string sourcerpm () const;
	virtual const FSize archivesize () const;
	virtual const std::list<std::string> authors () const;
	virtual const std::list<std::string> filenames () const;
	// suse packages values
	virtual const std::list<std::string> recommends () const;
	virtual const std::list<std::string> suggests () const;
	virtual const std::string location () const;
	virtual const std::list<std::string> keywords () const;

    /**
     * Package attribute retrieval.
     * @see PMPackage
     **/
    virtual PkgAttributeValue getAttributeValue( constPMPackagePtr pkg_r,
						 PMPackage::PMPackageAttribute attr_r ) = 0;

};

///////////////////////////////////////////////////////////////////

#endif // PMPackageDataProvider_h

