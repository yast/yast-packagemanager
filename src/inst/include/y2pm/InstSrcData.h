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
class InstSrcData: virtual public Rep {
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
    virtual void propagateObjects() {}

    /**
     * Call concrete InstSrcData to withdraw Objects from Manager classes.
     **/
    virtual void withdrawObjects() {}

#if 1
  private:

    InstData *_data;

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
     * generate PMPackage objects for each Item on the source
     * @return list of PMPackagePtr on this source
     * */
    void setPackages(std::list<PMPackagePtr> *pacs) { _data->setPackages(pacs); }

    /**
     * generate PMPackage objects for each Item on the source
     * @return list of PMPackagePtr on this source
     * */
    const std::list<PMPackagePtr> *getPackages (void) const;

    /**
     * find list of packages
     * @return list of PMPackagePtr matching name ,[version] ,[release] ,[architecture]
     */
    const std::list<PMPackagePtr> findPackages (const std::list<PMPackagePtr> *packages, const std::string& name = "", const std::string& version = "", const std::string& release = "", const std::string& arch = "") const;

    /**
     * generate PMSelection objects for each Item on the source
     * @return list of PMSelection Ptr on this source
     * */
    void setSelections (std::list<PMSelectionPtr> *sels) { _data->setSelections(sels); }

    /**
     * generate PMSelection objects for each selection on the source
     * @return list of PMSelectionPtr on this source
     */
    const std::list<PMSelectionPtr> *getSelections (void) const;

    /**
     * generate PMSolvable objects for each patch on the source
     * @return list of PMSolvablePtr on this source
     */
    const std::list<PMSolvablePtr> *getPatches (void) const;
#endif

  public:

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

