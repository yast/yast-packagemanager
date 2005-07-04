#ifndef YUM_Dependency_h
#define YUM_Dependency_h

struct YUMDependency {
  std::string name;
  std::string flags;
  std::string epoch;
  std::string ver;
  std::string rel;
};

#endif
