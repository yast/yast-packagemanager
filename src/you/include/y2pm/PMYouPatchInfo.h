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
#include <map>

#include <y2util/Url.h>
#include <y2util/Pathname.h>
#include <y2util/LangCode.h>
#include <y2util/FSize.h>

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
    PMYouPatchInfo( PMYouPatchPathsPtr paths, const std::string &lang = "" );

    /**
     * Destructor
     **/
    ~PMYouPatchInfo();

    /**
      Get directory file listing all available patches.
    */
    PMError getDirectory( bool useMediaDir );

    /**
     * Get patches from specified location.
     *
     * @param paths     Object holding the path location information.
     * @param patches   List of patch objects where the results are stored.
     * @param reload    If true, reload patch files from server.
     * @param checkSig  If true, check GPG signature of patch info files.
     **/
    PMError getPatches( std::list<PMYouPatchPtr> &patches,
                        bool reload = true, bool checkSig = true );
    
    /**
      Read patch info files from directory.

      @param baseUrl          Base of URL where patches are located.
      @param patches          List of patch objects where the results are stored.
      @param checkSig         If true, check GPG signature of patch info files.
      @param processMediaDir  If true, read patch file from media directory.
    **/
    PMError readDir( std::list<PMYouPatchPtr> &patches, bool reload = true,
                     bool checkSig = true, bool useMediaDir = true );

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

    /**
      Parse Files tag.
    */
    PMError parseFiles( const std::string &files, const PMYouPatchPtr &patch );

    /**
      Return language code used for displaying messages to the user.
    */
    LangCode langCode() const { return _lang; }

    /**
     * Translate standard lang code into full language name as used in patch
     * info files.
     */
    static std::string translateLangCode( const LangCode &lang );

    PMYouPackageDataProviderPtr packageDataProvider() const;

    PMError readDirectoryFile( const Pathname &,
                               std::list<std::string> &patchFiles );

    PMError processMediaDir();

  protected:
    std::string tagValueLocale ( YOUPatchTagSet::Tags tagIndex,
                                 std::istream &input );

    std::string tagValue( YOUPatchTagSet::Tags tagIndex, std::istream &input,
                          const std::string &locale = "" );
    std::string tagMultiValue( YOUPatchTagSet::Tags tagIndex, 
                               std::istream& input );

    std::string tagValue( YOUPackageTagSet::Tags tag );

    PMError createPackage( const PMYouPatchPtr &patch );

  private:
    YOUPatchTagSet _patchTagSet;
    YOUPackageTagSet _packageTagSet;
    
    PMYouPatchPathsPtr _paths;

    LangCode _lang;
    std::string _locale;
    static const std::string _defaultLocale;

    MediaAccess _media;

    PMYouPackageDataProviderPtr _packageDataProvider;
    
    std::list<std::string> _patchFiles;
    
    bool _doneMediaDir;
    bool _doneDirectory;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchInfo_h
