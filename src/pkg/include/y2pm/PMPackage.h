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

    File:       PMPackage.h

    Author:     Michael Andres <ma@suse.de>
    Maintainer: Michael Andres <ma@suse.de>

    Purpose: Defines the Package object.

/-*/
#ifndef PMPackage_h
#define PMPackage_h

#include <iosfwd>
#include <string>

//#include <y2util/Date.h>
//#include <y2util/FSize.h>

#include <y2pm/PMPackageDataProviderPtr.h>
#include <y2pm/PMPackagePtr.h>

#include <y2pm/PMObject.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackage
/**
 * The Package.
 **/
class PMPackage : virtual public Rep, public PMObject {
    REP_BODY(PMPackage);

    public:

	/**
	 * Attributes provided by PMPackage
	 **/
	enum PMPackageAttribute {
	PKG_ATTR_BEGIN = PMOBJ_NUM_ATTRIBUTES,
	// RPM values
	ATTR_BUILDTIME = PKG_ATTR_BEGIN,
	ATTR_BUILDHOST,
	ATTR_INSTALLTIME,
	ATTR_DISTRIBUTION,
	ATTR_VENDOR,
	ATTR_LICENSE,
	ATTR_PACKAGER,
	ATTR_GROUP,
	ATTR_CHANGELOG,
	ATTR_URL,
	ATTR_OS,
	ATTR_PREIN,
	ATTR_POSTIN,
	ATTR_PREUN,
	ATTR_POSTUN,
	ATTR_SOURCERPM,
	ATTR_ARCHIVESIZE,
	ATTR_AUTHORS,
	ATTR_FILENAMES,
	// SuSE packages values
	ATTR_RECOMMENDS,
	ATTR_SUGGESTS,
	ATTR_LOCATION,
	ATTR_KEYWORDS,
	// last entry:
	PMPKG_NUM_ATTRIBUTES
	};

	// overlay virtual PMObject functions
	const std::string summary() const;
	const std::list<std::string> description() const;
	const std::list<std::string> insnotify() const;
	const std::list<std::string> delnotify() const;

	// PMPackage functions come here
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
	 * Get attribute name as string.
	 **/
	std::string getAttributeName(PMPackageAttribute attr) const;

	/**
	 * Access to base class getAttributeName
	 **/
	PMObject::getAttributeName;

	/**
	 * Get attribute value
	 **/
	PkgAttributeValue getAttributeValue(PMPackageAttribute attr) const;

	/**
	 * Access to base class getAttributeValue
	 **/
	PMObject::getAttributeValue;

    protected:

	PMPackageDataProviderPtr _dataProvider;

    public:

	/**
	 * Provide DataProvider access to the underlying Object
	 **/
	virtual PMDataProviderPtr dataProvider() const { return _dataProvider; }

	/**
	 * access functions to attributes
	 */
//    virtual const Date& buildtime() const;

    public:

	PMPackage( const PkgName &    name_r,
	     const PkgEdition & edition_r,
	     const PkgArch &    arch_r,
	     PMPackageDataProviderPtr dataProvider_r );

	virtual ~PMPackage();

    public:

	virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMPackage_h
