#include "YUMGroupDataIterator.h"
#include "YUMGroupDataPtr.h"
#include "YUMGroupData.h"
#include <list>

using namespace std;
using namespace YUM;

typedef list<YUMGroupData::MultiLang> MultiLangList;
typedef list<YUMGroupData::MetaPkg> MetaPkgList;
typedef list<YUMGroupData::PackageReq> PackageReqList;


static void debugPrint(const MultiLangList& aList)
{
  for (MultiLangList::const_iterator iter=aList.begin();
       iter != aList.end();
       ++iter) {
         cout << "  [" << iter->language << "] "
           << iter->text << endl;
       }
}

static void debugPrint(const PackageReqList& aList)
{
  for (PackageReqList::const_iterator iter=aList.begin();
       iter != aList.end();
       ++iter) {
         cout << "* type: '" << iter->type << "'" << endl
           << "  epoch: '" << iter->epoch << "'" << endl
           << "  ver: '" << iter->ver << "'" << endl
           << "  rel: '" << iter->rel << "'" << endl
           << "  name: '" << iter->name << "'" << endl;
       }
}


static void debugPrint(const MetaPkgList& aList)
{
  for (MetaPkgList::const_iterator iter=aList.begin();
       iter != aList.end();
       ++iter) {
       cout << "* type: '" << iter->type << "'" << endl
           << "  name: '" << iter->name << "'" << endl;
       }
}


static void debugPrint(const YUMGroupDataPtr data)
{
  cout << "-------------------------------------------------" << endl
    << "Group Data: " << endl
    << "group-id: '" << data->groupId << "'" << endl
    << "name:" << endl;
  debugPrint(data->name);
  cout << "default: '" << data->default_  << "'" << endl
    << "user-visible: '" << data->userVisible  << "'" << endl
    << "description:" << endl;
  debugPrint(data->description);
  cout << "grouplist:" << endl;
  debugPrint(data->grouplist);
  cout << "packageList:" << endl;
  debugPrint(data->packageList);
  cout << endl;
}

  

int main()
{
  for (YUMGroupDataIterator iter(cin,"");
       !iter.atEnd();
       ++iter) {
    debugPrint(*iter());
  }
  return 0;
}
         

