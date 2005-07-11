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

File:       YUMData.h

Author:     Michael Radziej <mir@suse.de>
Maintainer: Michael Radziej <mir@suse.de>

Purpose: Declares the various YUMData classes, which are rather dumb
         structure-like classes that hold the data of specific YUM
         repository files. The parsers (YUM...Parser) create these objects,
         and the YUM installation source use them to build more sophisticated
         objects.
/-*/

#ifndef YUMData_h
#define YUMData_h

#include <string>
#include <list>
#include <y2util/RepDef.h>

namespace YUM {

  /**
  * @short Holds dependency data
  */
  class YUMDependency {
  public:
    YUMDependency();
    YUMDependency(const std::string& name,
                  const std::string& flags,
                  const std::string& epoch,
                  const std::string& ver,
                  const std::string& rel);
    std::string name;
    std::string flags;
    std::string epoch;
    std::string ver;
    std::string rel;
  };


  /**
  * @short Holds data about how much space will be needed per directory
  **/
  class YUMDirSize
  {
  public:
    YUMDirSize();
    YUMDirSize(const std::string& path,
              const std::string& size,
              const std::string& fileCount);
    const std::string path;
    const std::string sizeKByte;
    const std::string fileCount;
  };



  /**
   * @short Holds the metadata about a YUM repository
   **/
  class YUMRepomdData : public CountedRep {
    REP_BODY(YUMRepomdData);
    
  public:
    YUMRepomdData();
    std::string type;
    std::string location;
    std::string checksumType;
    std::string checksum;
    std::string timestamp;
    std::string openChecksumType;
    std::string openChecksum;
  };

  /**
   * @short Describes a package in a YUM repository
   **/
  class YUMPrimaryData : public CountedRep {
    REP_BODY(YUMPrimaryData);
    
  public:
    
    YUMPrimaryData();
    
    struct FileData {
      std::string name;
      std::string type;
      FileData();
      FileData(const std::string &name,
               const std::string &type);
    };
    
    std::string type;
    std::string name;
    std::string arch;
    std::string epoch;
    std::string ver;
    std::string rel;
    std::string checksumType;
    std::string checksumPkgid;
    std::string checksum;
    std::string summary;
    std::string description;
    std::string packager;
    std::string url;
    std::string timeFile;
    std::string timeBuild;
    std::string sizePackage;
    std::string sizeInstalled;
    std::string sizeArchive;
    std::string location;
    std::string license;
    std::string vendor;
    std::string group;
    std::string buildhost;
    std::string sourcerpm;
    std::string headerStart;
    std::string headerEnd;
    std::list<YUMDependency> provides;
    std::list<YUMDependency> conflicts;
    std::list<YUMDependency> obsoletes;
    std::list<YUMDependency> requires;
    std::list<FileData> files;
    
    // SuSE specific data
    std::list<std::string> authors;
    std::list<std::string> keywords;
    std::string  media;
    std::list<YUMDirSize> dirSizes;
    std::list<YUMDependency> freshen;
    bool installOnly;
  };


  /**
  * @short Describes the groups in a YUM repository
  **/
  class YUMGroupData : public CountedRep
  {
    REP_BODY(YUMGroupData);
    
  public:
    class MultiLang {
    public:
      MultiLang();
      MultiLang(const std::string& langugage,
                const std::string& text);
      std::string language;
      std::string text;
    };
    class MetaPkg {
    public:
      MetaPkg();
      MetaPkg(const std::string& type,
              const std::string& name);
      std::string type;
      std::string name;
    };
    class PackageReq {
    public:
      PackageReq();
      PackageReq(const std::string& type,
                const std::string& epoch,
                const std::string& ver,
                const std::string& rel,
                const std::string& name);
      std::string type;
      std::string epoch;
      std::string ver;
      std::string rel;
      std::string name;
    };
    
    YUMGroupData();
    std::string groupId;
    std::list<MultiLang> name;
    std::string default_;
    std::string userVisible;
    std::list<MultiLang> description;
    std::list<MetaPkg> grouplist;
    std::list<PackageReq> packageList;
  };

#endif