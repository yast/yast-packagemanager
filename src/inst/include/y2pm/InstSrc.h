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

   File:       InstSrc.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

/-*/
#ifndef InstSrc_h
#define InstSrc_h

#include <iosfwd>
#include <list>
#include <string>

#include <y2pm/InstSrcManager.h>

#include <y2pm/InstSrcPtr.h>
#include <y2pm/MediaInfoPtr.h>
#include <y2pm/InstSrcDescrPtr.h>
#include <y2pm/InstSrcDataPtr.h>
#include <y2pm/PMPackagePtr.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstSrc
/**
 *
 **/
class InstSrc: virtual public Rep {
  REP_BODY(InstSrc)

  protected:

    InstSrcManager::ISrcType _type;

    MediaInfoPtr        _media;
    InstSrcDescrPtr     _descr;
    InstSrcDataPtr      _data;
    std::list<PMPackagePtr> _pkglist;

    bool _activated;

  public:

    InstSrc( MediaInfoPtr media_r );

    virtual ~InstSrc();

  public:


    /** return the number of Items on this source */
    virtual int numItems()=0;

    /** do some initialisation */
    // TODO: return some error condition, enum?
    virtual bool Activate()=0;

    /** temporary deactivate source */
    virtual bool Deactivate()=0;

    /** clean up, e.g. remove all caches */
    virtual bool Erase()=0;


    /** &nbsp;
     * @return description of Installation source
     * */
    virtual const InstSrcDescrPtr getDescription() { return _descr; }


    /** generate PMPackage objects for each Item on the source
     * @return list of PMPackagePtr on this source
     * */
    virtual std::list<PMPackagePtr> getPackages()=0;
    
    /** determine which Items are immediately accessible, e.g.
     * don't require CD change or download
     *
     * @param l list of items which should be checked for
     * availability, if empty all known items are checked
     * 
     * @return list of immediately available items
     * */
    virtual std::list<PMPackagePtr> Immediate(const std::list<PMPackagePtr>& l)=0;

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstSrc_h

