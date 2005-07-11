#include <YUMDependency.h>

using namespace YUM;

YUMDependency::YUMDependency(const std::string& name,
                             const std::string& flags,
                             const std::string& epoch,
                             const std::string& ver,
                             const std::string& rel)
  : name(name),
    flags(flags),
    epoch(epoch),
    ver(ver),
    rel(rel)
{ };
