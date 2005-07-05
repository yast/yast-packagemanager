#include "YUMPrimaryDataIterator.h"
#include "YUMPrimaryDataPtr.h"
#include "YUMPrimaryData.h"

using namespace std;

void debugPrint(const YUMPrimaryDataPtr data)
{
  cout << "Primary Data: " << endl
       << "  type: '" << data->type << endl
       << " arch: '" << data->arch << endl
       << " ver: '" << data->ver << endl
       << "checksumType: '" << data->checksumType << endl
       << "checksumPkgid: '" << data->checksumPkgid << endl
       << "checksum: '" << data->checksum << endl
       << "summary: '" << data->summary << endl
       << "description: '" << data->description << endl
       << "packager: '" << data->packager << endl
       << "url: '" << data->url << endl
       << "timeFile: '" << data->timeFile << endl
       << "timeBuild: '" << data->timeBuild << endl
       << "sizePackage: '" << data->sizePackage << endl
       << "sizeInstalled: '" << data->sizeInstalled << endl
       << "sizeArchive: '" << data->sizeArchive << endl
       << "location: '" << data->location << endl
       << "license: '" << data->license << endl
       << "vendor: '" << data->vendor << endl
       << "group: '" << data->group << endl
       << "buildhost: '" << data->buildhost << endl
       << "sourcerpm: '" << data->sourcerpm << endl
       << "headerStart: '" << data->headerStart << endl
       << "headerEnd: '" << data->headerEnd << endl
       << "provides:" << endl;
  debugPrint(provides);
  cout << "conflicts:" << endl;
  debugPrint(conflicts);
  cout << "obsoletes:" << endl;
  debugPrint(obsoletes);
  cout << "requires:" << endl;
  debugPrint(requires);
  cout << "files:" << endl;
  debugPrint(files);
}

typedef list<YUMDependency> DepList;

void debugPrint(const DepList& dlist)
{
  for (DepList::iterator iter=dlist.begin();
       iter != dlist.end();
       ++iter) {
    cout << "  " << iter->name << " " << flags << " ";
    if (iter->epoch != string())
      cout << "epoch_" << iter->epoch << -;
    cout << iter->ver << "-" << iter->rel << endl;
  }
}

typedef list<YUMPrimaryData::FileData> FileList;

void debugPrint(const FileList& flist)
{
  for (FileList::iterator iter=flist.begin();
       iter != flist.end();
       ++iter) {
    cout << "  " << "(" << iter->type << ") "
         << iter->name << endl;
  }
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
         

