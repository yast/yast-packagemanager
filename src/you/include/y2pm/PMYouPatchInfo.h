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
#include <y2pm/MediaAccess.h>

#include <y2pm/PMYouPatchPtr.h>
#include <y2pm/PMYouPackageDataProviderPtr.h>
#include <y2pm/PMYouPatchTags.h>
#include <y2pm/PMYouPatchPaths.h>

#include <y2pm/PMYouPatchInfoPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchInfo
/**
 * Patch information.
 **/
class PMYouPatchInfo : virtual public Rep {
  REP_BODY(PMYouPatchInfo);

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
    PMError getPatches( PMYouPatchPathsPtr paths,
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
    
    PMYouPackageDataProviderPtr _packageProvider;

    PMYouPatchPathsPtr _paths;

    MediaAccess _media;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchInfo_h
