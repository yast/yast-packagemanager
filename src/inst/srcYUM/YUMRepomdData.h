#ifndef YUMRepomdData_h
#define YUMRepomdData_h

#include <string>
#include <y2util/RepDef.h>

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



#endif
