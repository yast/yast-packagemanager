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

   File:       DataOldSuSE.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

    Purpose:	Handle media content in common.pkd format

/-*/
#ifndef DataOldSuSE_h
#define DataOldSuSE_h

#include <iosfwd>

#include <y2pm/MediaAccess.h>

#include <y2pm/PMSolvable.h>
#include <y2pm/PMPackage.h>

#include <y2pm/DataOldSuSEPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : DataOldSuSE
/**
 *
 **/
class DataOldSuSE: virtual public Rep {
  REP_BODY(DataOldSuSE)

  private:
    const MediaAccess *media;

  public:

    DataOldSuSE (const MediaAccess *media);

    virtual ~DataOldSuSE();

  public:

    //-----------------------------
    // source content access

    /**
     * return the number of selections on this source
     */
    int numSelections() const;

    /**
     * return the number of packages on this source
     */
    int numPackages() const;

    /**
     * return the number of patches on this source
     */
    int numPatches() const;

    /**
     * generate PMSolvable objects for each selection on the source
     * @return list of PMSolvablePtr on this source
     */
    std::list<PMSolvablePtr> getSelections();
    
    /**
     * generate PMPackage objects for each Item on the source
     * @return list of PMPackagePtr on this source
     * */
    std::list<PMPackagePtr> getPackages();

    /**
     * generate PMSolvable objects for each patch on the source
     * @return list of PMSolvablePtr on this source
     */
    std::list<PMSolvablePtr> getPatches();

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // DataOldSuSE_h

