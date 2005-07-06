#ifndef YUM_Dependency_h
#define YUM_Dependency_h

#include <string>

class YUMDependency {
public:
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

#endif
