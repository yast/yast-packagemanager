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

    Purpose:	helper calls for InstSrcData
		is able to read and parse 'old format' SuSE media
		content descriptions
		namely suse/setup/descr/common.pkd for packages
		and suse/setup/descr/ *.sel for selections

		a possibly extension is reading
		update/<arch>/<product>/<version>/ * from an ftp patch repository

/-*/
#ifndef DataOldSuSE_h
#define DataOldSuSE_h

#include <iosfwd>

#include <y2pm/MediaAccess.h>

#include <y2pm/PMSolvable.h>
#include <y2pm/PMPackage.h>

#include <y2pm/DataOldSuSE.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : DataOldSuSE
/**
 *
 **/
class DataOldSuSE: virtual public Rep {
  REP_BODY(DataOldSuSE);

  private:
    std::list<PMSolvablePtr> *_selections;
    std::list<PMPackagePtr>  *_packages;
    std::list<PMSolvablePtr> *_patches;

  public:

    /**
     * constructor
     * read content data from media and
     *   fill _selections, _packages, and _patches
     */
    DataOldSuSE (MediaAccess *media);

    virtual ~DataOldSuSE();

  public:

    /**
     * @return ptr to list of selections on old SuSE source
     */
    const std::list<PMSolvablePtr> *getSelections (void) const;

    /**
     * @return ptr to list of packages on old SuSE source
     * */
    const std::list<PMPackagePtr> *getPackages (void) const;

    /**
     * @return ptr to list of packages on old SuSE source
     */
    const std::list<PMSolvablePtr> *getPatches (void) const;

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // DataOldSuSE_h

