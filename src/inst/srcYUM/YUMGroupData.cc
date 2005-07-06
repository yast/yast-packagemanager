#include "YUMGroupData.h"

using namespace std;
using namespace YUM;

YUMGroupData::MultiLang(const std::string& language,
                        const std::string& text)
: language(language), text(text)
{ }

YUMGroupData::MetaPkg(const std::string& type,
                      const std::string& text)
: type(type), text(text)
{ }

YUMGroupData::YUMGroupData()
{ }

