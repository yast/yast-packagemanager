#ifndef YUMGroupData_h
#define YUMGroupData_h

#include <string>
#include <list>
#include <y2util/RepDef.h>

namespace YUM {
class YUMGroupData : public CountedRep
  {
    REP_BODY(YUMGroupData);
    
    public:
    class MultiLang {
    public:
      MultiLang(const std::string& langugage,
                const std::string& text);
      std::string language;
      std::string text;
    };
    class MetaPkg {
    public:
      MetaPkg(const std::string& type,
              const std::string& name);
      std::string type;
      std::string name;
    };
    class PackageReq {
    public:
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
}

#endif
