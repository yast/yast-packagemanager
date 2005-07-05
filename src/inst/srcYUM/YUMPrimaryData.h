#ifndef YUMPrimaryData_h
#define YUMPrimaryData_h

#include "YUMRepomdDataIterator.h"
#include "YUMDependency.h"
#include <string>
#include <list>

class YUMPrimaryData : public CountedRep {
  REP_BODY(YUMPrimaryData)

public:

  YUMPrimaryData();

  struct FileData {
    std::string name;
    std::string type;
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
};



#endif
