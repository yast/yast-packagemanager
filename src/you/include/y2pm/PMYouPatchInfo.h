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

  File:       PMYouPatchInfo.h

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Utility class for reading the patch information

/-*/
#ifndef PMYouPatchInfo_h
#define PMYouPatchInfo_h

#include <list>
#include <string>

#include <y2util/CommonPkdParser.h>
#include <y2util/Url.h>
#include <y2util/Pathname.h>

#include <y2pm/PMError.h>

#include <y2pm/PMYouPatchPtr.h>
#include <y2pm/PMYouPackageDataProviderPtr.h>
#include <y2pm/PMYouPatchTags.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchPaths
/**
 * Pathes for patches.
 **/
class PMYouPatchPaths {

  public:
    PMYouPatchPaths( const std::string &product, const std::string &version,
                     const std::string &arch );

    void setPatchPath( const Pathname & );
    Pathname patchPath();

    Pathname rpmPath( const PMPackagePtr &pkg );

    void setPatchUrl( const Url & );
    Url patchUrl();

    Pathname localDir();

    PkgArch arch();

    bool businessProduct();

    PMError requestServers( const std::string &url = "",
                            const std::string &file = "" );

    std::list<Url> servers();
    
    Url defaultServer();

  private:
    Pathname _patchPath;
    Pathname _rpmPath;
    Url _patchUrl;
    
    PkgArch _arch;
    
    std::list<Url> _servers;

    bool _businessProduct;
};


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchInfo
/**
 * Patch information.
 **/
class PMYouPatchInfo {

  public:
    /**
     * Constructor
     *
     * @param lang  language to be parsed.
     **/
    PMYouPatchInfo( const std::string &lang );

    /**
     * Destructor
     **/
    ~PMYouPatchInfo();

    /**
     * Get patches from specified location.
     *
     * @param paths   Object holding the path location information.
     * @param patches List of patch objects where the results are stored.
     **/
    PMError getPatches( PMYouPatchPaths *paths,
                        std::list<PMYouPatchPtr> &patches );
    
    /**
     * Read patch info files from directory.
     *
     * @param baseUrl    Base of URL where patches are located.
     * @param patchPath  Path of patch directory relative to baseUrl.
     * @param patches    List of patch objects where the results are stored.
     **/
    PMError readDir( const Url &baseUrl, const Pathname &patchPath,
                     std::list<PMYouPatchPtr> &patches );

    /**
     * Read patch info from file.
     *
     * @param path     File path of package info file.
     * @param fileName Name of patch file.
     * @param patches  List of patch objects where the results are stored.
     **/
    PMError readFile( const Pathname &path, const std::string &fileName,
                      std::list<PMYouPatchPtr> &patches );

    /**
     * Parse package info.
     *
     * @param packages String containing the package information.
     * @param patch    Patch the packages belong to.
     **/
    PMError parsePackages( const std::string &packages,
                           const PMYouPatchPtr &patch );

  protected:
    std::string tagValue( YOUPatchTagSet::Tags tag );
    std::string tagValue( YOUPackageTagSet::Tags tag );

    PMError createPackage( const PMYouPatchPtr &patch );

  private:
    CommonPkdParser::TagSet *_patchtagset;
    CommonPkdParser::TagSet *_packagetagset;
    
    std::list<std::string> *_patchFiles;

    PMYouPackageDataProviderPtr _packageProvider;

    PMYouPatchPaths *_paths;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchInfo_h
