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

   File:       InstDescr.h

   Author:     Michael Andres <ma@suse.de>
   Maintainer: Michael Andres <ma@suse.de>

   Purpose:	installation description
		keeps all information to common to describe a source
		or a target.
/-*/
#ifndef InstDescr_h
#define InstDescr_h

#include <iosfwd>

#include <string>
#include <list>

#include <y2util/Pathname.h>
#include <y2pm/MediaAccess.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : InstDescr

class InstDescr : virtual public Rep {
  REP_BODY(InstDescr);

  public:

    /**
     * constructor
     * initialization with known media
     */
    InstDescr (const Pathname & descrcachefile);

    virtual ~InstDescr();

  private:
    // all this data is saved to a cache file when
    // this sources gets registered (for later use)

    /**
     * a source might be temporarely disabled
     */
    bool _activated;

    // data from suse/setup/descr/info
    std::string _vendor;
    std::string _version;
    int _release;
    std::string _name;
    std::string _product;
    std::string _type;

  public:

    /**
     * write media description to cache file
     * @return pathname of written cache
     * writes private data to an ascii file
     */
    const Pathname writeCache (void);

    const std::string& Vendor() const { return _vendor; }
    const std::string& Version() const { return _version; }
    int Release() const { return _release; }
    const std::string& Name() const { return _name; }
    const std::string& Product() const { return _product; }

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // InstDescr_h

