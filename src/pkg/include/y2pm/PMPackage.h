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

  enum PMPackageAttribute
  {
      ATTR_BUILDTIME = PMOBJ_NUM_ATTRIBUTES,
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

      PKG_NUM_ATTRIBUTES
  };

  protected:

    PkgArch _arch;

    PMPackageDataProviderPtr _dataProvider;

  public:

    PMPackage( const PkgName &    name_r,
	       const PkgEdition & edition_r,
	       const PkgArch &    arch_r );

    virtual ~PMPackage();

  public:

    const PkgArch& arch() const { return _arch; }

  public:

//    virtual std::string Summary() const;

    /** get attributes like Summary, Description, Group etc.
     *
     * @param attr Attribute number
     * @return Attribute value
     * */
    std::string getAttributeValue(PMPackageAttribute attr);

    std::string getAttributeValue(PMObjectAttribute attr);;

    /** get the name of an attribute
     *
     * @param attr Attribute number
     * @return Attribute name
     * */
    std::string getAttributeName(PMPackageAttribute attr);

    std::string getAttributeName(PMObjectAttribute attr);

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
