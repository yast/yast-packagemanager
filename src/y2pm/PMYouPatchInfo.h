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
 * @short facilitates reading and parsing patch information and patches from a given media.
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
      First, goes through the mediamap file (when useMediaDir is true).
      Then, reads directory file (or just listing the directory) and adds all
      found patchfiles to _settings. 

      @param useMediaDir if true, read mediamap file patches file in media directory.
    */
    PMError getDirectory( bool useMediaDir );

    /**
     * reads patch files
     *
     * @param patches   Return: List of patch objects
     * @return vector<PMYouPatchPtr> found patches
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
     * reads the mediamap file (which patches are on which media instance)
     * updates _settings
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

    /**
     * adds another package to the patch
     * @param patch Here the packages are added
     * @param strm  From this stream the tags are read. Old position will be
     *              restored.
     **/
    PMError createPackage( const PMYouPatchPtr &patch, std::istream& strm );

    void readMediaMap( const Pathname &file );

  private:
    YOUPatchTagSet _patchTagSet;
    YOUPackageTagSet _packageTagSet;
    
    PMYouSettingsPtr _settings;

    MediaAccess _media;

    PMYouPackageDataProviderPtr _packageDataProvider;
    
    /**
     * The last media for which the mediamap file was read successfully.
     * This attribute is handled exclusively by processMediaDir()
     **/
    Url _mediaDirLastVisited;

    bool _doneDirectory;

    int _totalPatchFileCount;

    std::map<std::string,int> _mediaMap;
};

///////////////////////////////////////////////////////////////////

#endif // PMYouPatchInfo_h
