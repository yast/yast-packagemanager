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

#include <y2pm/InstSrc.h>
#include <y2pm/InstSrcDescrPtr.h>
#include <y2pm/MediaAccessPtr.h>

#include <y2pm/PMPackageDataProviderPtr.h>
#include <y2pm/PMSelectionDataProviderPtr.h>

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
     * True after _instSrc_propagate, false after _instSrc_withdraw
     **/
    bool _propagating;

    /**
     * Adjust backreferences to InstSrc. Call loadObjects().
     **/
    PMError _instSrc_attach( const InstSrcPtr & instSrc_r );

    /**
     * Clear backreferences to InstSrc.
     **/
    PMError _instSrc_detach();

    /**
     * Propagate Objects to Manager classes.
     **/
    PMError _instSrc_propagate();

    /**
     * Withdraw Objects from Manager classes.
     **/
    PMError _instSrc_withdraw();

  public:

    /**
     * Do nothing in Constructor. Wait for InstSrc to attach
     * and trigger actions to perform.
     **/
    InstSrcData();

    virtual ~InstSrcData();

  protected:

    /**
     * True if attached to an InstSrc
     **/
    bool attached() const { return _instSrc; }

    /**
     * Backreference to InstSrc (provided on attach, NULL after detach)
     **/
    InstSrcPtr _instSrc;

  protected:

    /**
     * InstSrcData is alowed to retrieve an Objects DataProviderPtr.
     **/
    static PMPackageDataProviderPtr   getDataProvider( const PMPackagePtr & obj_r );

    /**
     * InstSrcData is alowed to retrieve an Objects DataProviderPtr.
     **/
    static PMSelectionDataProviderPtr getDataProvider( const PMSelectionPtr & obj_r );

  protected:

    /**
     * Call concrete InstSrcData to load it's Object lists, either from
     * media or from cache (if present). Do not propaget them yet.
     *
     * Return E_ok if concrete InstSrcData is able to propagate Objects.
     *
     * InstSrc is attached.
     **/
    virtual PMError loadObjects() = 0;

    /**
     * Call concrete InstSrcData to propagate Objects to Manager classes.
     *
     * InstSrc is attached.
     **/
    virtual PMError propagateObjects();

    /**
     * Call concrete InstSrcData to withdraw Objects from Manager classes.
     *
     * Sync to cache (if there is one), clear all lists, and whatever is
     * necessary to go out of scope. InstSrc will detach immediately after
     * this.
     **/
    virtual PMError withdrawObjects();

  public:

    /**
     * Write data to cache, if necessary
     **/
    virtual PMError writeCache( const Pathname & cache_dir_r ) const;

  public:

    /**
     * Default for concrete InstSrcData providing no Selections
     **/
    virtual const std::list<PMSelectionPtr> & getSelections() const { return InstData::getSelections(); }

    /**
     * Default for concrete InstSrcData providing no Packages
     **/
    virtual const std::list<PMPackagePtr> &   getPackages()   const { return InstData::getPackages(); }

    /**
     * Default for concrete InstSrcData providing no Patches
     **/
    virtual const std::list<PMYouPatchPtr> &  getPatches()    const { return InstData::getPatches(); }

  public:

    std::ostream & dumpOn( std::ostream & str ) const;

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

