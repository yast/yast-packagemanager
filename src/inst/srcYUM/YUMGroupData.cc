#include "YUMGroupData.h"
#include <string>

using namespace std;
using namespace YUM;

YUMGroupData::MultiLang::MultiLang(const std::string& language,
                                   const std::string& text)
: language(language), text(text)
{ }

YUMGroupData::MetaPkg::MetaPkg(const std::string& type,
                               const std::string& name)
: type(type), name(name)
{ }

YUMGroupData::PackageReq::PackageReq( const std::string& type,
                                      const std::string& epoch,
                                      const std::string& ver,
                                      const std::string& rel,
                                      const std::string& name)
: type(type), epoch(epoch), ver(ver), rel(rel), name(name)
{ }

YUMGroupData::YUMGroupData()
{ }

