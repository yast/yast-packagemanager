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

   File:       InstSrc.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef InstSrc_h
#define InstSrc_h

#include <iosfwd>

#include <y2pm/InstSrcManager.h>

#include <y2pm/InstSrcPtr.h>
#include <y2pm/MediaInfoPtr.h>
#include <y2pm/InstSrcDescrPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrc
/**
 *
 **/
class REP_CLASS(InstSrc) {
  REP_BODY(InstSrc)

  private:

    InstSrcManager::ISrcType _type;

    MediaInfoPtr        _media;
    InstSrcDescrPtr     _descr;

  public:

    InstSrc( MediaInfoPtr media_r );

    virtual ~InstSrc();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrc_h

