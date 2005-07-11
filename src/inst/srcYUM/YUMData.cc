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

File:       YUMData.cc

Author:     Michael Radziej <mir@suse.de>
Maintainer: Michael Radziej <mir@suse.de>

Purpose:    Boring constructors of YUM repository data structures.
            Don't read this in a tired mood.
/-*/


#include <YUMData.h>

using namespace YUM;

YUMDependency::YUMDependency()
{ }

YUMDependency::YUMDependency(const std::string& name,
                             const std::string& flags,
                             const std::string& epoch,
                             const std::string& ver,
                             const std::string& rel)
: name(name),
flags(flags),
epoch(epoch),
ver(ver),
rel(rel)
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

YUMPrimaryData::FileSize::FileSize()
{ }

YUMPrimaryData::FileData::FileData(const std::string &name,
                                   const std::string &type)
: name(name), type(type)
{ }


YUMGroupData::YUMGroupData()
{ }

YUMGroupData::MultiLang::MultiLang()
{ }

YUMGroupData::MultiLang::MultiLang(const std::string& langugage,
                                   const std::string& text)
: language(language), text(text)
{ }


YUMGroupData::MetaPkg::MetaPkg()
{ }

YUMGroupData::MetaPkg::MetaPkg(const std::string& type,
                               const std::string& name)
{ }

YUMGroupData::PackageReq::PackageReq()
{ }

YUMGroupData::PackageReq::PackageReq(const std::string& type,
                           const std::string& epoch,
                           const std::string& ver,
                           const std::string& rel,
                           const std::string& name)
: type(type), epoch(epoch), ver(ver), rel(rel), name(name)
{ }

