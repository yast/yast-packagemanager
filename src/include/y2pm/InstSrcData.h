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
#include <list>

#include <y2pm/InstSrcDataPtr.h>
#include <y2pm/PMPackagePtr.h>
#include <y2pm/MediaInfoPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcData
/**
 * Works on a MediaInfoPtr, has access to physical package
 * descriptions, creates PMSolvable objects from descriptions
 **/
class InstSrcData: virtual public Rep {
  REP_BODY(InstSrcData)

  protected:
    MediaInfoPtr _media;

  public:

    InstSrcData(MediaInfoPtr media_r);

    virtual ~InstSrcData();

  public:

    /** generate PMPackage objects for each Item on the source,
     * e.g. by reading common.pkd or scanning all rpms
     * @return list of PMPackagePtr on this source
     * */
    virtual std::list<PMPackagePtr> getPackages()=0;


    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcData_h

