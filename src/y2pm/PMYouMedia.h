/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SUSE LINUX AG |
\----------------------------------------------------------------------/

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Class representing a YOU media.

/-*/
#ifndef PMYOUMEDIA_H
#define PMYOUMEDIA_H

#include <y2pm/PMYouMediaPtr.h>

#include <y2pm/MediaAccess.h>

#include <string>

/**
  This class provides represents a media for YOU.
*/
class PMYouMedia : public CountedRep
{
  REP_BODY(PMYouMedia);

  public:
    /**
      Constructor.
    */
    PMYouMedia( MediaAccess & );

    /**
      Destructor
    */
    ~PMYouMedia();

    PMError readInfo( int number );

    bool isPartOf( const PMYouMediaPtr & );

    int number() const { return _number; }

    std::string vendor() const { return _vendor; }
    std::string id() const { return _id; }
    int count() const { return _count; }
    std::string flags() const { return _flags; }

    bool doubleSided() const;

  private:
    MediaAccess &_media;
  
    int _number;
  
    std::string _vendor;
    std::string _id;
    int _count;
    std::string _flags;
};

#endif
