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

   File:       InstSrcData.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef InstSrcData_h
#define InstSrcData_h

#include <iosfwd>

#include <y2pm/InstSrcDataPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcData
/**
 *
 **/
class REP_CLASS(InstSrcData) {
  REP_BODY(InstSrcData)

  public:

    InstSrcData();

    virtual ~InstSrcData();

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcData_h

