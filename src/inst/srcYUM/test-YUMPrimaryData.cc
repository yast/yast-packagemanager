#include "YUMPrimaryDataIterator.h"
#include "YUMPrimaryDataPtr.h"
#include "YUMPrimaryData.h"

using namespace std;
using namespace YUM;

typedef list<YUMDependency> DepList;

static string join(const list<string>& aList,
                   const string& joiner)
{
  string res;
  for (list<string>::const_iterator iter = aList.begin();
       iter != aList.end();
       ++ iter) {
         if (iter != aList.begin())
           res += joiner;
         res += *iter;
       }
  return res;
}

static void debugPrint(const DepList& dlist)
{
  for (DepList::const_iterator iter=dlist.begin();
       iter != dlist.end();
       ++iter) {
    cout << "  " << iter->name << " " << iter->flags << " ";
    if (! (iter->epoch.empty() 
           && iter->ver.empty()
           && iter->rel.empty())) {
      if (iter->epoch != string())
        cout << "epoch_" << iter->epoch << "-";
      cout << iter->ver << "-" << iter->rel ;
    }
    cout << endl;
  }
}

typedef list<YUMPrimaryData::FileData> FileList;

static void debugPrint(const FileList& flist)
{
  for (FileList::const_iterator iter=flist.begin();
       iter != flist.end();
       ++iter) {
    cout << "  " << iter->name;
    if (! iter->type.empty()) {
      cout << ": " << iter->type;
    }
    cout << endl;
  }
}

typedef list<YUMDirSize> DirSizeList;

static void debugPrint(const DirSizeList& aList)
{
  for (DirSizeList::const_iterator iter=aList.begin();
       iter != aList.end();
       ++iter) {
       cout << "  " << iter->path
           << ": " << iter->sizeKByte << " kByte, "
           << iter->fileCount << " files"
           << endl;
       }
}

static void debugPrint(const YUMPrimaryDataPtr data)
{
  cout << "-------------------------------------------------" << endl
       << "Primary Data: " << endl
       << "name: '" << data->name << "'" << endl
       << "type: '" << data->type << "'" << endl
      << " arch: '" << data->arch << "'" << endl
      << " ver: '" << data->ver << "'" << endl
      << "checksumType: '" << data->checksumType << "'" << endl
      << "checksumPkgid: '" << data->checksumPkgid << "'" << endl
      << "checksum: '" << data->checksum << "'" << endl
      << "summary: '" << data->summary << "'" << endl
      << "description: '" << data->description << "'" << endl
      << "packager: '" << data->packager << "'" << endl
      << "url: '" << data->url << "'" << endl
      << "timeFile: '" << data->timeFile << "'" << endl
      << "timeBuild: '" << data->timeBuild << "'" << endl
      << "sizePackage: '" << data->sizePackage << "'" << endl
      << "sizeInstalled: '" << data->sizeInstalled << "'" << endl
      << "sizeArchive: '" << data->sizeArchive << "'" << endl
      << "location: '" << data->location << "'" << endl
      << "license: '" << data->license << "'" << endl
      << "vendor: '" << data->vendor << "'" << endl
      << "group: '" << data->group << "'" << endl
      << "buildhost: '" << data->buildhost << "'" << endl
      << "sourcerpm: '" << data->sourcerpm << "'" << endl
      << "headerStart: '" << data->headerStart << "'" << endl
      << "headerEnd: '" << data->headerEnd << "'" << endl
       << "provides:" << endl;
  debugPrint(data->provides);
  cout << "conflicts:" << endl;
  debugPrint(data->conflicts);
  cout << "obsoletes:" << endl;
  debugPrint(data->obsoletes);
  cout << "requires:" << endl;
  debugPrint(data->requires);
  cout << "files:" << endl;
  debugPrint(data->files);
  cout << "authors: " << join(data->authors,", ") << endl
    << "keywords: " << join(data->keywords,", ") << endl
    << "media: " << data->media << endl
    << "dirsizes: " << endl;
  debugPrint(data->dirSizes);
  cout << "freshen: " << endl;
  debugPrint(data->freshen);
  cout << "install-only: '" << data->installOnly << "'" << endl;
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
         

