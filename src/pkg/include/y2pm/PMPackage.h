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
class REP_CLASS(PMPackage), public PMObject {
  REP_BODY(PMPackage)

  public:

  enum PMPackageAttribute
  {
      PKG_BUILDTIME = PMObject::PMOBJ_NUM_ATTRIBUTES,
      PKG_BUILDHOST,
      PKG_INSTALLTIME,
      PKG_DISTRIBUTION,
      PKG_VENDOR,
      PKG_LICENSE,
      PKG_PACKAGER,
      PKG_GROUP,
      PKG_CHANGELOG,
      PKG_URL,
      PKG_OS,
      PKG_ARCH,
      PKG_PREIN,
      PKG_POSTIN,
      PKG_PREUN,
      PKG_POSTUN,
      PKG_SOURCERPM,
      PKG_ARCHIVESIZE,

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
