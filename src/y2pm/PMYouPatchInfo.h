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

  Doc-State: mir at work
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
#include <y2pm/PMYouSettings.h>

#include <y2pm/PMYouPatchInfoPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMYouPatchInfo
/**
 * @short facilitates to read and parse patch information 
 * and patches from a given media.
 *
 **/



/**
 * Patch information.
 **/
class PMYouPatchInfo : public CountedRep {
  REP_BODY(PMYouPatchInfo);

  public:
    /**
      Constructor

      @param settings  Pointer to object representing the YOU settings
    */
    PMYouPatchInfo( PMYouSettingsPtr settings );

    /**
     * Destructor
     **/
    ~PMYouPatchInfo();

    /**
      Get directory file listing all available patches.
      ???

      @param useMediaDir If true, read patch file from media directory.
    */
    PMError getDirectory( bool useMediaDir );

    /**
     * same as processMediaDir()
     * Also saves last used server in sysconfig
     *
     * @param patches   Return: List of patch objects
     **/
    PMError getPatches( std::vector<PMYouPatchPtr> &patches );
    
    /**
      Read patch info files from directory.

      @param patches      Return: list of patches
      @param useMediaDir  If true, read patch file from media directory.
    **/
    PMError readDir( std::vector<PMYouPatchPtr> &patches,
                     bool useMediaDir = true );

    /**
      Read patch info from file.
     
      @param path     File path of package info file.
      @param fileName Name of patch file.
      @param patch    Return: patch
    */
    PMError readFile( const Pathname &path, const std::string &fileName,
                      PMYouPatchPtr &patch );

    /**
     * Parse package info, results go into _packageTagSet
     *
     * @param packages String containing the package information.
     * @param patch    Patch the packages belong to.
     **/
    PMError parsePackages( const std::string &packages,
                           const PMYouPatchPtr &patch );

    /**
     * Parse files and add them to a patch
     *
     * @param files  Names of files to parse (separated by \n)
     * @param patch  All parsed files will be added to the 
     *               target of this "ptr".
     **/
    PMError parseFiles( const std::string &files, const PMYouPatchPtr &patch );

    /** 
     * Parse Delta RPMs and add them to a patch
     * 
     * @param files  Names of files to parse (separated by \\n)
     * @param patch  All parsed files will be added to the 
     *               target of this "ptr".
     **/     
    PMError parseDeltas( const std::string &files, const PMYouPatchPtr &patch );

    /* ??? */
    PMYouPackageDataProviderPtr packageDataProvider() const;

    /**
     * reads a file and returns a list of all names except comments
     * (should probably better be static protected ???)
     *
     * @param Pathname Name of the file to parse
     * @param patchFile Return: List of names
     **/
    PMError readDirectoryFile( const Pathname &,
                               std::list<std::string> &patchFiles );

    /**
     * reads the media file (_settings->mediaPatchesFile()
     * updates _settings
     * ???
     **/
    PMError processMediaDir();

    /**
     * @return int media number from _mediaMap
     **/
    int mediaNumber( const PMYouPatchPtr & );

  protected:

    /**
     * gets tag from _patchTagSet by index ???
     * @param tagIndex Index of tag
     * @param input ??? mir is here ???
     **/
    std::string tagValueLocale ( YOUPatchTagSet::Tags tagIndex,
                                 std::istream &input );

    std::string tagValue( YOUPatchTagSet::Tags tagIndex, std::istream &input,
                          const std::string &locale = "" );
    std::string tagMultiValue( YOUPatchTagSet::Tags tagIndex, 
                               std::istream& input );

    std::string tagValue( YOUPackageTagSet::Tags tag );

    PMError createPackage( const PMYouPatchPtr &patch, std::istream& strm );

    void readMediaMap( const Pathname &file );

  private:
    YOUPatchTagSet _patchTagSet;
    YOUPackageTagSet _packageTagSet;
    
    PMYouSettingsPtr _settings;

    MediaAccess _media;

    PMYouPackageDataProviderPtr _packageDataProvider;
    
    bool _doneMediaDir;
    bool _doneDirectory;

    int _totalPatchFileCount;

    std::map<std::string,int> _mediaMap;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchInfo_h
