#ifndef YUMRepomdData_h
#define YUMRepomdData_h

#include <string>
#include <YUMRepomdDataIterator.h>

typedef XMLNodeIterator<YUMRepomdDataPtr> YUMRepomdDataIterator;

class YUMRepomdData : public CountedRep {
  REP_BODY(YUMRepomdData);

public:
  std::string _type;
  std::string _location;
  std::string _checksumType;
  std::string _checksum;
  std::string _timestamp;
  std::string _openChecksumType;
  std::string _openChecksum;

  static YUMRepomdDataIteratorPtr parse(std::istream is);
};



#endif
