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

#include <y2pm/PMPackageDataProviderPtr.h>
#include <y2pm/PMPackagePtr.h>

#include <y2pm/PMObject.h>
#include <y2pm/PkgArch.h>

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
      ATTR_ARCH,
      ATTR_PREIN,
      ATTR_POSTIN,
      ATTR_PREUN,
      ATTR_POSTUN,
      ATTR_SOURCERPM,
      ATTR_ARCHIVESIZE,
      ATTR_AUTHOR,
      ATTR_FILENAMES,
      // last entry:
      PKG_NUM_ATTRIBUTES
    };

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

    PkgArch _arch;

    PMPackageDataProviderPtr _dataProvider;

  protected:

    /**
     * Provide DataProvider access to the underlying Object
     **/
    virtual PMDataProviderPtr dataProvider() const { return _dataProvider; }

  public:

    PMPackage( const PkgName &    name_r,
	       const PkgEdition & edition_r,
	       const PkgArch &    arch_r );

    virtual ~PMPackage();

  public:

    const PkgArch& arch() const { return _arch; }

  public:

    /** assign a data provider
     * @param dataprovider the dataprovider
     * */
    void setDataProvider(PMPackageDataProviderPtr dataprovider)
    { _dataProvider = dataprovider; }

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // PMPackage_h
