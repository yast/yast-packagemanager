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

   File:       InstSrcDataCommonPkd.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef InstSrcDataCommonPkd_h
#define InstSrcDataCommonPkd_h

#include <iosfwd>

#include <y2pm/InstSrcDataCommonPkdPtr.h>
#include <y2pm/InstSrcData.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcDataCommonPkd
/**
 *
 **/
class InstSrcDataCommonPkd: virtual public Rep, public InstSrcData {
  REP_BODY(InstSrcDataCommonPkd)

  public:

    InstSrcDataCommonPkd(MediaInfoPtr media_r);

    virtual ~InstSrcDataCommonPkd();

  public:

    /** generate PMPackage objects for each Item on the source
     * @return list of PMPackagePtr on this source
     * */
    virtual std::list<PMPackagePtr> getPackages();

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcDataCommonPkd_h

