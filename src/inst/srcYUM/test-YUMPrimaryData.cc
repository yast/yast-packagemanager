#include "YUMPrimaryDataIterator.h"
#include "YUMPrimaryDataPtr.h"
#include "YUMPrimaryData.h"

using namespace std;

typedef list<YUMDependency> DepList;

static void debugPrintDepList(const DepList& dlist)
{
  for (DepList::const_iterator iter=dlist.begin();
       iter != dlist.end();
       ++iter) {
    cout << "  " << iter->name << " " << iter->flags << " ";
    if (iter->epoch != string())
      cout << "epoch_" << iter->epoch << "-";
    cout << iter->ver << "-" << iter->rel << endl;
  }
}

typedef list<YUMPrimaryData::FileData> FileList;

static void debugPrintFileList(const FileList& flist)
{
  for (FileList::const_iterator iter=flist.begin();
       iter != flist.end();
       ++iter) {
    cout << "  " << "(" << iter->type << ") "
         << iter->name << endl;
  }
}

static void debugPrint(const YUMPrimaryDataPtr data)
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
  debugPrintDepList(data->provides);
  cout << "conflicts:" << endl;
  debugPrintDepList(data->conflicts);
  cout << "obsoletes:" << endl;
  debugPrintDepList(data->obsoletes);
  cout << "requires:" << endl;
  debugPrintDepList(data->requires);
  cout << "files:" << endl;
  debugPrintFileList(data->files);
}

  

int main()
{
  for (YUMPrimaryDataIterator iter(cin,"");
       !iter.atEnd();
       ++iter) {
    debugPrint(*iter());
  }
  return 0;
}
         

