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

   File:       InstSrcDescr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef InstSrcDescr_h
#define InstSrcDescr_h

#include <iosfwd>

#include <y2pm/InstSrcDescrPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDescr
/**
 *
 **/
class REP_CLASS(InstSrcDescr) {
  REP_BODY(InstSrcDescr)

  public:

    InstSrcDescr();

    virtual ~InstSrcDescr();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcDescr_h

