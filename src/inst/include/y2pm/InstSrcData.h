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

#include <y2pm/InstSrc.h>
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
     * True after _instSrc_propagate, false after _instSrc_withdraw
     **/
    bool _propagating;

    /**
     * Adjust backreferences to InstSrc.
     **/
    void _instSrc_attach( const InstSrcPtr & instSrc_r );

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
     * True if attached to an InstSrc
     **/
    bool attached() const { return _instSrc; }

    /**
     * Backreference to InstSrc (provided on attach, NULL after detach)
     **/
    InstSrcPtr _instSrc;

  protected:

    /**
     * Call concrete InstSrcData to propagate Objects to Manager classes.
     *
     * InstSrc is attached. If data are not yet present, get them from cache
     * (if there is one) or media.
     **/
    virtual void propagateObjects();

    /**
     * Call concrete InstSrcData to withdraw Objects from Manager classes.
     *
     * Sync to cache (if there is one), clear all lists, and whatever is
     * necessary to go out of scope. InstSrc will detach immediately after
     * this.
     **/
    virtual void withdrawObjects();

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

