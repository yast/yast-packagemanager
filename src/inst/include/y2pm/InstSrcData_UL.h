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

  File:       InstSrcData_UL.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Concrete InstSrcData able to handle UnitedLinux style layout.

/-*/
#ifndef InstSrcData_UL_h
#define InstSrcData_UL_h

#include <iosfwd>

#include <y2pm/InstSrcData_ULPtr.h>
#include <y2pm/InstSrcData.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrcData_UL
/**
 * @short Concrete InstSrcData able to handle UnitedLinux style layout.
 **/
class InstSrcData_UL : virtual public Rep, public InstSrcData {
  REP_BODY(InstSrcData_UL);

  public:

    /**
     * default error class
     **/
    typedef InstSrcError Error;


  public:

    InstSrcData_UL();

    virtual ~InstSrcData_UL();

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
};

///////////////////////////////////////////////////////////////////

#endif // InstSrcData_UL_h

