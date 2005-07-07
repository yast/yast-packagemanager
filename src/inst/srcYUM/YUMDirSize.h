#ifndef YUMDirSize_h
#define YUMDirSize_h

#include <string>

namespace YUM {
  class YUMDirSize {
  public:
    YUMDirSize(const std::string& path,
               const std::string& size,
               const std::string& fileCount);
    const std::string path;
    const std::string sizeKByte;
    const std::string fileCount;
  };
}

#endif
