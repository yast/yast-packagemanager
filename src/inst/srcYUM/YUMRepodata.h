/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                         (C) SuSE Linux Products GmbH |
\----------------------------------------------------------------------/

  File:       YUMRepodata.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef YUMRepodata_h
#define YUMRepodata_h

#include <iosfwd>

#include <y2util/Pathname.h>

#include "y2pm/PMError.h"
#include "y2pm/MediaAccessPtr.h"

///////////////////////////////////////////////////////////////////
namespace YUM
{ /////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  //
  //	CLASS NAME : Repodata
  /**
   *
   **/
  class Repodata
  {
  public:

    Repodata( const Pathname & repodataDir_r );

    ~Repodata();

    PMError
    update( MediaAccessPtr media_r, const Pathname & repodataDir_r );

    bool
    hasData() const
    { return _repodataId.size(); }

    std::string
    getId() const
    { return _repodataId; }

    Pathname
    primaryFile() const;

  public:

    static Pathname
    defaultRepodataDir()
    { return "repodata"; }

    static std::string
    defaultRepomd_Xml()
    { return "repomd.xml"; }

  private:

    Pathname    _repodataDir;
    std::string _repodataId;
  };
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////
} // namespace YUM
///////////////////////////////////////////////////////////////////
#endif // YUMRepodata_h
