#include <YUMDirSize.h>

using namespace std;
using namespace YUM;

YUMDirSize::YUMDirSize(const std::string& path,
                       const std::string& sizeKByte,
                       const std::string& fileCount)
: path(path), sizeKByte(sizeKByte), fileCount(fileCount)
{ }
