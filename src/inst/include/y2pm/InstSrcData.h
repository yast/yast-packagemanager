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

    Purpose: Base class for all concrete InstSrcData classes.

/-*/
#ifndef InstSrcData_h
#define InstSrcData_h

#include <iosfwd>
#include <list>

#include <y2util/Pathname.h>

#include <y2pm/InstSrcDataPtr.h>
#include <y2pm/InstSrcError.h>

#include <y2pm/InstSrcPtr.h>
#include <y2pm/InstSrcDescrPtr.h>
#include <y2pm/MediaAccessPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcData
/**
 * @short Base class for all concrete InstSrcData classes.
 **/
class InstSrcData: virtual public Rep {
  REP_BODY(InstSrcData);

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;


  public:

    InstSrcData();

    virtual ~InstSrcData();
#if 0
  private:
    InstData *_data;

  public:

    /**
     * constructor
     * initialization with new media
     */
    InstSrcData (MediaAccess *media);

    /**
     * constructor
     * initialization with known media
     */
    InstSrcData (const Pathname & contentcachefile);

    virtual ~InstSrcData();

  public:

    //-----------------------------
    // cache file handling
    /**
     * write media content data to cache file
     * @param pathname of corresponding InstSrcDescr cache file
     * @return pathname of written cache
     * writes content cache data to an ascii file
     */
    const Pathname writeCache (const Pathname &descrpathname);

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
    const std::list<PMSolvablePtr> *getSelections (void) const;

    /**
     * generate PMPackage objects for each Item on the source
     * @return list of PMPackagePtr on this source
     * */
    const std::list<PMPackagePtr> *getPackages (void) const;

    /**
     * generate PMSolvable objects for each patch on the source
     * @return list of PMSolvablePtr on this source
     */
    const std::list<PMSolvablePtr> *getPatches (void) const;
#endif

    virtual std::ostream & dumpOn( std::ostream & str ) const;

  public:

    /**
     * Any concrete InstSrcData must realize this, as it knows the expected
     * layout on the media. Expect MediaAccessPtr to be open and attached.
     *
     * Return the InstSrcDescr retrieved from the media via ndescr_r,
     * or NULL and PMError set.
     **/
    static PMError tryGetDescr( InstSrcDescrPtr & ndescr_r,
				MediaAccessPtr media_r, const Pathname & produduct_dir_r );
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcData_h

