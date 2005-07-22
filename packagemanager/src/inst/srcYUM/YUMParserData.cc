/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

File:       YUMParserData.cc

Author:     Michael Radziej <mir@suse.de>
Maintainer: Michael Radziej <mir@suse.de>

Purpose:    Boring constructors of YUM repository data structures
            and also boring output operators
            Don't read in a tired mood.
/-*/


#include <y2pm/YUMParserData.h>

using namespace YUM;
using namespace std;

YUMDependency::YUMDependency()
{ }

YUMDependency::YUMDependency(const std::string& name,
                             const std::string& flags,
                             const std::string& epoch,
                             const std::string& ver,
                             const std::string& rel,
                             const std::string& pre)
: name(name),
flags(flags),
epoch(epoch),
ver(ver),
rel(rel),
pre(pre)
{ };

YUMDirSize::YUMDirSize()
{ }

YUMDirSize::YUMDirSize(const std::string& path,
                       const std::string& sizeKByte,
                       const std::string& fileCount)
: path(path), sizeKByte(sizeKByte), fileCount(fileCount)
{ }

YUMRepomdData::YUMRepomdData()
{ }

YUMPrimaryData::YUMPrimaryData()
{ }

FileData::FileData()
{ }

FileData::FileData(const std::string &name,
                   const std::string &type)
: name(name), type(type)
{ }


YUMGroupData::YUMGroupData()
{ }

MultiLang::MultiLang()
{ }

MultiLang::MultiLang(const std::string& language,
                     const std::string& text)
: language(language), text(text)
{ }


MetaPkg::MetaPkg()
{ }

MetaPkg::MetaPkg(const std::string& type,
                 const std::string& name)
: type(type), name(name)
{ }

PackageReq::PackageReq()
{ }

PackageReq::PackageReq(const std::string& type,
                       const std::string& epoch,
                       const std::string& ver,
                       const std::string& rel,
                       const std::string& name)
: type(type), epoch(epoch), ver(ver), rel(rel), name(name)
{ }

ChangelogEntry::ChangelogEntry()
{ }

ChangelogEntry::ChangelogEntry(const std::string& author,
                               const std::string& date,
                               const std::string& entry)
: author(author), date(date), entry(entry)
{ }

                
YUMFileListData::YUMFileListData()
{ }

YUMOtherData::YUMOtherData()
{ }


/* Define pointer classes */

IMPL_BASE_POINTER(YUMRepomdData);
IMPL_BASE_POINTER(YUMPrimaryData);
IMPL_BASE_POINTER(YUMGroupData);
IMPL_BASE_POINTER(YUMFileListData);
IMPL_BASE_POINTER(YUMOtherData);

/* output operators */

namespace {
  /**
   * @short Generic stream output for lists of Ptrs
   * @param out the ostream where the output goes to
   * @param aList the list to output
   * @return is out
   */
  template<class T>
  ostream& operator<<(ostream &out, const list<T>& aList)
  {
    typedef typename list<T>::const_iterator IterType;
    for (IterType iter = aList.begin();
        iter != aList.end();
        ++iter) {
          if (iter != aList.begin())
            out << endl;
          ::operator<<(out,*iter);
        }
    return out;
  }
}



  /**
   * Join a list of strings into a single string
   * @param aList the list of strings
   * @param joiner what to put between the list elements
   * @return the joined string
   */
  string join(const list<string>& aList,
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


  
ostream& operator<<(ostream &out, const YUMDependency& data)
{
  out << data.name << " " << data.flags << " ";
  if (! data.epoch.empty())
    out << data.epoch << "-";
  out << data.ver << "-" << data.rel ;
  if (! data.pre.empty() && data.pre != "0")
    out << " (pre=" << data.pre << ")";
  return out;
}
  
ostream& operator<<(ostream &out, const YUMDirSize& data)
{
  out << data.path
    << ": " << data.sizeKByte << " kByte, "
    << data.fileCount << " files";
  return out;
}
    
ostream& operator<<(ostream &out, const FileData& data)
{
  out << data.name;
  if (! data.type.empty()) {
    out << ": " << data.type;
  }
  return out;
}

ostream& operator<<(ostream &out, const MultiLang& data)
{
  if (!data.language.empty())
    out << "[" << data.language << "] ";
  out << data.text;
  return out;
}

ostream& operator<<(ostream &out, const MetaPkg& data)
{
  out << "type: " << data.type
    << ", name: " << data.name;
  return out;
}

ostream& operator<<(ostream &out, const PackageReq& data)
{
  out << "[" << data.type << "] "
    << data.name
    << " " << data.epoch
    << "-" << data.ver
    << "-" << data.rel;
  return out;
}

ostream& operator<<(ostream &out, const ChangelogEntry& data)
{
  out << data.date
    << " - " << data.author << endl
    << data.entry;
  return out;
}

ostream& operator<<(ostream &out, const YUMRepomdData& data)
{
  out << "Repomd Data: " << endl
    << "  type: '" << data.type << "'" << endl
    << "  location: '" << data.location << "'" <<endl
    << "  checksumType: '" << data.checksumType << "'" << endl
    << "  checksum: '" << data.checksum << "'" << endl
    << "  timestamp: '" << data.timestamp << "'" << endl
    << "  openChecksumType: '" << data.openChecksumType << "'" << endl
    << "  openChecksum: '" << data.openChecksum << "'" << endl;
  return out;
}

ostream& operator<<(ostream &out, const YUMPrimaryData& data)
{
  out << "-------------------------------------------------" << endl
    << "Primary Data: " << endl
    << "name: '" << data.name << "'" << endl
    << "type: '" << data.type << "'" << endl
    << " arch: '" << data.arch << "'" << endl
    << " ver: '" << data.ver << "'" << endl
    << "checksumType: '" << data.checksumType << "'" << endl
    << "checksumPkgid: '" << data.checksumPkgid << "'" << endl
    << "checksum: '" << data.checksum << "'" << endl
    << "summary: '" << data.summary << "'" << endl
    << "description: '" << data.description << "'" << endl
    << "packager: '" << data.packager << "'" << endl
    << "url: '" << data.url << "'" << endl
    << "timeFile: '" << data.timeFile << "'" << endl
    << "timeBuild: '" << data.timeBuild << "'" << endl
    << "sizePackage: '" << data.sizePackage << "'" << endl
    << "sizeInstalled: '" << data.sizeInstalled << "'" << endl
    << "sizeArchive: '" << data.sizeArchive << "'" << endl
    << "location: '" << data.location << "'" << endl
    << "license: '" << data.license << "'" << endl
    << "vendor: '" << data.vendor << "'" << endl
    << "group: '" << data.group << "'" << endl
    << "buildhost: '" << data.buildhost << "'" << endl
    << "sourcerpm: '" << data.sourcerpm << "'" << endl
    << "headerStart: '" << data.headerStart << "'" << endl
    << "headerEnd: '" << data.headerEnd << "'" << endl
    << "provides:" << endl
    << data.provides << endl
    << "conflicts:" << endl
    << data.conflicts << endl
    << "obsoletes:" << endl
    << data.obsoletes << endl
    << "requires:" << endl
    << data.requires << endl
    << "files:" << endl
    << data.files << endl
    << "authors: " << join(data.authors,", ") << endl
    << "keywords: " << join(data.keywords,", ") << endl
    << "media: " << data.media << endl
    << "dirsizes: " << endl
    << data.dirSizes << endl
    << "freshen: " << endl
    << data.freshen << endl
    << "install-only: '" << data.installOnly << "'" << endl;
  return out;
}

ostream& operator<<(ostream &out, const YUMGroupData& data)
{
  out << "-------------------------------------------------" << endl
    << "Group Data: " << endl
    << "group-id: '" << data.groupId << "'" << endl
    << "name:" << endl
    << data.name << endl
    << "default: '" << data.default_  << "'" << endl
    << "user-visible: '" << data.userVisible  << "'" << endl
    << "description:" << endl
    << data.description << endl
    << "grouplist:" << endl
    << data.grouplist << endl
    << "packageList:" << endl
    << data.packageList << endl;
  return out;
}

ostream& operator<<(ostream &out, const YUMFileListData& data)
{
  out << "-------------------------------------------------" << endl
    << "File List: " << endl
    << "pkgid: " << data.pkgId << endl
    << "package: " << data.name << " "
    << data.epoch << "-" << data.ver << "-" << data.rel << endl
    << "files:" << endl
    << data.files << endl;
  return out;
}

ostream& operator<<(ostream& out, const YUMOtherData& data)
{
  out << "-------------------------------------------------" << endl
    << "Other: " << endl
    << "pkgid: " << data.pkgId
    << "package: " << data.name << " "
    << data.epoch << "-" << data.ver << "-" << data.rel << endl
    << "Changelog:" << endl
    << data.changelog << endl;
  return out;
}
