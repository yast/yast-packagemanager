/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Provide product-specific information for YOU.

/-*/
#ifndef PMYOUPRODUCT_H
#define PMYOUPRODUCT_H

#include <list>
#include <string>

#include <y2util/Url.h>
#include <y2util/Pathname.h>
#include <y2util/LangCode.h>

#include <y2pm/PMError.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/PkgArch.h>

#include <y2pm/PMYouServers.h>
#include <y2pm/InstSrcDescrPtr.h>

#include <y2pm/PMYouProductPtr.h>

class SysConfig;

/**
  This class provides product-specific information for the online update.
*/
class PMYouProduct : public CountedRep
{
  REP_BODY( PMYouProduct );

  public:
    /**
      Constructor.
    */
    PMYouProduct( const constInstSrcDescrPtr &, PMYouSettings & );

    PMYouProduct( const string &product, const string &version,
                  const string &baseArch, PMYouSettings & );

    /**
      Destructor
    */
    ~PMYouProduct();

    /**
      Init object.
      
      @param product  Product name
      @param version  Product version
      @param baseArch Base architecture of product
      @param youUrl   URL used to get YOU server list
      @param path     Path on server to directory containing patches, rpms and
                      scripts directories
      @param business True, if product is a business product, needing
                      authentification on the server
    */
    void init( const std::string &product, const std::string &version,
               const std::string &baseArch, const std::string &youUrl,
               const std::string &path, bool business );
    /**
      Init object with given product, version and baseArch values.
    */
    void init( const std::string &product, const std::string &version,
               const std::string &baseArch );

    /**
      Set path on server to directory containing the "patches", "rpms" and
      "scripts" directories. This path depends on the product.
    */
    void setPatchPath( const Pathname & );
    /**
      Return path to patches on server.
    */
    Pathname patchPath();

    /**
      Return path to RPM for base architecture.
      
      @param pkg      Pointer to package
      @param patchRpm If true, return path to patch RPM, if false, return path
                      to full RPM.
    */
    Pathname rpmPath( const PMPackagePtr &pkg, bool patchRpm = false );
    /**
      Return path to RPM.
      
      @param pkg      Pointer to package
      @param arch     Architecture of RPM.
      @param patchRpm If true, return path to patch RPM, if false, return path
                      to full RPM.
    */
    Pathname rpmPath( const PMPackagePtr &pkg, const std::string & arch,
                      bool patchRpm = false );

    /**
      Return path of script.
      
      @param scriptName Name of script
    */
    Pathname scriptPath( const std::string &scriptName );

    /**
      Return path of script at download location.
      
      @param scriptName Name of script
    */
    
    Pathname localScriptPath( const std::string &scriptName );

    /**
      Return name of product.
    */
    std::string product();
    
    /**
      Return name of distribution.
    */
    std::string distProduct();
    
    /**
      Return version of product.
    */
    std::string version();
    
    /**
      Return base architecture.
    */
    PkgArch baseArch();

    /**
      Set list of compatible architectures.
    */
    void setArchs( const std::list<PkgArch> & );
    /**
      Return list of compatible architectures.
    */
    std::list<PkgArch> archs();

    /**
      Return architecture.
    */
    PkgArch arch();

    /**
      Return whether the product is a business product or not. Business products
      require authentification on the server.
    */
    bool businessProduct();

    /**
      Return base URL where list of YOU servers is read from.

      Return default URL, if the information can't be read from the product
      information.
    */
    std::string youUrl();

    /**
      Set patch files associated with this product.
    */
    void setPatchFiles( const std::list<std::string> &patchFiles );

    /**
      Return patch files associated with this product.
    */
    std::list<std::string> patchFiles() const;

  protected:
    void init( const std::string &path );

  private:
    constInstSrcDescrPtr _productDescr;
    PMYouSettings &_settings;

    Pathname _patchPath;
    Pathname _rpmPath;
    Pathname _scriptPath;

    std::string _product;
    std::string _version;
    PkgArch _baseArch;

    std::string _distProduct;

    std::list<PkgArch> _archs;
    PkgArch _arch;
    
    std::string _youUrl;
    bool _businessProduct;
    
    std::list<std::string> _patchFiles;
};

#endif
