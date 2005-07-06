#include "YUMPrimaryData.h"

using namespace std;
using namespace YUM;

YUMPrimaryData::YUMPrimaryData()
{ }

YUMPrimaryData::FileData::FileData(const string& name,
                                   const string& type)
  : name(name), type(type)
{ }

