#ifndef YUMGroupData_h
#define YUMGroupData_h

#include <string>
#include <y2util/RepDef.h>

namespace YUM {
class YUMGroupData : public CountedRep
  {
    REP_BODY(YUMRepomdData);
    
    public:
    class MultiLang {
    public:
      MultiLang(const std::string& langugage,
                const std::string& text);
      std::string language;
      std::string text;
    };
    class MetaPkg {
      MetaPkg(const std::string& type,
              const std::string& text);
      std::string type;
      std::string text;
    };

    std::string groupId;
    std::list<MultiLang> name;
    std::string default;
    std::string userVisible;
    std::list<MultiLang> description;
    std::list<MetaPkg> grouplist;
    std::string pkgType;
    std::string epoch;
    std::string ver;
    std::string rel;
  };
}

#endif