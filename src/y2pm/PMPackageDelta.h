/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                          (C) SUSE AG |
\----------------------------------------------------------------------/

  File:       PMPackageDelta.h

  Author:     Ludwig Nussel <lnussel@suse.de>
  Maintainer: Ludwig Nussel <lnussel@suse.de>

  Purpose: Information about delta files.

/-*/
#ifndef PMPackageDelta_h
#define PMPackageDelta_h

#include <string>

#include <y2util/FSize.h>
#include <y2util/Date.h>

/**
  Information about delta files.
*/
class PMPackageDelta
{
  public:
    PMPackageDelta( const std::string &filename,
	const FSize &size,
	const std::string& md5sum,
	const PkgNameEd& ned,
	Date buildtime,
	const std::string& srcmd5,
	const std::string& seq
	)
      : _filename( filename ), _size( size ),
	  _md5sum(md5sum), _ned(ned),
	  _buildtime(buildtime), _srcmd5(srcmd5),
	 _seq(seq)
	 {}

    const std::string& filename() const { return _filename; }
    FSize size() const { return _size; }
    const std::string& md5sum() const { return _md5sum; }
    const PkgNameEd& ned() const { return _ned; }
    Date buildtime() const { return _buildtime; }
    const std::string& srcmd5() const { return _srcmd5; }
    const std::string& seq() const { return _seq; }

    // implementation in PMPackage.cc
    std::ostream& dumpOn(std::ostream& os) const;

  private:
    std::string _filename;
    FSize _size;
    std::string _md5sum;
    PkgNameEd _ned;
    Date _buildtime;
    std::string _srcmd5;
    std::string _seq;
};

std::ostream& operator<<(std::ostream& os, const PMPackageDelta& delta);

#endif
