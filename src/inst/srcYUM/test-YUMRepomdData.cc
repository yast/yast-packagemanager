#include "YUMRepomdDataIterator.h"
#include "YUMRepomdDataPtr.h"
#include "YUMRepomdData.h"

using namespace std;

void debugPrint(const YUMRepomdDataPtr data)
{
  cout << "Repomd Data: " << endl
       << "  type: '" << data->type << "'" << endl
       << "  location: '" << data->location << "'" <<endl
       << "  checksumType: '" << data->checksumType << "'" << endl
       << "  checksum: '" << data->checksum << "'" << endl
       << "  timestamp: '" << data->timestamp << "'" << endl
       << "  openChecksumType: '" << data->openChecksumType << "'" << endl
       << "  openChecksum: '" << data->openChecksum << "'" << endl;
}


int main()
{
  for (YUMRepomdDataIterator iter(cin,"");
       !iter.atEnd();
       ++iter) {
    debugPrint(*iter());
  }
  return 0;
}
         

