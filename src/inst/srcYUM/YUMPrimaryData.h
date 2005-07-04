#ifndef YUMPrimaryData_h
#define YUMPrimaryData_h

#include <string>
#include <YUMRepomdDataIterator.h>
#include <YUMDependency.h>

typedef XMLNodeIterator<YUMPrimaryDataPtr> YUMPrimaryDataIterator;

class YUMPrimaryData : public CountedRep {
  REP_BODY(YUMRepomdData);

public:

  struct FileData {
    std::string type;
    std::string name;
  };

  std::string type;
  std::string arch;
  std::string arch;
  std::string epoch;
  std::string ver;
  std::string rel;
  std::string checksum_type;
  std::string checksum_pkgid;
  std::string checksum;
  std::string summary;
  std::string description;
  std::string packager;
  std::string url;
  std::string time_file;
  std::string time_build;
  std::string size_package;
  std::string size_installed;
  std::string size_archive;
  std::string location;
  std::string license;
  std::string vendor;
  std::string group;
  std::string buildhost;
  std::string sourcerpm;
  std::string header_start;
  std::string header_end;
  std::list<YUMDependency> provides;
  std::list<YUMDependency> conflicts;
  std::list<YUMDependency> obsoletes;
  std::list<YUMDependency> requires;
  std::list<FileData> files;

  static YUMPrimaryDataIteratorPtr parse(std::istream is);
};



#endif
