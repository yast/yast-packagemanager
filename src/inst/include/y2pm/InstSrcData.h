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

#include <y2pm/InstData.h>

#include <y2pm/InstSrcDataPtr.h>
#include <y2pm/InstSrcError.h>

#include <y2pm/PMPackagePtr.h>
#include <y2pm/PMSolvablePtr.h>

#include <y2pm/InstSrcPtr.h>
#include <y2pm/InstSrcDescrPtr.h>
#include <y2pm/MediaAccessPtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcData
/**
 * @short Base class for all concrete InstSrcData classes.
 **/
class InstSrcData: virtual public Rep, public InstData {
  REP_BODY(InstSrcData);

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;

  private:

    /**
     * InstSrc triggers certain actions to perfom
     **/
    friend class InstSrc;

    /**
     * Backreference to InstSrc
     **/
    InstSrcPtr _instSrc;

    /**
     * True after _instSrc_propagate, false after _instSrc_withdraw
     **/
    bool _propagating;

    /**
     * Adjust backreferences to InstSrc.
     **/
    void _instSrc_atach( const InstSrcPtr & instSrc_r );

    /**
     * Clear backreferences to InstSrc.
     **/
    void _instSrc_detach();

    /**
     * Propagate Objects to Manager classes.
     **/
    void _instSrc_propagate();

    /**
     * Withdraw Objects from Manager classes.
     **/
    void _instSrc_withdraw();

  public:

    InstSrcData();

    virtual ~InstSrcData();

  protected:

    /**
     * Call concrete InstSrcData to propagate Objects to Manager classes.
     **/
    virtual void propagateObjects();

    /**
     * Call concrete InstSrcData to withdraw Objects from Manager classes.
     **/
    virtual void withdrawObjects();

    //--------------------------------------------------------------------
    // InstData interface  

    //-----------------------------
    // cache file handling
    /**
     * write media content data to cache file
     * @param pathname of corresponding InstSrcDescr cache file
     * @return pathname of written cache
     * writes content cache data to an ascii file
     */
    virtual const Pathname writeCache (const Pathname &descrpathname);

  public:

    /**
     * generate PMSelection objects for each selection on the source
     * @return list of PMSelectionPtr on this source
     */
    virtual const std::list<PMSelectionPtr>& getSelections (void) const = 0;

    /**
     * generate PMPackage objects for each Item on the source/target
     * @return list of PMPackagePtr on this source
     * */
    virtual const std::list<PMPackagePtr>& getPackages (void) const = 0;

    /**
     * generate PMSolvable objects for each patch on the source
     * @return list of PMSolvablePtr on this source
     */
    virtual const std::list<PMYouPatchPtr>& getPatches (void) const = 0;

    virtual std::ostream & dumpOn( std::ostream & str ) const;

    /**
     * Any concrete InstSrcData must realize this, as it knows the expected
     * layout on the media. Expect MediaAccessPtr to be open and attached.
     *
     * Return the InstSrcDescr retrieved from the media via ndescr_r,
     * or NULL and PMError set.
     **/
    static PMError tryGetDescr( InstSrcDescrPtr & ndescr_r,
				MediaAccessPtr media_r, const Pathname & produduct_dir_r );

    /**
     * Any concrete InstSrcData must realize this, as it knows the expected
     * layout on the media. Expect MediaAccessPtr to be open and attached.
     *
     * Return the InstSrcData retrieved from the media via ndescr_r,
     * or NULL and PMError set. <b>InstSrcData must not provide the objects
     * retieved to any Manager class.</b> This is to be done on explicit request
     * via propagateObjects only.
     **/
    static PMError tryGetData( InstSrcDataPtr & ndata_r,
			       MediaAccessPtr media_r, const Pathname & descr_dir_r );
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcData_h

